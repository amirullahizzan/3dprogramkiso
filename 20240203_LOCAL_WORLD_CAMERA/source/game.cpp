//===========================================================================
//!	@file	game.cpp
//!	@brief	ゲームメインループ
//===========================================================================

std::shared_ptr<Texture> texture;   //!< テクスチャ

constexpr float PI = 3.141592f;   //!< 円周率

//! ベクトルのなす角を求める
//! @param  [in]    a   ベクトルA
//! @param  [in]    b   ベクトルB
//! @return ラジアン角
float GetAngleBetweenVector(float3 a, float3 b)
{
    // →  →    →   →
    // a・b = |a| |b| cosθ
    float cosine = dot(normalize(a), normalize(b));

    cosine = std::clamp(cosine, -1.0f, +1.0f);
    return acosf(cosine);
}

void SetMatrix(const matrix& m)
{
    glLoadMatrixf((GLfloat*)&m);
}

void drawArrow(const float3& p0, const float3& p1, const Color& c)
{
    glBegin(GL_LINES);

    glColor4ubv((GLubyte*)&c);
    {
        //------------------------------------------------------
        // 中心軸
        //------------------------------------------------------
        glVertex3fv((GLfloat*)&p0);
        glVertex3fv((GLfloat*)&p1);

        //------------------------------------------------------
        // 四角錐
        //------------------------------------------------------
        float3 dir = normalize(p1 - p0);

        //---- 4点の位置を計算
        // 基準になる軸上の位置を求める
        float3 base = lerp(p0, p1, 0.8f);

        float3 right = cross(dir, float3(0.0f, 1.0f, 0.0f));   // 真上方向の基準軸で横方向を求める
        if(dot(right, right) < float1(FLT_EPSILON)) {
            // dirが真上方向だった場合は計算できないため、基準軸を変えてリトライ
            right = cross(dir, float3(1.0f, 0.0f, 0.0f));
        }

        float3 up = cross(right, dir);

        right = normalize(right) * 0.15f;
        up    = normalize(up) * 0.15f;

        float3 v[4]{
            base + right,   //
            base + up,      //
            base - right,   //
            base - up       //
        };

        glVertex3fv((GLfloat*)&v[0]);
        glVertex3fv((GLfloat*)&v[1]);

        glVertex3fv((GLfloat*)&v[1]);
        glVertex3fv((GLfloat*)&v[2]);

        glVertex3fv((GLfloat*)&v[2]);
        glVertex3fv((GLfloat*)&v[3]);

        glVertex3fv((GLfloat*)&v[3]);
        glVertex3fv((GLfloat*)&v[0]);

        // 斜めの部分
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

class Camera
{
public:
    //!const
    Camera() = default;   //automatically generate initialize value of uninitialized member variable data
    //!dest
    virtual ~Camera() = default;

    //!　更新
    void update();

    //設定
    //! @name 設定
    //!@{

    //! 位置を設定
    //! @param [in] position 位置
    void setPosition(const float3& position) { position_ = position; }
    //!注視点を設定
    //! @param [in] look_at 注視点
    void setLookAt(const float3& look_at) { look_at_ = look_at; }

    //!@}
    //! @name 参照
    //!@{

    //!位置を取得
    float3 getPosition() const { return position_; }
    //!注視点を取得
    float3 getLookAt() const { return look_at_; }

    //!ワールドを取得
    const matrix& getWorldMatrix() const { return mat_world_; }

    //!ビューを取得
    const matrix& getViewMatrix() const { return mat_view_; }

    //!投影行列を取得
    const matrix& getProjMatrix() const { return mat_proj_; }

    //!@}

private:
    float3 position_ = float3(0.0f, 1.0f, 5.0f);   //!位置
    float3 look_at_  = float3(0.0f, 0.0f, 0.0f);   //!注視点

    //Ctrl K + D to organize document
    matrix mat_world_ = matrix::identity();   //!ワールド行列 World Matrix
    matrix mat_view_  = matrix::identity();   //!ビュー行列 View Matrix
    matrix mat_proj_  = matrix::identity();   //!投影行列 Projection Matrix
};

//!更新
void Camera::update()
{
    //カメラおワールド行列を作成
    float3 axis_z = normalize(position_ - look_at_);
    float3 axis_x = normalize(cross(float3(0.0f, 1.0f, 0.0f), axis_z));
    float3 axis_y = cross(axis_z, axis_x);

    //same
    //mat_world_.axisVectorX()     = float4(axis_z, 0.0f);
    //mat_world_.axisVectorY()     = float4(axis_y, 0.0f);
    //mat_world_.axisVectorZ()     = float4(axis_z, 0.0f);
    //mat_world_.translateVector() = float4(position_, 1.0f);

    mat_world_._11_12_13_14 = float4(axis_x, 0.0f);
    mat_world_._21_22_23_24 = float4(axis_y, 0.0f);
    mat_world_._31_32_33_34 = float4(axis_z, 0.0f);
    mat_world_._41_42_43_44 = float4(position_, 1.0f);

    //ビュー行列 = カメラのワールド行列の逆行列
    mat_view_ = inverse(mat_world_);

    //投影行列
    // 一時に未使用
    // mat_projection_ = matrix::
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
}

//--------------------------------------------------------------
// カメラ情報
//--------------------------------------------------------------
Camera camera;

float3 eye_position = float3(20.0f, 5.0f, 20.0f);   // 位置
float3 look_at      = float3(0.0f, 0.0f, 0.0f);     // 注視点

float3 camera_dir      = normalize(float3(0.0f, 2.0, 5.0f));   // カメラがある方向 (正規化)
float  camera_distance = 10.0f;                                // カメラの距離

//--------------------------------------------------------------
// プレイヤー情報
//--------------------------------------------------------------
float3 position = float3(0.0f, 0.0f, 0.0f);

//---------------------------------------------------------------------------
//	更新
//---------------------------------------------------------------------------
void GAME_update()
{
    // カメラの操作

    // マウスの現在位置を取得(Win32 API) ※デスクトップ画面の現在位置
    static bool  initialized = false;   // 初回の初期化済かどうか
    static POINT last_pos;
    POINT        cursor_pos;
    GetCursorPos(&cursor_pos);
    {
        // 初回の初期化
        if(!initialized) {
            last_pos = cursor_pos;

            initialized = true;   // 初期化済
        }

        // マウスの移動量を計算
        constexpr float mouse_sensitivity = 0.01f;   // マウス感度

        float dx = float(cursor_pos.x - last_pos.x) * mouse_sensitivity;
        float dy = float(cursor_pos.y - last_pos.y) * mouse_sensitivity;

        //------------------------------------------------------
        // カメラを回転
        //------------------------------------------------------

        // 左右首振り
        matrix rotY = matrix::rotateY(-dx);
        camera_dir  = mul(float4(camera_dir, 0.0f), rotY).xyz;

        // 上下
        float3 axisX = normalize(cross(camera_dir, float3(0, 1, 0)));   // カメラの方向に合わせた真横方向
        matrix rotX  = matrix::rotateAxis(axisX, -dy);
        camera_dir   = mul(float4(camera_dir, 0.0f), rotX).xyz;
    }
    last_pos = cursor_pos;   // 次のフレームのために保存

    // カメラ設定
    look_at      = position;   // プレイヤーを見る
    eye_position = position + camera_dir * camera_distance;

    camera.setPosition(eye_position);
    camera.setLookAt(look_at);
    camera.update();

    //----------------------------------------------------------
    // 座標更新
    //----------------------------------------------------------
    glLoadIdentity();

    if constexpr(true) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // 投影行列（画角 (FOV)、アスペクト比、nearZ, farZ)
        gluPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

        glMultMatrixf((GLfloat*)&camera.getViewMatrix());

        //gluLookAt(eye_position.x,   // x    カメラの位置(eye position)
        //          eye_position.y,   // y
        //          eye_position.z,   // z
        //          look_at.x,        // x    注視点(look at)
        //          look_at.y,        // y
        //          look_at.z,        // z
        //          0.0f,             // x    世界の上方向のベクトル(up)
        //          1.0f,             // y
        //          0.0f);            // z

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

    static float3 dir = float3(0.0f, 0.0f, 1.0f);   // プレイヤーが向いている方向（内部的）
    static float3 appearrance_dir = float3(0.0f, 0.0f, 1.0f);   // プレイヤーが向いている方向 (見た目)

    // 移動量
    float3 move = float3(0.0f, 0.0f, 0.0f);

    if(GetKeyState(VK_RIGHT) & 0x8000) {
        move.x += 1.0f;
    }
    if(GetKeyState(VK_LEFT) & 0x8000) {
        move.x -= 1.0f;
    }
    if(GetKeyState(VK_UP) & 0x8000) {
        move.z -= 1.0f;
    }
    if(GetKeyState(VK_DOWN) & 0x8000) {
        move.z += 1.0f;
    }

    // 移動
    //  if(!(move.x == 0.0f && move.y == 0.0f && move.z == 0.0f)) {
    if(dot(move, move) > float1(FLT_EPSILON))   // dot(v, v) = 距離の2乗
    {
        dir = normalize(move);
        position += normalize(move) * 0.1f;
    }
    //----------------------------------------------------------
    // 四角形をテクスチャつきで描画
    //----------------------------------------------------------
    glLoadIdentity();   // 元に戻す

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
    // キャラクターの回転補間 Character interpolate rotation
    //----------------------------------------------------------
    {
        // 長さを1.0にすることで計算を簡略化
        dir             = normalize(dir);
        appearrance_dir = normalize(appearrance_dir);

        // appearrance_dir → dir に追従させる

        // 両者のベクトルのなす角を求める
        float theta = GetAngleBetweenVector(dir, appearrance_dir);

        // 回転軸を求める
        // 両者のベクトルで外積を求めると、180度の方向を起点にベクトルの向きが変わる
        // この軸ベクトルを中心に回転すると最短方向で回転できる
        float3 axis = float3(0.0f, 1.0f, 0.0f);

        float3 c = cross(appearrance_dir, dir);
        if(float1(FLT_EPSILON) < dot(c, c)) {   // cの長さが0.0になった場合は適用しないようにする
            axis = normalize(c);
        }

        // 軸中心に回転させる
        matrix rotY     = matrix::rotateAxis(axis, theta * 0.1f);
        appearrance_dir = mul(float4(appearrance_dir, 0.0f), rotY).xyz;
    }

    //----------------------------------------------------------
    // ピラミッドの位置やスケール回転を指定
    //----------------------------------------------------------
    matrix m = matrix::identity();

    //m = mul(m, matrix::rotateZ(PI * 0.25f));    // Z軸中心に45度
    //m = mul(m, matrix::translate(position));   // 5m右へ移動

    float3 axisZ = appearrance_dir;   // 表示用方向
    float3 axisX = normalize(cross(axisZ, float3(0.0f, 1.0f, 0.0f)));
    float3 axisY = normalize(cross(axisX, axisZ));

    // 作成した軸情報と位置情報で行列に直接代入して作成する
    m._11_12_13_14 = float4(axisX, 0.0f);
    m._21_22_23_24 = float4(axisY, 0.0f);
    m._31_32_33_34 = float4(axisZ, 0.0f);
    m._41_42_43_44 = float4(position, 1.0f);

    SetMatrix(m);

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

    //drawArrow(float3(0, 0, 0), float3(5, 5, -5), Color(255, 0, 0));
    //drawArrow(float3(0, 0, 0), float3(0, 5, 0), Color(255, 0, 255));

    // キャラクターの表示行列の軸を表示
    {
        float3 axis_x = m._11_12_13;
        float3 axis_y = m._21_22_23;
        float3 axis_z = m._31_32_33;
        float3 p      = m._41_42_43;

        drawArrow(p, p + axis_x * 2.5f, Color(255, 0, 0));
        drawArrow(p, p + axis_y * 2.5f, Color(0, 255, 0));
        drawArrow(p, p + axis_z * 2.5f, Color(0, 0, 255));

        drawArrow(p, p + dir * 5.0f, Color(255, 255, 255));             // dir=白
        drawArrow(p, p + appearrance_dir * 5.0f, Color(255, 0, 255));   // appearrance_dir=マゼンタ
    }

    // static float t = 0.0f;
    // position       = lerp(float3(0, 0, 0), float3(5, 5, -5), t);
    // t += 0.005f;

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
