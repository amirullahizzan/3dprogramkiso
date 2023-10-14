//===========================================================================
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


   // #define PI 3.141592f //old formula、cause it cannot be used outside of the scope. 
//ex: outside a header. while const float can be carried through header reference
    //const float PI = 3.141592f;

    static constexpr float PI = std::numbers::pi_v<float>;

   constexpr int DIV_COUNT = 32; //readonly
    //glBegin(GL_POLYGON);
    glBegin(GL_LINES);
    {
       
            glColor3ub(255, 255, 255);
        for(int i = 0; i < DIV_COUNT; i++) {
           // float angle0 = static_cast<float>(i) /DIV_COUNT; 
           //float angle1 = static_cast<float>(i + 1) / DIV_COUNT;
            //float angle0 = static_cast<float>(i) * 0.03125; 
            //float angle1 = static_cast<float>(i + 1) * 0.03125;
            
            float angle0 = static_cast<float>(i) * (1.0f / DIV_COUNT); // BOTH are defined values so won't affect performance when multiplied together
            float angle1 = static_cast<float>(i + 1) * (1.0f / DIV_COUNT);
            angle0 *= 2.0f * PI; //times 2Phi
            angle1 *= 2.0f * PI;

            float x0 = sinf(angle0);
            float y0 = cosf(angle0);

            float x1 = sinf(angle1);
            float y1 = cosf(angle1);

                glVertex2f(x0, y0); //start point
                glVertex2f(x1, y1); //end point
                //glVertex2f(angle1, angle1);
        }
  
    }


    glEnd();
}

//---------------------------------------------------------------------------
//	解放
//---------------------------------------------------------------------------
void GAME_cleanup()
{
    texture.reset();   // テクスチャを解放(手動)
}
