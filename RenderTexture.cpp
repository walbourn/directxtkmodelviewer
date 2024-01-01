//--------------------------------------------------------------------------------------
// File: RenderTexture.cpp
//
// Helper for managing offscreen render targets
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "RenderTexture.h"

#include "DirectXHelpers.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

RenderTexture::RenderTexture(DXGI_FORMAT format) noexcept :
    m_format(format),
    m_width(0),
    m_height(0)
{
}

void RenderTexture::SetDevice(_In_ ID3D11Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    {
        UINT formatSupport = 0;
        if (FAILED(device->CheckFormatSupport(m_format, &formatSupport)))
        {
            throw std::runtime_error("CheckFormatSupport");
        }

        constexpr UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
        if ((formatSupport & required) != required)
        {
#ifdef _DEBUG
            char buff[128] = {};
            sprintf_s(buff, "RenderTexture: Device does not support the requested format (%u)!\n", m_format);
            OutputDebugStringA(buff);
#endif
            throw std::runtime_error("RenderTexture");
        }
    }

    m_device = device;
}


void RenderTexture::SizeResources(size_t width, size_t height)
{
    if (width == m_width && height == m_height)
        return;

    if (m_width > UINT32_MAX || m_height > UINT32_MAX)
    {
        throw std::out_of_range("Invalid width/height");
    }

    if (!m_device)
        return;

    m_width = m_height = 0;

    // Create a render target
    CD3D11_TEXTURE2D_DESC renderTargetDesc(
        m_format,
        static_cast<UINT>(width),
        static_cast<UINT>(height),
        1, // The render target view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
        D3D11_USAGE_DEFAULT,
        0,
        1
    );

    ThrowIfFailed(m_device->CreateTexture2D(
        &renderTargetDesc,
        nullptr,
        m_renderTarget.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_renderTarget.Get(), "RenderTexture RT");

    // Create RTV.
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_format);

    ThrowIfFailed(m_device->CreateRenderTargetView(
        m_renderTarget.Get(),
        &renderTargetViewDesc,
        m_renderTargetView.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_renderTargetView.Get(), "RenderTexture RTV");

    // Create SRV.
    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_format);

    ThrowIfFailed(m_device->CreateShaderResourceView(
        m_renderTarget.Get(),
        &shaderResourceViewDesc,
        m_shaderResourceView.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_shaderResourceView.Get(), "RenderTexture SRV");

    m_width = width;
    m_height = height;
}


void RenderTexture::ReleaseDevice() noexcept
{
    m_renderTargetView.Reset();
    m_shaderResourceView.Reset();
    m_renderTarget.Reset();

    m_device.Reset();

    m_width = m_height = 0;
}

void RenderTexture::SetWindow(const RECT& output)
{
    // Determine the render target size in pixels.
    auto const width = size_t(std::max<LONG>(output.right - output.left, 1));
    auto const height = size_t(std::max<LONG>(output.bottom - output.top, 1));

    SizeResources(width, height);
}
