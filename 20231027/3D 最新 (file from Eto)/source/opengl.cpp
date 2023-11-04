//===========================================================================
//!	@file	opengl.cpp
//!	@brief	OpenGL初期化処理
//===========================================================================

//---- グローバル変数（外部非公開）
namespace
{
HWND  gHwnd = nullptr;   //!< 対象のウィンドウハンドル
HDC   gHdc  = nullptr;   //!< デバイスコンテキスト
HGLRC gHrc  = nullptr;   //!< OpenGLリソースコンテキスト
}   // namespace

//---------------------------------------------------------------------------
//	OpenGLのピクセルフォーマットを設定
//!	@param	hdc		[in]	ディスプレイデバイスコンテキスト
//!	@retval	true	正常終了    	(成功)
//!	@retval	false	エラー終了	(失敗)
//---------------------------------------------------------------------------
static bool setupPixelFormatGL(HDC hdc)
{
    // clang-format off
    PIXELFORMATDESCRIPTOR desc{
		sizeof(PIXELFORMATDESCRIPTOR),								// この構造体のサイズ
		1,															// 構造体のバージョン番号(1)
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	// 特性フラグ(ダブルバッファ)
		PFD_TYPE_RGBA,												// ピクセルのカラーデータ(RGBAカラー or インデックスカラー)
		32,															// カラーのビット数
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0,  0, 0, 0,
		32,															// デプスバッファのピクセルあたりのビット数
		0,															// ステンシルバッファのピクセルあたりのビット数
		0,
		PFD_MAIN_PLANE,												// レイヤータイプ(Win32ではPFD_MAIN_PLANEである必要がある)
		0,
		0, 0, 0
	};
    // clang-format on

    // 現在のコンテキストにピクセルフォーマットを設定
    u32 pixelFormat;
    if((pixelFormat = ChoosePixelFormat(hdc, &desc)) == 0) {
        MessageBox(nullptr, "ピクセルフォーマットの選択に失敗しました.", "SetupPixelFormat()", MB_OK);
        return false;
    }
    // ピクセルフォーマットを設定
    if(!SetPixelFormat(hdc, pixelFormat, &desc)) {
        MessageBox(nullptr, "ピクセルフォーマットの設定に失敗しました.", "SetupPixelFormat()", MB_OK);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//!	OpenGLを初期化
//!	@param	[in]	hwnd	対象のウィンドウハンドル
//!	@retval	true	正常終了		(成功)
//!	@retval	false	エラー終了	(失敗)
//---------------------------------------------------------------------------
bool OpenGL_setup(HWND hwnd)
{
    //-------------------------------------------------------------
    // ウィンドウのデバイスコンテキストを取得
    //-------------------------------------------------------------
    HDC hdc = GetDC(hwnd);
    if(hdc == nullptr) {   // コンテキストが取れてない場合は失敗
        MessageBox(hwnd, "デバイスコンテキスト取得に失敗しました.", "InitializeOpenGL()", MB_OK);
        return false;
    }
    gHwnd = hwnd;
    gHdc  = hdc;

    //-------------------------------------------------------------
    // ピクセルフォーマットを設定
    //-------------------------------------------------------------
    if(setupPixelFormatGL(hdc) == false) {
        return false;
    }

    //-------------------------------------------------------------
    // OpenGLリソースハンドラを生成
    //-------------------------------------------------------------
    HGLRC hrc = wglCreateContext(hdc);
    if(hrc == 0) {   // OpenGLリソースハンドラを生成できなかったらエラー
        MessageBox(hwnd, "OpenGLリソースハンドラ生成に失敗しました.", "InitializeOpenGL()", MB_OK);
        return false;
    }
    gHrc = hrc;

    //---- OpenGLリソースとコンテキストを関連付け
    if(wglMakeCurrent(hdc, hrc) == false) {
        MessageBox(hwnd, "OpenGLリソースとコンテキスト関連付けに失敗しました.", "InitializeOpenGL()", MB_OK);
        return false;
    }

    //-------------------------------------------------------------
    // OpenGL初期設定
    //-------------------------------------------------------------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // 画面クリアカラーの設定

    glClearDepth(1.0f);        // Ｚバッファの初期化値
    glEnable(GL_DEPTH_TEST);   // Ｚバッファを有効にする

    return true;
}

//---------------------------------------------------------------------------
//! OpenGL画面更新
//---------------------------------------------------------------------------
void OpenGL_swapBuffer()
{
    SwapBuffers(gHdc);
}

//---------------------------------------------------------------------------
//!	OpenGLを解放
//!	@retval	true	正常終了		(成功)
//!	@retval	false	エラー終了	(失敗)
//---------------------------------------------------------------------------
bool OpenGL_cleanup()
{
    // リソースとデバイスコンテキストを解放
    wglMakeCurrent(0, 0);

    //-------------------------------------------------------------
    // コンテキストを削除
    //-------------------------------------------------------------
    if(wglDeleteContext(gHrc) == false) {
        MessageBox(gHwnd, "OpenGLの解放に失敗しました.", "cleanupOpenGL()", MB_OK);
        return false;
    }

    //---- デバイスコンテキストを解放する
    ReleaseDC(gHwnd, gHdc);

    gHwnd = nullptr;
    gHdc  = nullptr;
    gHrc  = nullptr;

    return true;
}
