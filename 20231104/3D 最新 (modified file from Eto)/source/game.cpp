//===========================================================================
//!	@file	game.cpp
//!	@brief	ゲームメインループ
//===========================================================================

//                        変数
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
    if(!texture) {return false;}

    return true;
}

//---------------------------------------------------------------------------
//	更新
//---------------------------------------------------------------------------
void GAME_update()
{
    //pointer lesson
    int  a = 10;
    int* p = &a;
    int**   p2 = &p;
    int***  p3 = &p2;
    int**** p4 = &p3;

    //int b = ****p4; 
    int b = *((*(*(*p4))));
    // 
    //-------------------------------------------------------------
    // 座標更新
    //-------------------------------------------------------------
    glLoadIdentity();

    if constexpr(true) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        gluLookAt(10.0f,    // x    カメラの位置(eye position)
                  5.0f,    // y
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

    //-------------------------------------------------
    //グリッドの描画
    //-------------------------------------------------
    
    //定数
    constexpr float SIZE = 64.0f;

    glBegin(GL_LINES);
    {
        //x軸
        glColor3ub(255, 0, 0);
        glVertex3f(-SIZE, 0.0f, 0.0f);
        glVertex3f(+SIZE, 0.0f, 0.0f);

        //Y軸
        glColor3ub(0, 255, 0);
        glVertex3f(0.0f, -SIZE, 0.0f);
        glVertex3f(0.0f, +SIZE, 0.0f);

        //Z軸
        glColor3ub(0, 0, 255);
        glVertex3f(0.0f, 0.0f, -SIZE);
        glVertex3f(0.0f, 0.0f, +SIZE);
    }
     glEnd();

     

     //定数
//    constexpr float SIZE = 64.0f;
     //グリッド線
     glBegin(GL_LINES);
    {
        

            ////X軸
            //glColor3ub(0, 0, 0);
            //glVertex3f(-SIZE, 0.0f, 0.0f );
            //glVertex3f(+SIZE, 0.0f, 0.0f);

            ////Y軸
            //glColor3ub(0, 0, 0);
            //glVertex3f(0.0f, -SIZE, 0.0f);
            //glVertex3f(0.0f, +SIZE, 0.0f);

            
            //白色
        //glColor3ub(0, 0, 0);
        glColor3ub(255, 255, 255);
        for(int i = -64; i <= 64; ++i) {
            //   /の方向
            glVertex3f(i, 0.0f, -SIZE);
            glVertex3f(i, 0.0f, +SIZE);
            //   \の方向
            glVertex3f(-SIZE, 0.0f, i);
            glVertex3f(+SIZE, 0.0f, i);
        }
        
    }
     glEnd();

     static float x = 0.0f;
     static float z = 0.0f;
     static float y = 0.0f;
 
     if(GetKeyState(VK_RIGHT) & 0x8000) {
        x++;
     }
     if(GetKeyState(VK_LEFT) & 0x8000) {
        x--;
     }

     if(GetKeyState(VK_UP) & 0x8000) {
        z--;
     }
     if(GetKeyState(VK_DOWN) & 0x8000) {
        z++;
     }
     

     //-----------------------------------------------
     // 四角形をテクスチャ付きで描画
     //-----------------------------------------------
     glLoadIdentity(); //元に戻す

     SetTexture(texture);
     //四角形を描画
     glBegin(GL_TRIANGLE_STRIP);
     {
        glColor3ub(255, 255, 255);
        //0        1
        //+--------+
        //|　　　／|
        //| 　／   |
        //|／      |
        //+--------+
        //2         3

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1, +1, 0); //左上

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(+1, +1, 0); //右上

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1, -1, 0); //左下

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(+1, -1, 0); //右上




     }
     glEnd();
     //描画が終わったら空っぽに
     SetTexture(nullptr);

     //-----------------------------------------------
     //ピラミッドの位置やスケール回転を指定
     //-----------------------------------------------

     
     glLoadIdentity();//元に戻す
     glTranslatef(x, y,z ); //移動
     glScalef(4.0f, 4.0f, 4.0f);         //スケール
     glRotatef(30.0f, 0.0f, 1.0f, 0.0f); //回転

     glBegin(GL_TRIANGLES);
     {
         //底面
        glColor3ub(255,255,0);
        glVertex3f(-1, 0, -1);
        glVertex3f(+1, 0, -1);
        glVertex3f(-1, 0, +1);

        glVertex3f(+1, 0, -1);
        glVertex3f(-1, 0, +1);
        glVertex3f(+1, 0, +1);

        //奥側面
        glColor3ub(255, 255, 255);
        glVertex3f(-1,0,-1);
        glVertex3f(+1,0,-1);
        glVertex3f(0, 1, 0); 

        //左側面
        glColor3ub(0, 0, 255);
        glVertex3f(-1, 0, -1);
        glVertex3f(-1, 0, +1);
        glVertex3f(0, 1, 0); 

        //右側面
        glColor3ub(0, 255, 0);
        glVertex3f(+1, 0, +1);
        glVertex3f(+1, 0, -1);
        glVertex3f(0, 1, 0); 

        //手前側面
        glColor3ub(255, 0, 0);
        glVertex3f(-1, 0, +1);
        glVertex3f(+1, 0, +1);
        glVertex3f(0, 1, 0); 
     }
     glEnd();
    
    // glLoadIdentity();//元に戻す
     

  
}

//---------------------------------------------------------------------------
//	解放
//---------------------------------------------------------------------------
void GAME_cleanup()
{
    texture.reset();   // テクスチャを解放(手動)
}
