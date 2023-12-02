//===========================================================================
//!	@file	game.cpp
//!	@brief	ゲームメインループ
//===========================================================================

std::shared_ptr<Texture> texture;   //!< テクスチャ
#include "../vectormath/vectormath.h"

constexpr float PI = 3.141592f; //!< 円周率


//!ベクトルのなす角を求める
//! @param [in] a : ベクトルA
//! @param [in] b : ベクトルB
//! @return ラジアン角
float GetAngleBetweenVector(float3 a, float3 b)
{
    //Dot product is a scalar operation
    //→  →    →  →
    //a・b = |a||b| cosθ
    float cosine = dot(normalize(a), normalize(b));
    //restrict a value between a min and max value
    cosine       = std::clamp(cosine, -1.0f, +1.0f);
    return acosf(cosine);
}
void SetMatrix(const matrix& m)
{
    glLoadMatrixf((GLfloat*)&m);
}

void DrawArrow(const float3& p0, const float3 p1, const Color& c)
{
    glBegin(GL_LINES);

    glColor4ubv((GLubyte*)&c);
    {
        //中心軸
    glVertex3fv((GLfloat*)&p0);
    glVertex3fv((GLfloat*)&p1);

    float3 dir   = normalize(p1 - p0);
    float3 base = lerp(p0, p1, 0.8f);
    float3 right = cross(dir, float3(0.0f,1.0f,0.0f));
    if (dot(right,right) < float1(FLT_EPSILON))
    {
        right = cross(dir, float3(1.0f, 0.0f, 0.0f));
    }
    float3 up = cross(right, dir);

    right = normalize(right) * 0.15f;
    up = normalize(up) * 0.15f;

    float3 v[4]{base + right, base + up, base - right, base - up};

    glVertex3fv((GLfloat*)&v[0]);
    glVertex3fv((GLfloat*)&v[0]);
    
    glVertex3fv((GLfloat*)&v[1]);
    glVertex3fv((GLfloat*)&v[2]);

    glVertex3fv((GLfloat*)&v[2]);
    glVertex3fv((GLfloat*)&v[3]);

    glVertex3fv((GLfloat*)&v[3]);
    glVertex3fv((GLfloat*)&v[0]);

    glVertex3fv((GLfloat*)&v[0]);
    glVertex3fv((GLfloat*)&p1);

    glVertex3fv((GLfloat*)&v[1]);
    glVertex3fv((GLfloat*)&p1);

    glVertex3fv((GLfloat*)&v[2]);
    glVertex3fv((GLfloat*)&p1);

    glVertex3fv((GLfloat*)&v[3]);
    glVertex3fv((GLfloat*)&p1);

    }

    glEnd();
}

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
    if(texture == nullptr) {
        return false;
    }

    return true;

    //NVIDIA CUDA :
    // ほぼｃ＋＋
    // 用途。研究用途
    // 
    // 
    
    //denser data
    //float * 4 = 16bytes;
    //const float1 int1 uint1 //x 
    //const float2 int2 uint2   //xy
    //const float3 int3 uint3   //xyz
    //float4 int4 uint4   //xyzw
    {
        //初期化
        static float3 v3 = float3(0.0f, 0.0f, 0.0f);
        static float4 f4 = float4(v3, 1.0f);
    }

        //Cast
    static float4 v4 = float4(0,1,2,3); //xyzw
    //static float2 v2 = float2(v4.x, v4.y);
    static float2 v2 = v4.xy;
                  //v2 = float2(v4.zw);
        //Shuffle / Swizzle (no cost)
                  v2 = v4.yx;
                  v2 = (v4.zw);
                  v2 = v4.zz;

        //Normalize and Cos. Inner Product (内積) 
        // is a way to combine two vectors and get a single number
        // A * B = |A| |B| cosΘ
        // Normalized -> A * B = 1 *  1 cosΘ
        //float theta = acosf(); //put cosΘ in arg
                  //theta is radian
                  //if Inner product/Dot product is < 0 then Acute/Sharp angle
                  //if Inner product/Dot product is = 0 then Right angle
                  //if Inner product/Dot product is < 0 then Obtuse angle
}

