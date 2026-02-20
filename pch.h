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
#error Support for Xbox One has been retired.
#else

#include <winsdkver.h>
#ifdef _M_ARM64
#define _WIN32_WINNT 0x0A00
#else
#define _WIN32_WINNT 0x0603
#endif
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

#define DIRECTX_TOOLKIT_IMPORT
#include <directxtk/CommonStates.h>
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/Effects.h>
#include <directxtk/GamePad.h>
#include <directxtk/GraphicsMemory.h>
#include <directxtk/Keyboard.h>
#include <directxtk/Model.h>
#include <directxtk/Mouse.h>
#include <directxtk/PostProcess.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/SpriteBatch.h>
#include <directxtk/SpriteFont.h>
#include <directxtk/VertexTypes.h>
