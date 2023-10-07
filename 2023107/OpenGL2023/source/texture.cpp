//===========================================================================
//!	@file	texture.cpp
//!	@brief	テクスチャ
//===========================================================================
#include <locale>
#include <vector>

#include <iostream>
#include <fstream>
#include <filesystem>

#define min std::min
#define max std::max

#pragma warning(push)
#pragma warning(disable : 4458)   //  'xxxxx' を宣言すると、クラス メンバーが隠蔽されます

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")   // リンク時に必要なライブラリ

#pragma warning(pop)
#undef min
#undef max

#pragma pack(push, 1)   // コンパイラーに変数の詰め込みを指示。パディング生成を抑制
struct HeaderTGA
{
    u8 id_;                //!<
    u8 colorMap_;          //!< カラーマップ有無   0:なし 1:あり
    u8 type_;              //!< 画像形式
                           //!<  0:イメージなし
                           //!<  1:インデックスカラー(256色)
                           //!<  2:フルカラー
                           //!<  3:白黒
                           //!<  9:(RLE圧縮)インデックスカラー(256色)
                           //!< 10:(RLE圧縮)フルカラー
                           //!< 11:(RLE圧縮)白黒
    u16 colorMapIndex_;    //!<
    u16 colorMapLength_;   //!<
    u8  colorMapSize_;     //!<
    u16 x_;                //!<
    u16 y_;                //!<
    u16 width_;            //!< 画像の幅
    u16 height_;           //!< 画像の高さ
    u8  bpp_;              //!< 色深度 (Bit per pixel) 8=256色 16:65536色 24:フルカラー 32:フルカラー+α
    u8  attribute_;        //!< 属性
                           //!< bit0-3:属性
                           //!< bit4  :格納方向 0:左から右  1:右から左
                           //!< bit5  :格納方向 0:下から上  1:上から下
                           //!< bit6,7:インターリーブ（使用不可）
};
#pragma pack(pop)

//===========================================================================
//! 画像クラス
//===========================================================================
class Image
{
public:
    //! コンストラクタ
    Image() = default;

    //! 初期化
    bool resize(s32 w, s32 h);

    //! ピクセル参照
    Color& pixel(s32 x, s32 y);

    //! 画像をUV座標で読み取り(バイリニアフィルタ)
    Color fetch(f32 u, f32 v);

private:
    std::vector<Color> image_;        //!< イメージ配列(幅×高さ の配列)
    s32                width_  = 0;   //!< 幅
    s32                height_ = 0;   //!< 高さ
};

//---------------------------------------------------------------------------
//! ピクセル参照
//---------------------------------------------------------------------------
Color& Image::pixel(s32 x, s32 y)
{
    x = std::clamp(x, 0, width_ - 1);
    y = std::clamp(y, 0, height_ - 1);
    return image_[y * width_ + x];
}

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Image::resize(s32 w, s32 h)
{
    image_.resize(w * h);

    width_  = w;
    height_ = h;
    return true;
}

//---------------------------------------------------------------------------
//! 画像をUV座標で読み取り(バイリニアフィルタ)
//---------------------------------------------------------------------------
Color Image::fetch(f32 u, f32 v)
{
    f32 fx = u * static_cast<f32>(width_);
    f32 fy = v * static_cast<f32>(height_);

    f32 s = std::modf(fx, &fx);
    f32 t = std::modf(fy, &fy);
    s32 x = static_cast<s32>(fx);
    s32 y = static_cast<s32>(fy);

    float4 c0 = float4(static_cast<f32>(pixel(x, y).r_),
                       static_cast<f32>(pixel(x, y).g_),
                       static_cast<f32>(pixel(x, y).b_),
                       static_cast<f32>(pixel(x, y).a_));

    float4 c1 = float4(static_cast<f32>(pixel(x + 1, y).r_),
                       static_cast<f32>(pixel(x + 1, y).g_),
                       static_cast<f32>(pixel(x + 1, y).b_),
                       static_cast<f32>(pixel(x + 1, y).a_));

    float4 c2 = float4(static_cast<f32>(pixel(x, y + 1).r_),
                       static_cast<f32>(pixel(x, y + 1).g_),
                       static_cast<f32>(pixel(x, y + 1).b_),
                       static_cast<f32>(pixel(x, y + 1).a_));

    float4 c3 = float4(static_cast<f32>(pixel(x + 1, y + 1).r_),
                       static_cast<f32>(pixel(x + 1, y + 1).g_),
                       static_cast<f32>(pixel(x + 1, y + 1).b_),
                       static_cast<f32>(pixel(x + 1, y + 1).a_));

    float4 c = lerp(lerp(c0, c1, s), lerp(c2, c3, s), t);

    c = clamp(c, 0.0f, 255.5f);

    Color color;
    color.r_ = static_cast<u8>(c.r);
    color.g_ = static_cast<u8>(c.g);
    color.b_ = static_cast<u8>(c.b);
    color.a_ = static_cast<u8>(c.a);

    return color;
}

