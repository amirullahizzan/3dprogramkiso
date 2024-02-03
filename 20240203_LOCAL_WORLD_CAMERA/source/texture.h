//===========================================================================
//!	@file	texture.h
//!	@brief	テクスチャ
//===========================================================================
#pragma once

//===========================================================================
//! テクスチャ
//===========================================================================
class Texture
{
public:
    //! コンストラクタ
    Texture() = default;

    //! OpenGLのテクスチャIDを取得
    virtual GLuint getTextureID() const = 0;

    //! 幅を取得
    virtual s32 getWidth() const = 0;

    //! 高さを取得
    virtual s32 getHeight() const = 0;
};

//! テクスチャを読み込み
//! @param  [in]    fileName    ファイル名
std::shared_ptr<Texture> LoadTexture(const char fileName[]);

//! テクスチャを設定
//!	@param	[in]	texture	テクスチャのポインタ(nullptr指定でOFF)
void SetTexture(const Texture* texture);

//!	テクスチャを設定
//!	@param	[in]	texture	テクスチャのポインタ
void SetTexture(std::shared_ptr<Texture>& texture);
