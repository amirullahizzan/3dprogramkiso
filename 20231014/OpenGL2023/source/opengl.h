//===========================================================================
//!	@file	opengl.h
//!	@brief	OpenGL初期化処理
//===========================================================================
#pragma once

//!	OpenGLを初期化
//!	@param	[in]	hwnd	対象のウィンドウハンドル
//!	@retval	true	正常終了		(成功)
//!	@retval	false	エラー終了	(失敗)
bool OpenGL_setup(HWND hwnd);

//! OpenGL画面更新
void OpenGL_swapBuffer();

//!	OpenGLを解放
//!	@retval	true	正常終了		(成功)
//!	@retval	false	エラー終了	(失敗)
bool OpenGL_cleanup();
