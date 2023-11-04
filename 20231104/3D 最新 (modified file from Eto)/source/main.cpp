//===========================================================================
//!	@file	main.cpp
//!	@brief	アプリケーション開始
//===========================================================================

//---------------------------------------------------------------------------
//!	ウィンドウプロシージャ
//!	@param	[in]	hwnd	対象のウィンドウハンドル
//!	@param	[in]	message	ウィンドウメッセージ
//!	@param	[in]	wparam	パラメーター引数1
//!	@param	[in]	lparam	パラメーター引数2
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message) {
    case WM_PAINT:   //---- 再描画リクエスト
        {
            PAINTSTRUCT          ps;
            [[maybe_unused]] HDC hdc = BeginPaint(hwnd, &ps);

            EndPaint(hwnd, &ps);
        }
        return 0;
    case WM_DESTROY:   //---- ウィンドウ破棄
        PostQuitMessage(0);
        return 0;
    default:   //---- その他
        break;
    }
    //---- デフォルトのウィンドウプロシージャ
    return DefWindowProc(hwnd, message, wparam, lparam);
}

//---------------------------------------------------------------------------
//!	アプリケーション開始関数
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const char* titleName = "OpenGL 3D";   // タイトルバーのテキスト
    const char* className = "OpenGL";      // メインウィンドウクラス名

    //-------------------------------------------------------------
    // ウィンドウクラス登録
    //-------------------------------------------------------------
    WNDCLASSEX wcex{
        .cbSize = sizeof(WNDCLASSEX),                   // 自分自身の構造体サイズ
        .style  = CS_HREDRAW | CS_VREDRAW |             // 水平・垂直リサイズ時に再描画
                 CS_DBLCLKS |                           // ダブルクリック有効化
                 CS_OWNDC,                              // DC保持(OpenGLに必要)
        .lpfnWndProc   = WndProc,                       // ウィンドウプロシージャー
        .hInstance     = GetModuleHandle(nullptr),      // アプリケーションインスタンスハンドル
        .hCursor       = LoadCursor(NULL, IDC_CROSS),   // カーソル（十字）
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),    // 背景色（ブラシ）
        .lpszClassName = className,                     // ウィンドウクラス名
    };
    RegisterClassEx(&wcex);

    //-------------------------------------------------------------
    // ウィンドウの位置を画面中央にセンタリング
    //-------------------------------------------------------------
#if 1
    //---- ウィンドウ表示
    SIZE windowSize{.cx = 1280, .cy = 720};
    u32  style   = WS_OVERLAPPEDWINDOW;
    u32  styleEx = 0;
#else
    //---- フルスクリーン表示
    SIZE windowSize = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    u32  style      = WS_POPUP;
    u32  styleEx    = 0;
#endif

    //---- ウィンドウの大きさをウィンドウスタイルによって補正
    RECT windowRect{.left = 0, .top = 0, .right = windowSize.cx, .bottom = windowSize.cy};   // ウィンドウ矩形
    AdjustWindowRectEx(&windowRect, style, false, styleEx);

    //---- ウィンドウ表示位置をセンタリング
    s32 w = windowRect.right - windowRect.left;
    s32 h = windowRect.bottom - windowRect.top;
    s32 x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    s32 y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    //-------------------------------------------------------------
    // ウィンドウ作成
    //-------------------------------------------------------------
    HWND hwnd = CreateWindowEx(styleEx,     // 拡張ウィンドウスタイル
                               className,   // ウィンドウクラス名
                               titleName,   // タイトル名 (任意)
                               style,       // ウィンドウスタイル
                               x,
                               y,   // X,Y座標
                               w,
                               h,                          // 幅, 高さ
                               nullptr,                    // 親ウィンドウのハンドル
                               nullptr,                    // メニューのハンドル
                               GetModuleHandle(nullptr),   // アプリケーションインスタンスハンドル
                               nullptr);                   // WM_CREATEへの引数(任意)
    //---- 作成失敗した場合は終了
    if(hwnd == nullptr) {
        return 0;
    }

    //---- 初期値はウィンドウ表示OFF状態のため、ここでON
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    //=============================================================
    // [OpenGL] 初期化
    //=============================================================
    if(OpenGL_setup(hwnd) == false) {
        return 0;
    }

    //---- 【ゲーム】初期化
    MSG message{};
    if(GAME_setup() == true) {
        //-------------------------------------------------------------
        // メインメッセージループ:
        //-------------------------------------------------------------
        for(;;) {
            if(PeekMessage(&message, nullptr, 0, 0, PM_NOREMOVE)) {
                if(GetMessage(&message, nullptr, 0, 0) == 0) {
                    break;
                }
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
            else {
                //---- 【ゲーム】更新処理
                GAME_update();

                //=============================================================
                // [OpenGL]	画面更新
                //=============================================================
                OpenGL_swapBuffer();
            }
        }
    }

    //---- 【ゲーム】解放
    GAME_cleanup();

    //=============================================================
    // [OpenGL]	解放
    //=============================================================
    OpenGL_cleanup();   // OpenGLを解放

    return (int)message.wParam;
}