//---------------------------------------------------------------------------
//	更新
//---------------------------------------------------------------------------
void GAME_update()
{
    //----------------------------------------------------------
    // 座標更新
    //----------------------------------------------------------
    glLoadIdentity();

    if constexpr(true) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        gluLookAt(10.0f,   // x    カメラの位置(eye position)
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

#if 0
    //---- 三角形を描画
    glBegin(GL_TRIANGLES);
    {
        glColor3ub(255, 0, 0);
        glVertex2f(0.0f, 0.0f);
        glColor3ub(0, 0, 255);
        glVertex2f(1.0f, 0.0f);
        glColor3ub(0, 255, 0);
        glVertex2f(1.0f, 1.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glColor3ub(0, 255, 0);

        glVertex2f(-0.5f, -1.0f);
        glVertex2f(0.0f, -1.0f);
        glVertex2f(0.0f, -0.5f);
    }
    glEnd();

    constexpr int DIV_COUNT = 32;   // 分割数

    //glBegin(GL_LINE_LOOP);
    glBegin(GL_POLYGON);
    {
        constexpr float PI = std::numbers::pi_v<float>;   // 円周率 3.141592.....

        for(int i = 0; i < DIV_COUNT; ++i) {
            float angle = static_cast<float>(i) * (1.0f / DIV_COUNT);   // 一周で1.0fになる
            angle *= 2.0f * PI;                                         // 一周で 2π になる = radian

            // 始点と終点の座標を計算する
            float x = cosf(angle);
            float y = sinf(angle);

            glVertex2f(x, y);
        }
    }
    glEnd();
#endif

    //static float x = 0.0f;
    //static float z = 0.0f;
    static float3 pyramidpos = float3(0,0,0);
    static float3 movepos3 = float3(0,0,0);
    if(GetKeyState(VK_RIGHT) & 0x8000) {
        //x++;
        movepos3.x += 1;
    }
    if(GetKeyState(VK_LEFT) & 0x8000) {
        //x--;
        movepos3.x -= 1;
    }
    if(GetKeyState(VK_UP) & 0x8000) {
        //z--;
        movepos3.z -= 1;
    }
    if(GetKeyState(VK_DOWN) & 0x8000) {
        //z++;
        movepos3.z += 1;
    }
    
    if (dot(movepos3,movepos3) > float1(FLT_EPSILON))
    {
        pyramidpos += normalize(movepos3) * 0.1f;
    }

    //pyramidpos += movepos3 * 0.1f;
    //pyramidpos += normalize(movepos3); NaN case v÷L Div by 0
    if (    !(movepos3.x == 0 && movepos3.y == 0 && movepos3.z == 0) ) //prevents divby0
    {
    pyramidpos += normalize(movepos3) * 0.1;
    }
    //----------------------------------------------------------
    // 四角形をテクスチャつきで描画
    //----------------------------------------------------------
    glLoadIdentity();   // 元に戻す
    glTranslatef(pyramidpos.x, pyramidpos.y, pyramidpos.z);

    //m = mul(matrix::rotateZ(PI * 0.5f), matrix::translate(5.0f, 0.0f, 0.0f));
    //glLoadMatrixf((GLfloat*)&m);

    SetTexture(texture);   // glBegin()の外側でのみ変更可
    glBegin(GL_TRIANGLE_STRIP);
    {
        glColor3ub(255, 255, 255);

        // [0]    [1]
        // +--------+
        // |      ／|
        // |   ／   |
        // |／      |
        // +--------+
        // [2]    [3]
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1, +1, 0);   // 左上

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(+1, +1, 0);   // 右上

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1, -1, 0);   // 左下

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(+1, -1, 0);   // 右下
    }
    glEnd();
    SetTexture(nullptr);   // 描画が終わったら元に戻す

    //----------------------------------------------------------
    // ピラミッドの位置やスケール回転を指定
    //----------------------------------------------------------
    
      matrix m = (matrix::identity());
    //m        = mul(matrix::rotateZ(3.141592f * 0.5), matrix::translate(5.0f,0.0,0.0f) );
      m = mul(m, matrix::rotateZ(3.141592f * 0.5));     //Z軸中心に９０度
      m = mul(m, matrix::translate(pyramidpos));      //5m 右へ移動
    //glLoadMatrixf((GLfloat*)&m);
    SetMatrix(m);

    //glLoadIdentity();                     // 元に戻す
    //glTranslatef(pyramidpos.x, pyramidpos.y, pyramidpos.z);   // 移動
    //glTranslatef(x, 0, z);                // 移動
    //glTranslatef(movepos3.x, movepos3.y, movepos3.z);   // 移動
    //glScalef(4.0f, 4.0f, 4.0f);           // スケール
    //glRotatef(30.0f, 0.0f, 1.0f, 0.0f);   // 回転

  

    //---- ピラミッド(Pylamid)を描画
    //     頂上(0, 1, 0)
    //            E
    //           ／＼
    //   (-1, 0, -1) ＼  (+1, 0, -1)
    //      A-----/-----B
    //  ---/ - - + - - /---
    //    C-----/-----D
    // (-1, 0, +1)     (+1, 0, +1)
    glBegin(GL_TRIANGLES);
    {
        // 底面
        glColor3ub(255, 255, 0);
        glVertex3f(-1, 0, -1);   // A
        glVertex3f(+1, 0, -1);   // B
        glVertex3f(-1, 0, +1);   // C
        glVertex3f(+1, 0, -1);   // B
        glVertex3f(-1, 0, +1);   // C
        glVertex3f(+1, 0, +1);   // D

        // 奥側面
        glColor3ub(255, 255, 255);
        glVertex3f(-1, 0, -1);   // A
        glVertex3f(+1, 0, -1);   // B
        glVertex3f(0, 1, 0);     // E

        // 左側面
        glColor3ub(0, 0, 255);
        glVertex3f(-1, 0, -1);   // A
        glVertex3f(-1, 0, +1);   // C
        glVertex3f(0, 1, 0);     // E

        // 右側面
        glColor3ub(0, 255, 0);
        glVertex3f(+1, 0, +1);   // D
        glVertex3f(+1, 0, -1);   // B
        glVertex3f(0, 1, 0);     // E

        // 手前側面
        glColor3ub(255, 0, 0);
        glVertex3f(-1, 0, +1);   // C
        glVertex3f(+1, 0, +1);   // D
        glVertex3f(0, 1, 0);     // E
    }
    glEnd();

    glLoadIdentity();   // 元に戻す

    DrawArrow(float3(0, 0, 0), float3(5, 5, -5), Color(255, 0, 0));
    DrawArrow(float3(0, 0, 0), float3(0, 5, 0), Color(255, 0, 255));

    {
        float3 axis_x = m._11_12_13;
        float3 axis_y = m._21_22_23;
        float3 axis_z = m._31_32_33;
        float3 p = m._41_42_43;

        DrawArrow(p,p+axis_x * 5,Color(255,0,0));
        DrawArrow(p, p + axis_y * 5, Color(0, 255, 0));
        DrawArrow(p, p + axis_z * 5, Color(0, 0, 255));

    }

    //LERP
  /*  static float t = 0.0f;

    pyramidpos = lerp(float3(0,0,0), float3(5,5,-5),t);
    t += 0.0005f;*/


    //----------------------------------------------------------
    // グリッドを描画
    //----------------------------------------------------------
    constexpr float SIZE = 64.0f;


    glBegin(GL_LINES);
    {
        // X軸
        glColor3ub(255, 0, 0);
        glVertex3f(-SIZE, 0.0f, 0.0f);
        glVertex3f(+SIZE, 0.0f, 0.0f);

        // Y軸
        glColor3ub(0, 255, 0);
        glVertex3f(0.0f, -SIZE, 0.0f);
        glVertex3f(0.0f, +SIZE, 0.0f);

        // Z軸
        glColor3ub(0, 0, 255);
        glVertex3f(0.0f, 0.0f, -SIZE);
        glVertex3f(0.0f, 0.0f, +SIZE);

        glColor3ub(255, 255, 255);   // 白色
        for(int i = -64; i <= 64; ++i) {
            glVertex3f(i, 0.0f, -SIZE);
            glVertex3f(i, 0.0f, +SIZE);

            glVertex3f(-SIZE, 0.0f, i);
            glVertex3f(+SIZE, 0.0f, i);
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
