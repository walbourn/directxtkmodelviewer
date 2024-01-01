//--------------------------------------------------------------------------------------
// File: RenderTexture.h
//
// Helper for managing offscreen render targets
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#include <cstddef>

#include <wrl/client.h>

#include <DirectXMath.h>

namespace DX
{
    class RenderTexture
    {
    public:
        explicit RenderTexture(DXGI_FORMAT format) noexcept;

        RenderTexture(RenderTexture&&) = default;
        RenderTexture& operator= (RenderTexture&&) = default;

        RenderTexture(RenderTexture const&) = delete;
        RenderTexture& operator= (RenderTexture const&) = delete;

        void SetDevice(_In_ ID3D11Device* device);

        void SizeResources(size_t width, size_t height);

        void ReleaseDevice() noexcept;

        void SetWindow(const RECT& rect);

        ID3D11Texture2D* GetRenderTarget() const noexcept { return m_renderTarget.Get(); }
        ID3D11RenderTargetView* GetRenderTargetView() const noexcept { return m_renderTargetView.Get(); }
        ID3D11ShaderResourceView* GetShaderResourceView() const noexcept { return m_shaderResourceView.Get(); }

        DXGI_FORMAT GetFormat() const noexcept { return m_format; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Device>                m_device;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_renderTarget;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_shaderResourceView;

        DXGI_FORMAT                                         m_format;

        size_t                                              m_width;
        size_t                                              m_height;
    };
}
