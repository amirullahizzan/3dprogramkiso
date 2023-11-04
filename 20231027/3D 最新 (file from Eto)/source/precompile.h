﻿//===========================================================================
//!	@file	precompile.h
//!	@brief	プリコンパイルヘッダー
//!
//!	標準のシステムインクルードファイルのインクルードファイル、又は
//!	参照回数が多く、かつあまり変更されないプロジェクト専用の
//!	インクルードファイルを記述します。
//===========================================================================
#pragma once

#pragma warning(disable : 4201)   // 非標準の拡張機能が使用されています : 無名の構造体または共用体です。

//===========================================================================
// システム用ヘッダー
//===========================================================================

//--------------------------------------------------------------
// Windows ヘッダー ファイル
//--------------------------------------------------------------
#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN	※texture.cppでgdiplus利用時に定義するとエラーになる
#include <windows.h>

//--------------------------------------------------------------
// STL
//--------------------------------------------------------------
#include <algorithm>
#include <array>
#include <cmath>   // 算術演算
#include <iostream>
#include <memory>
#include <vector>
#include <numbers>  // PI

//--------------------------------------------------------------
//	OpenGL
//--------------------------------------------------------------
#include <GL/gl.h>   // OpenGL利用に必要
#include <GL/glu.h>   // OpenGLユーティリティーライブラリ glu*()

#pragma comment(lib, "opengl32.lib")   // OpenGL用ライブラリをリンク
#pragma comment(lib, "glu32.lib")      // glu関数用ライブラリをリンク

//--------------------------------------------------------------
//	hlslpp
//--------------------------------------------------------------
#include "../opensource/hlslpp/include/hlsl++.h"
using namespace hlslpp;

//===========================================================================
// プログラムに必要な追加ヘッダー
//===========================================================================
#include "typedef.h"

#include "opengl.h"
#include "texture.h"
#include "main.h"
#include "game.h"
