//--------------------------------------------------------------------------------------
// File: pch.h
//
// Header for standard system include files.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once

#pragma warning(disable : 4619 4616 26812)
// C4619/4616 #pragma warning warnings
// 26812: The enum type 'x' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).

// Use the C++ standard templated min/max
#define NOMINMAX

#if defined(_XBOX_ONE) && defined(_TITLE)

#include <xdk.h>
#include <d3d11_x.h>

#else

#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

// DirectX apps don't need GDI
#define NODRAWTEXT

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d11_1.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#endif

#include <wrl/client.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <exception>
#include <iterator>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}

#include <CommonStates.h>
#include <DDSTextureLoader.h>
#include <Effects.h>
#include <GamePad.h>
#include <GraphicsMemory.h>
#include <Keyboard.h>
#include <Model.h>
#include <Mouse.h>
#include <PostProcess.h>
#include <PrimitiveBatch.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <VertexTypes.h>
