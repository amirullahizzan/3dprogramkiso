﻿//===========================================================================
//!	@file	game.cpp
//!	@brief	ゲームメインループ
//===========================================================================

std::shared_ptr<Texture> texture;   //!< テクスチャ

//---------------------------------------------------------------------------
//	初期化
//!	@retval	true	正常終了    	(成功)
//!	@retval	false	エラー終了	(失敗)
//---------------------------------------------------------------------------
bool GAME_setup()
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);   // 画面クリアカラーの設定
    glClearDepth(1.0f);                        // Ｚバッファの初期化値
    glEnable(GL_DEPTH_TEST);                   // Ｚバッファを有効にする

    //----------------------------------------------------------
    // テクスチャを読み込む
    //----------------------------------------------------------
    texture = LoadTexture("data/sample.tga");

    return true;
}

//---------------------------------------------------------------------------
//	更新
//---------------------------------------------------------------------------
void GAME_update()
{
    //-------------------------------------------------------------
    // 座標更新
    //-------------------------------------------------------------
    glLoadIdentity();

    if constexpr(false) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        gluLookAt(0.0f,    // x    カメラの位置(eye position)
                  0.0f,    // y
                  10.0f,   // z
                  0.0f,    // x    注視点(look at)
                  0.0f,    // y
                  0.0f,    // z
                  0.0f,    // x    世界の上方向のベクトル(up)
                  1.0f,    // y
                  0.0f);   // z

        // もとに戻す
        glMatrixMode(GL_MODELVIEW);
    }

    //-------------------------------------------------------------
    // 描画
    //-------------------------------------------------------------

    //---- 画面クリア
    // 描画の開始のためにバックバッファとＺバッファを初期化します
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //---- 三角形を描画
    glBegin(GL_TRIANGLES);

    glColor3ub(255, 0, 0);
    glVertex2f(0.0f, 0.0f);
    glColor3ub(0, 255, 0);
    glVertex2f(1.0f, 0.0f);
    glColor3ub(0, 0, 255);
    glVertex2f(1.0f, 1.0f);

    glEnd();
}

//---------------------------------------------------------------------------
//	解放
//---------------------------------------------------------------------------
void GAME_cleanup()
{
    texture.reset();   // テクスチャを解放(手動)
}