//---------------------------------------------------------------------------
//! x より大きい最小の２のべき乗数を計算
//! @param  [in]    x   対象値
//---------------------------------------------------------------------------
s32 nextPowerOf2(u32 x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

class TGAStream
{
public:
    //! コンストラクタ
    TGAStream() {}

    //! デストラクタ
    virtual ~TGAStream() {}

    //! 1ピクセル読み込み
    virtual Color read() = 0;
};

//===========================================================================
//! 非圧縮データストリーム
//===========================================================================
class TGAStreamRAW : public TGAStream
{
public:
    TGAStreamRAW(std::ifstream& file)
        : file_(file)
    {
    }

    //! 1ピクセル読み込み
    virtual Color read()
    {
        u8 r, g, b, a;
        file_.read(reinterpret_cast<char*>(&b), 1);
        file_.read(reinterpret_cast<char*>(&g), 1);
        file_.read(reinterpret_cast<char*>(&r), 1);
        file_.read(reinterpret_cast<char*>(&a), 1);

        return Color(r, g, b, a);
    }

public:
    std::ifstream& file_;
};

//===========================================================================
//! RLE圧縮データストリーム
//===========================================================================
class TGAStreamRLE : public TGAStream
{
public:
    enum class State
    {
        Unknown,
        Compressed,
        Uncompressed
    };

    TGAStreamRLE(std::ifstream& file)
        : file_(file)
    {
    }

    //! 1ピクセル読み込み
    virtual Color read()
    {
        // 繰り返しフラグ
        if(count_ == 0) {
            u8 flagCount;
            file_.read(reinterpret_cast<char*>(&flagCount), 1);

            // 最上位ビットがセット(1のとき)の場合→「連続するデータの数」
            // 最上位ビットがセット(0のとき)の場合→「連続しないデータの数」
            if(flagCount & 0x80) {
                state_ = State::Compressed;

                u8 r, g, b, a;
                file_.read(reinterpret_cast<char*>(&b), 1);
                file_.read(reinterpret_cast<char*>(&g), 1);
                file_.read(reinterpret_cast<char*>(&r), 1);
                file_.read(reinterpret_cast<char*>(&a), 1);

                color_ = Color(r, g, b, a);
            }
            else {
                state_ = State::Uncompressed;
            }
            count_ = (flagCount & 127) + 1;
        }

        if(state_ == State::Uncompressed) {
            u8 r, g, b, a;
            file_.read(reinterpret_cast<char*>(&b), 1);
            file_.read(reinterpret_cast<char*>(&g), 1);
            file_.read(reinterpret_cast<char*>(&r), 1);
            file_.read(reinterpret_cast<char*>(&a), 1);

            color_ = Color(r, g, b, a);
        }

        count_--;

        return color_;
    }

public:
    s32   count_ = 0;
    State state_ = State::Unknown;
    Color color_ = Color(0, 0, 0, 0);

    std::ifstream& file_;
};

//===========================================================================
//! テクスチャ実装部
//===========================================================================
class TextureImpl final : public Texture
{
public:
    //! コンストラクタ
    TextureImpl() = default;

    //! 読み込み
    bool load(const char fileName[]);

    //! OpenGLのテクスチャIDを取得
    virtual GLuint getTextureID() const override;

    //! 幅を取得
    virtual s32 getWidth() const override;

    //! 高さを取得
    virtual s32 getHeight() const override;

    //! TGAファイルを読み込み
    bool loadTGA(const char fileName[]);

private:
    bool loadFromFile(const char fileName[]);

    // 代入禁止 / move禁止
    TextureImpl(const Texture&)        = delete;
    TextureImpl(Texture&&)             = delete;
    void operator=(const TextureImpl&) = delete;
    void operator=(TextureImpl&&)      = delete;

private:
    s32    width_  = 0;              //!< 幅
    s32    height_ = 0;              //!< 高さ
    GLuint id_     = 0xfffffffful;   //!< テクスチャID
};

//---------------------------------------------------------------------------
//! TGAファイルを読み込み
//---------------------------------------------------------------------------
bool TextureImpl::loadTGA(const char fileName[])
{
    std::ifstream file(fileName, std::ios::binary);
    if(!file.is_open()) {
        return false;
    }

    //-------------------------------------------------------------
    // ヘッダーを読み込む
    //-------------------------------------------------------------
    HeaderTGA header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if(header.bpp_ != 32) {
        MessageBox(nullptr, fileName, "32bitカラー形式ではありません.現時点ではサポートしていない形式です.", MB_OK);
        return false;
    }

    s32 width  = header.width_;
    s32 height = header.height_;

    //-------------------------------------------------------------
    // 圧縮モード判定
    //-------------------------------------------------------------
    std::unique_ptr<TGAStream> stream;

    if(header.type_ & (1 << 3)) {
        // RLE圧縮
        stream = std::make_unique<TGAStreamRLE>(file);
    }
    else {
        // 非圧縮
        stream = std::make_unique<TGAStreamRAW>(file);
    }

    //-------------------------------------------------------------
    // TGAファイルからイメージを取り出す
    //-------------------------------------------------------------
    Image image;
    image.resize(width, height);

    if(header.attribute_ & (1 << 5)) {
        // 通常
        for(s32 y = 0; y < height; y++) {
            for(s32 x = 0; x < width; x++) {
                image.pixel(x, y) = stream->read();
            }
        }
    }
    else {
        // 上下反転
        for(s32 y = height - 1; y >= 0; y--) {
            for(s32 x = 0; x < width; x++) {
                image.pixel(x, y) = stream->read();
            }
        }
    }

    //---- OpenGLで画像の転送
    // OpenGLでは2の乗数のサイズではないテクスチャをサポートしていないため
    // リサイズを行う
    s32 alignedW = nextPowerOf2(width);
    s32 alignedH = nextPowerOf2(height);

    std::vector<Color> alignedImage(alignedW * alignedH);

    for(s32 y = 0; y < alignedH; y++) {
        for(s32 x = 0; x < alignedW; x++) {
            f32 u = (f32)x / (f32)alignedW;
            f32 v = (f32)y / (f32)alignedH;

            alignedImage[y * alignedW + x] = image.fetch(u, v);
        }
    }

    // 画像転送
    glTexImage2D(GL_TEXTURE_2D,       // テクスチャタイプ
                 0,                   // ミップマップ段数 (0で無効)
                 GL_RGBA,             // 透明度あり
                 alignedW,            // 幅
                 alignedH,            // 高さ
                 0,                   // テクスチャボーダーON/OFF
                 GL_RGBA,             // テクスチャのピクセル形式
                 GL_UNSIGNED_BYTE,    // ピクセル1要素のサイズ
                 &alignedImage[0]);   // 画像の場所

    return true;
}

bool TextureImpl::loadFromFile(const char fileName[])
{
    // 文字コードをワイド文字列に変換
    // 【注意】本来はこの箇所は文字列バッファ長の考慮の他に文字列終端コードを処理するよりセキュアな対応が好ましいです。
    wchar_t path[MAX_PATH];
    size_t  pathLength = 0;

    setlocale(LC_ALL, "jpn");
    if(mbstowcs_s(&pathLength,   // [out]    変換された文字数
                  &path[0],   // [out]    変換されたワイド文字列を格納するバッファのアドレス(変換先)
                  MAX_PATH,            // [in]     出力側のバッファのサイズ(単位:文字数)
                  fileName,            // [in]     マルチバイト文字列のアドレス(変換元)
                  _TRUNCATE) != 0) {   // [in]     出力先に格納するワイド文字の最大数
        return false;
    }

    //  GDI+オブジェクト（画像展開に必要）
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR                    gdiplusToken;

    //---- GDI+の初期設定
    if(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok) {
        return false;
    }

    //-------------------------------------------------------------
    // 画像を読み込み
    //-------------------------------------------------------------

    //--- 画像ファイルを開く
    //  【対応画像形式】  BMP, JPEG, PNG, GIF, TIFF, WMF, EMF
    std::unique_ptr<Gdiplus::Bitmap> bitmap(Gdiplus::Bitmap::FromFile(path));

    if(!bitmap)
        return false;
    if(bitmap->GetLastStatus() != Gdiplus::Ok)
        return false;

    //---- 画像サイズ分の領域確保
    u32 width  = bitmap->GetWidth();    // 画像の幅
    u32 height = bitmap->GetHeight();   // 画像の高さ

    // サイズを保存しておく
    width_  = width;
    height_ = height;

    //---- 画像イメージ読み込み
    std::vector<u8> image(width * height * 4);   // 幅*高さ*RGBA
    for(u32 y = 0; y < height; y++) {
        for(u32 x = 0; x < width; x++) {
            Gdiplus::Color srcColor;
            bitmap->GetPixel(x, y, &srcColor);

            u8 r = srcColor.GetR();
            u8 g = srcColor.GetG();
            u8 b = srcColor.GetB();
            u8 a = srcColor.GetA();

            image[(y * width + x) * 4 + 0] = r;
            image[(y * width + x) * 4 + 1] = g;
            image[(y * width + x) * 4 + 2] = b;
            image[(y * width + x) * 4 + 3] = a;
        }
    }

    //---- OpenGLで画像の転送
    glTexImage2D(GL_TEXTURE_2D,      // テクスチャタイプ
                 0,                  // ミップマップ段数 (0で無効)
                 GL_RGBA,            // 透明度あり
                 width,              // 幅
                 height,             // 高さ
                 0,                  // テクスチャボーダーON/OFF
                 GL_RGBA,            // テクスチャのピクセル形式
                 GL_UNSIGNED_BYTE,   // ピクセル1要素のサイズ
                 image.data());      // 画像の場所

    //---- GDI+の解放
    Gdiplus::GdiplusShutdown(gdiplusToken);

    return true;
}

//---------------------------------------------------------------------------
//! 読み込み
//!	@param	[in]	fileName	画像ファイル名
//---------------------------------------------------------------------------
bool TextureImpl::load(const char fileName[])
{
    //-------------------------------------------------------------
    // (1) テクスチャIDを作成
    //-------------------------------------------------------------
    glGenTextures(1, &id_);

    //-------------------------------------------------------------
    // (2) IDをGPUに設定
    //-------------------------------------------------------------
    glBindTexture(GL_TEXTURE_2D, id_);

    //-------------------------------------------------------------
    // (3) テクスチャーのフィルター設定
    //-------------------------------------------------------------
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //-------------------------------------------------------------
    // (4) 画像イメージを転送
    //-------------------------------------------------------------
#if 0
	int	w = 16;
	int	h = 16;

	unsigned char	image[16 * 16][4];

	unsigned char*	pImage = &image[0][0];


	for( int y=0; y<h; y++ ) {
		for( int x=0; x<w; x++ ) {
			image[y * w + x][0] = rand();	// R
			image[y * w + x][1] = rand();	// G
			image[y * w + x][2] = rand();	// B
			image[y * w + x][3] = rand();	// A
		} 
	}
#endif

    if(strstr(fileName, ".tga") || strstr(fileName, ".TGA")) {
        return loadTGA(fileName);
    }
    return loadFromFile(fileName);
}

//---------------------------------------------------------------------------
//! OpenGLのテクスチャIDを取得
//---------------------------------------------------------------------------
GLuint TextureImpl::getTextureID() const
{
    return id_;
}

//---------------------------------------------------------------------------
//! 幅を取得
//---------------------------------------------------------------------------
s32 TextureImpl::getWidth() const
{
    return width_;
}

//---------------------------------------------------------------------------
//! 高さを取得
//---------------------------------------------------------------------------
s32 TextureImpl::getHeight() const
{
    return height_;
}

//---------------------------------------------------------------------------
//! テクスチャを読み込み
//---------------------------------------------------------------------------
std::shared_ptr<Texture> LoadTexture(const char fileName[])
{
    auto p = std::make_shared<TextureImpl>();

    if(!p->load(fileName)) {
        p.reset();
    }
    return p;
}

//---------------------------------------------------------------------------
//!	テクスチャを設定
//---------------------------------------------------------------------------
void SetTexture(const Texture* texture)
{
    if(texture) {
        //---- テクスチャがある場合はテクスチャマッピングを有効にして設定
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
    }
    else {
        //---- テクスチャがない場合はテクスチャマッピングを無効化
        glDisable(GL_TEXTURE_2D);
    }
}

//---------------------------------------------------------------------------
//!	テクスチャを設定
//---------------------------------------------------------------------------
void SetTexture(std::shared_ptr<Texture>& texture)
{
    SetTexture(texture.get());
}
