//
// Game.cpp -
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

static const float ROTATION_GAIN = 0.004f;

// Constructor.
Game::Game() :
    m_window(0),
#ifdef _XBOX_ONE
    m_outputWidth(1920),
    m_outputHeight(1080),
#else
    m_outputWidth(800),
    m_outputHeight(600),
#endif
    m_featureLevel(D3D_FEATURE_LEVEL_10_0),
    m_gridScale(10.f),
    m_fov(XM_PI / 4.f),
    m_zoom(1.f),
    m_distance(10.f),
    m_farPlane(10000.f),
    m_sensitivity(1.f),
    m_gridDivs(20),
    m_showHud(true),
    m_showCross(true),
    m_showGrid(false),
    m_usingGamepad(false),
    m_wireframe(false),
    m_reloadModel(false),
    m_lhcoords(true),
    m_fpscamera(false),
    m_selectFile(0),
    m_firstFile(0)
{
    m_clearColor = Colors::Black.v;
    m_uiColor = Colors::Yellow.v;

    *m_szModelName = 0;
    *m_szStatus = 0;
    *m_szError = 0;
}

// Initialize the Direct3D resources required to run.
#ifdef _XBOX_ONE
void Game::Initialize(IUnknown* window)
{
    m_window = window;
#else
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);
#endif

    m_ballCamera.SetWindow(m_outputWidth, m_outputHeight);
    m_ballModel.SetWindow(m_outputWidth, m_outputHeight);

    CreateDevice();

    CreateResources();

    m_gamepad.reset(new GamePad);

    m_keyboard.reset(new Keyboard);

    m_mouse.reset(new Mouse);
#ifndef _XBOX_ONE
    m_mouse->SetWindow(window);
#endif
}

// Executes basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

   Render();
}

// Updates the world
void Game::Update(DX::StepTimer const& timer)
{
    if (m_reloadModel)
        LoadModel();

    float elapsedTime = float(timer.GetElapsedSeconds());

    float handed = (m_lhcoords) ? 1.f : -1.f;

    auto gpad = m_gamepad->GetState(0);
    if (gpad.IsConnected())
    {
        m_usingGamepad = true;

        m_gamepadButtonTracker.Update(gpad);

        if (!m_fileNames.empty())
        {
            if (m_gamepadButtonTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED)
            {
                --m_selectFile;
                if (m_selectFile < 0)
                    m_selectFile = int(m_fileNames.size()) - 1;
            }
            else if (m_gamepadButtonTracker.dpadDown == GamePad::ButtonStateTracker::PRESSED)
            {
                ++m_selectFile;
                if (m_selectFile >= int(m_fileNames.size()))
                    m_selectFile = 0;
            }
            else if (gpad.IsAPressed())
            {
                swprintf_s(m_szModelName, L"D:\\%ls", m_fileNames[m_selectFile].c_str());
                m_selectFile = m_firstFile = 0;
                m_fileNames.clear();
                LoadModel();
            }
            else if (gpad.IsBPressed())
            {
                m_selectFile = m_firstFile = 0;
                m_fileNames.clear();
            }
        }
        else
        {
            // Translate camera
            Vector3 move;

            if (m_fpscamera)
            {
                move.x = gpad.thumbSticks.leftX;
                move.z =  gpad.thumbSticks.leftY * handed;
            }
            else
            {
                move.z = gpad.thumbSticks.leftY * handed;
            }

            if (gpad.IsDPadUpPressed())
            {
                move.y += 1.f;
            }
            else if (gpad.IsDPadDownPressed())
            {
                move.y -= 1.f;
            }

            if (gpad.IsDPadLeftPressed())
            {
                move.x -= 1.f;
            }
            else if (gpad.IsDPadRightPressed())
            {
                move.x += 1.f;
            }

            Matrix im;
            m_view.Invert(im);
            move = Vector3::TransformNormal(move, im);

            // Rotate camera
            if (m_fpscamera)
            {
                m_viewRot = Quaternion::CreateFromAxisAngle(Vector3::Right, gpad.thumbSticks.rightY * XM_PI / 2.f * handed);
                m_viewRot  *= Quaternion::CreateFromAxisAngle(Vector3::Up, -gpad.thumbSticks.rightX * XM_PI / 2.f * handed);
                m_viewRot.Normalize();
            }
            else
            {
                Vector3 orbit(gpad.thumbSticks.rightX, gpad.thumbSticks.rightY, gpad.thumbSticks.leftX);
                orbit *= elapsedTime * m_sensitivity;

                m_cameraRot *= Quaternion::CreateFromAxisAngle(im.Right(), orbit.y * handed);
                m_cameraRot *= Quaternion::CreateFromAxisAngle(im.Up(), -orbit.x * handed);
                m_cameraRot *= Quaternion::CreateFromAxisAngle(im.Forward(), orbit.z);
                m_cameraRot.Normalize();
            }

            m_cameraFocus += move * m_distance * elapsedTime * m_sensitivity;

            // Zoom camera
            m_zoom += (gpad.triggers.left - gpad.triggers.right) * elapsedTime * m_sensitivity;
            m_zoom = std::max(m_zoom, 0.1f);

            // Other controls
            if (m_gamepadButtonTracker.leftShoulder == GamePad::ButtonStateTracker::PRESSED
                && m_gamepadButtonTracker.rightShoulder == GamePad::ButtonStateTracker::PRESSED)
            {
                m_sensitivity = 1.f;
            }
            else
            {
                if (gpad.IsRightShoulderPressed())
                {
                    m_sensitivity += 0.01f;
                    if (m_sensitivity > 10.f)
                        m_sensitivity = 10.f;
                }
                else if (gpad.IsLeftShoulderPressed())
                {
                    m_sensitivity -= 0.01f;
                    if (m_sensitivity < 0.01f)
                        m_sensitivity = 0.01f;
                }
            }

            if (gpad.IsViewPressed())
            {
#ifdef _XBOX_ONE
                EnumerateModelFiles();
#else
                PostMessage(m_window, WM_USER, 0, 0);
#endif
            }

            if (m_gamepadButtonTracker.b == GamePad::ButtonStateTracker::PRESSED)
            {
                m_wireframe = !m_wireframe;
            }

            if (m_gamepadButtonTracker.x == GamePad::ButtonStateTracker::PRESSED)
            {
                int value = ((int)m_showGrid << 2) | ((int)m_showHud << 1) | ((int)m_showCross);

                value = (value + 1) & 0x7;

                m_showCross = (value & 0x1) != 0;
                m_showHud   = (value & 0x2) != 0;
                m_showGrid  = (value & 0x4) != 0;
            }

            if (m_gamepadButtonTracker.y == GamePad::ButtonStateTracker::PRESSED)
            {
                CycleBackgroundColor();
            }

            if ( m_gamepadButtonTracker.a == GamePad::ButtonStateTracker::PRESSED)
            {
                m_fpscamera = !m_fpscamera;
                m_viewRot = Quaternion::Identity;
            }

            if (m_gamepadButtonTracker.leftStick == GamePad::ButtonStateTracker::PRESSED)
            {
                CameraHome();
                m_modelRot = Quaternion::Identity;
            }

            if (!m_fpscamera && m_gamepadButtonTracker.rightStick == GamePad::ButtonStateTracker::PRESSED)
            {
                m_viewRot = m_cameraRot = m_modelRot = Quaternion::Identity;
            }
        }
    }
#ifndef _XBOX_ONE
    else
    {
        m_usingGamepad = false;

        m_gamepadButtonTracker.Reset();

        auto kb = m_keyboard->GetState();

        // Camera movement
        Vector3 move = Vector3::Zero;

        float scale = m_distance;
        if (kb.LeftShift || kb.RightShift)
            scale *= 0.5f;

        if (kb.Up)
            move.y += scale;

        if (kb.Down)
            move.y -= scale;

        if (kb.Right || kb.D)
            move.x += scale;

        if (kb.Left || kb.A)
            move.x -= scale;

        if (kb.PageUp || kb.W)
            move.z += scale * handed;

        if (kb.PageDown || kb.S)
            move.z -= scale * handed;

        if (kb.Space)
        {
            m_viewRot = Quaternion::CreateFromAxisAngle(Vector3::Right, XM_PI / 2.f * handed);
        }
        else if (kb.X)
        {
            m_viewRot = Quaternion::CreateFromAxisAngle(Vector3::Right, -XM_PI / 2.f * handed);
        }
        else if (kb.Q)
        {
            m_viewRot = Quaternion::CreateFromAxisAngle(Vector3::Up, XM_PI / 2.f * handed);
        }
        else if (kb.E)
        {
            m_viewRot = Quaternion::CreateFromAxisAngle(Vector3::Up, -XM_PI / 2.f * handed);
        }
        else
        {
            m_viewRot = Quaternion::Identity;
        }

        Matrix im;
        m_view.Invert(im);
        move = Vector3::TransformNormal(move, im);

        m_cameraFocus += move * elapsedTime;

        // Other keyboard controls
        if (kb.Home)
        {
            CameraHome();
        }

        if (kb.End)
        {
            m_viewRot = m_modelRot = Quaternion::Identity;
        }

        if (m_showGrid)
        {
            if (kb.OemPlus)
            {
                m_gridScale += 2.f * elapsedTime;
            }
            else if (kb.OemMinus)
            {
                m_gridScale -= 2.f * elapsedTime;
                if (m_gridScale < 1.f)
                    m_gridScale = 1.f;
            }
        }

        m_keyboardTracker.Update(kb);

        if (m_keyboardTracker.pressed.J)
            m_showCross = !m_showCross;

        if (m_keyboardTracker.pressed.G)
            m_showGrid = !m_showGrid;

        if (m_keyboardTracker.pressed.R)
            m_wireframe = !m_wireframe;

        if (m_keyboardTracker.pressed.H)
            m_showHud = !m_showHud;

        if (m_keyboardTracker.pressed.C)
            CycleBackgroundColor();

        if (m_keyboardTracker.pressed.O)
        {
            PostMessage(m_window, WM_USER, 0, 0);
        }

        // Mouse controls
        auto mouse = m_mouse->GetState();

        m_mouseButtonTracker.Update(mouse);

        if (mouse.positionMode == Mouse::MODE_RELATIVE)
        {
            // Translate camera
            Vector3 delta;
            if (kb.LeftShift || kb.RightShift)
            {
                delta = Vector3(0.f, 0.f, -float(mouse.y) * handed) * m_distance * elapsedTime;
            }
            else
            {
                delta = Vector3(-float(mouse.x), float(mouse.y), 0.f) * m_distance * elapsedTime;
            }

            delta = Vector3::TransformNormal(delta, im);

            m_cameraFocus += delta * elapsedTime;
        }
        else if (m_ballModel.IsDragging())
        {
            // Rotate model
            m_ballModel.OnMove(mouse.x, mouse.y);
            m_modelRot = m_ballModel.GetQuat();
        }
        else if (m_ballCamera.IsDragging())
        {
            // Rotate camera
            m_ballCamera.OnMove(mouse.x, mouse.y);
            Quaternion q = m_ballCamera.GetQuat();
            q.Inverse(m_cameraRot);
        }
        else
        {
            // Zoom with scroll wheel
            m_zoom = 1.f + mouse.scrollWheelValue / float(120 * 10);
            m_zoom = std::max(m_zoom, 0.1f);
        }

        if (!m_ballModel.IsDragging() && !m_ballCamera.IsDragging())
        {
            if (m_mouseButtonTracker.rightButton == Mouse::ButtonStateTracker::PRESSED)
                m_mouse->SetMode(Mouse::MODE_RELATIVE);
            else if (m_mouseButtonTracker.rightButton == Mouse::ButtonStateTracker::RELEASED)
                m_mouse->SetMode(Mouse::MODE_ABSOLUTE);

            if (m_mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
            {
                if (kb.LeftShift || kb.RightShift)
                {
                    m_ballModel.OnBegin(mouse.x, mouse.y);
                }
                else
                {
                    m_ballCamera.OnBegin(mouse.x, mouse.y);
                }
            }
        }
        else if (m_mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
        {
            m_ballCamera.OnEnd();
            m_ballModel.OnEnd();
        }
    }
#endif

    // Update camera
    Vector3 position = m_cameraFocus;

    Vector3 dir = Vector3::Transform((m_lhcoords) ? Vector3::Forward : Vector3::Backward, m_cameraRot);
    Vector3 up = Vector3::Transform(Vector3::Up, m_cameraRot);

    position += (m_distance * m_zoom) * dir;
        
    Matrix view;
    if (m_lhcoords)
    {
        view = XMMatrixLookAtLH(position, m_cameraFocus, up);
    }
    else
    {
        view = Matrix::CreateLookAt(position, m_cameraFocus, up);
    }

    m_view = view * Matrix::CreateFromQuaternion(m_viewRot);

    m_world = Matrix::CreateFromQuaternion(m_modelRot);
}

// Draws the scene
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    Clear();

    if (m_showGrid)
    {
        DrawGrid();
    }

    if (m_showCross)
    {
        DrawCross();
    }

    if (!m_model)
    {
        m_spriteBatch->Begin();

        if (*m_szError)
        {
            m_fontComic->DrawString(m_spriteBatch.get(), m_szError, XMFLOAT2(100, 100), Colors::Red);
        }
        else
        {
            m_fontComic->DrawString(m_spriteBatch.get(), L"No model is loaded\n", XMFLOAT2(100, 100), Colors::Red);
        }

        m_spriteBatch->End();
    }
    else
    {
        m_model->Draw( m_d3dContext.Get(), *m_states, m_world, m_view, m_proj, m_wireframe );

        if (*m_szStatus && m_showHud)
        {
            m_spriteBatch->Begin();

            Viewport vp(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));

            Vector3 cameraPos = vp.Unproject(Vector3(static_cast<float>(m_outputWidth) / 2.f, static_cast<float>(m_outputHeight) / 2.f, 0),
                                             m_proj, m_view, Matrix::Identity);

            Vector3 up = Vector3::TransformNormal(Vector3::Up, m_view);

            WCHAR szCamera[128];
            swprintf_s(szCamera, L"     Camera: (%8.4f,%8.4f,%8.4f) Look At: (%8.4f,%8.4f,%8.4f) Up: (%8.4f,%8.4f,%8.4f)",
                                 cameraPos.x, cameraPos.y, cameraPos.z,
                                 m_cameraFocus.x, m_cameraFocus.y, m_cameraFocus.z,
                                 up.x, up.y, up.z);

            WCHAR szMode[64];
            swprintf_s(szMode, L" %ls (Sensitivity: %8.4f)", (m_fpscamera) ? L"  FPS" : L"Orbit", m_sensitivity);

#ifdef _XBOX_ONE
            RECT rct = vp.ComputeTitleSafeArea(m_outputWidth, m_outputHeight);

            m_fontConsolas->DrawString(m_spriteBatch.get(), m_szStatus, XMFLOAT2(float(rct.left), float(rct.top)), m_uiColor);
            m_fontConsolas->DrawString(m_spriteBatch.get(), szCamera, XMFLOAT2(float(rct.left), float(rct.top + 20)), m_uiColor);
            if (m_usingGamepad)
            {
                m_fontConsolas->DrawString(m_spriteBatch.get(), szMode, XMFLOAT2(float(rct.right - 100), float(rct.bottom - 20.f)), m_uiColor);
            }
#else
            m_fontConsolas->DrawString(m_spriteBatch.get(), m_szStatus, XMFLOAT2(10, 10), m_uiColor);
            m_fontConsolas->DrawString(m_spriteBatch.get(), szCamera, XMFLOAT2(10, 30), m_uiColor);
            if (m_usingGamepad)
            {
                m_fontConsolas->DrawString(m_spriteBatch.get(), szMode, XMFLOAT2(m_outputWidth - 300.f, m_outputHeight - 20.f), m_uiColor);
            }
#endif

            m_spriteBatch->End();
        }
    }

    if (!m_fileNames.empty())
    {
        m_spriteBatch->Begin();

        float spacing = m_fontComic->GetLineSpacing();

        if (m_selectFile < m_firstFile)
            m_firstFile = m_selectFile;

        if (m_selectFile > (m_firstFile + 18))
            m_firstFile = m_selectFile;

        float y = 200.f;
        for (int j = m_firstFile; j < int(m_fileNames.size()); ++j)
        {
            m_fontComic->DrawString(m_spriteBatch.get(), m_fileNames[j].c_str(), XMFLOAT2(50, y), (m_selectFile == j) ? Colors::Yellow : Colors::Gray );
            y += spacing;
        }

        m_spriteBatch->End();
    }

    Present();
}

// Helper method to clear the backbuffers
void Game::Clear()
{
    // Clear the views
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), m_clearColor);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewPort);
}

// Presents the backbuffer contents to the screen
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    m_keyboardTracker.Reset();
    m_mouseButtonTracker.Reset();
    m_gamepadButtonTracker.Reset();
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef _XBOX_ONE
    m_d3dContext->Suspend(0);
#endif
}

void Game::OnResuming()
{
#ifdef _XBOX_ONE
    m_d3dContext->Resume();
#endif  
    m_timer.ResetElapsedTime();
    m_keyboardTracker.Reset();
    m_mouseButtonTracker.Reset();
    m_gamepadButtonTracker.Reset();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    m_ballCamera.SetWindow(m_outputWidth, m_outputHeight);
    m_ballModel.SetWindow(m_outputWidth, m_outputHeight);

    CreateResources();
}

void Game::OnFileOpen(const WCHAR* filename)
{
    if (!filename)
        return;

    wcscpy_s(m_szModelName, filename);
    m_reloadModel = true;
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1280;
    height = 720;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    // Create the DX11 API device object, and get a corresponding context.
#ifdef _XBOX_ONE
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,                                // leave as nullptr unless software device
        creationFlags,                          // optionally set debug and Direct2D compatibility flags
        featureLevels,                          // list of feature levels this app can support
        _countof(featureLevels),                // number of entries in above list
        D3D11_SDK_VERSION,                      // always set this to D3D11_SDK_VERSION
        device.ReleaseAndGetAddressOf(),        // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        context.ReleaseAndGetAddressOf()        // returns the device immediate context
        );
#else
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,                                // leave as nullptr unless software device
        creationFlags,                          // optionally set debug and Direct2D compatibility flags
        featureLevels,                          // list of feature levels this app can support
        _countof(featureLevels),                // number of entries in above list
        D3D11_SDK_VERSION,                      // always set this to D3D11_SDK_VERSION
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                creationFlags, &featureLevels[1], _countof(featureLevels) - 1,
                                D3D11_SDK_VERSION, m_d3dDevice.ReleaseAndGetAddressOf(),
                                &m_featureLevel, m_d3dContext.ReleaseAndGetAddressOf());
    }
#endif

    DX::ThrowIfFailed(hr);

#if !defined(_XBOX_ONE) && !defined(NDEBUG)
    ComPtr<ID3D11Debug> d3dDebug;
    hr = m_d3dDevice.As(&d3dDebug);
    if (SUCCEEDED(hr))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        hr = d3dDebug.As(&d3dInfoQueue);
        if (SUCCEEDED(hr))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
            };
            D3D11_INFO_QUEUE_FILTER filter;
            memset(&filter, 0, sizeof(filter));
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

#ifdef _XBOX_ONE
    // Get the DirectX11.X device by QI off the DirectX11 one.
    hr = device.As(&m_d3dDevice);
    DX::ThrowIfFailed(hr);

    // And get the corresponding device context in the same way.
    hr = context.As(&m_d3dContext);
    DX::ThrowIfFailed(hr);
#else
    // DirectX 11.1 if present
    hr = m_d3dDevice.As(&m_d3dDevice1);
    if (SUCCEEDED(hr))
        (void)m_d3dContext.As(&m_d3dContext1);
#endif

    auto ctx = m_d3dContext.Get();

    m_spriteBatch.reset(new SpriteBatch(ctx));

    m_fontConsolas.reset(new SpriteFont(m_d3dDevice.Get(), L"consolas.spritefont"));
    m_fontComic.reset(new SpriteFont(m_d3dDevice.Get(), L"comic.spritefont"));

    m_states.reset(new CommonStates(m_d3dDevice.Get()));

    m_lineEffect.reset(new BasicEffect(m_d3dDevice.Get()));
    m_lineEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_lineEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength, m_lineLayout.ReleaseAndGetAddressOf()));
    }

    m_lineBatch.reset(new PrimitiveBatch<VertexPositionColor>(ctx));

    m_world = Matrix::Identity;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
#ifdef _XBOX_ONE
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
#else
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
#endif

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(2, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

#ifdef _XBOX_ONE
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
        swapChainDesc.Flags = DXGIX_SWAP_CHAIN_FLAG_QUANTIZATION_RGB_FULL;

        // Create a SwapChain from a CoreWindow.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice.Get(),
            m_window, &swapChainDesc,
            nullptr, m_swapChain.GetAddressOf()));
#else
        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent( IID_PPV_ARGS( &dxgiFactory ) ) );

        ComPtr<IDXGIFactory2> dxgiFactory2;
        HRESULT hr = dxgiFactory.As(&dxgiFactory2);
        if (SUCCEEDED(hr))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(), m_window, &swapChainDesc,
                &fsSwapChainDesc,
                nullptr, m_swapChain1.ReleaseAndGetAddressOf()));

            m_swapChain1.As(&m_swapChain);
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = 2;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support 'full-screen' mode and prevents the ALT+ENTER shortcut from working
        dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER);
#endif
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    if (m_lhcoords)
    {
        m_proj = XMMatrixPerspectiveFovLH( m_fov, float(backBufferWidth) / float(backBufferHeight), 0.1f, m_farPlane);
    }
    else
    {
        m_proj = Matrix::CreatePerspectiveFieldOfView(m_fov, float(backBufferWidth) / float(backBufferHeight), 0.1f, m_farPlane);
    }

    m_lineEffect->SetProjection(m_proj);
}

void Game::OnDeviceLost()
{
    m_spriteBatch.reset();
    m_fontConsolas.reset();
    m_fontComic.reset();
    m_model.reset();
    m_states.reset();
    m_lineEffect.reset();
    m_lineBatch.reset();

    m_lineLayout.Reset();

    m_depthStencil.Reset();
    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

#ifndef _XBOX_ONE
    m_swapChain1.Reset();
    m_d3dContext1.Reset();
    m_d3dDevice1.Reset();
#endif

    CreateDevice();

    CreateResources();
}

void Game::LoadModel()
{
    m_model.reset();
    *m_szStatus = 0;
    *m_szError = 0;
    m_reloadModel = false;
    m_modelRot = Quaternion::Identity;

    if (!*m_szModelName)
        return;

    WCHAR drive[ _MAX_DRIVE ];
    WCHAR path[ MAX_PATH ];
    WCHAR ext[ _MAX_EXT ];
    WCHAR fname[ _MAX_FNAME ];
    _wsplitpath_s( m_szModelName, drive, _MAX_DRIVE, path, MAX_PATH, fname, _MAX_FNAME, ext, _MAX_EXT );
               
    EffectFactory fx(m_d3dDevice.Get());

    if (*drive || *path)
    {
        WCHAR dir[MAX_PATH] = { 0 };
        _wmakepath_s(dir, drive, path, nullptr, nullptr);
        fx.SetDirectory(dir);
    }

    try
    {
        if (_wcsicmp(ext, L".sdkmesh") == 0)
        {
            m_model = Model::CreateFromSDKMESH(m_d3dDevice.Get(), m_szModelName, fx, m_lhcoords);
        }
        else if (_wcsicmp(ext, L".cmo") == 0)
        {
            m_model = Model::CreateFromCMO(m_d3dDevice.Get(), m_szModelName, fx, !m_lhcoords);
        }
        else if (_wcsicmp(ext, L".vbo") == 0)
        {
            m_model = Model::CreateFromVBO(m_d3dDevice.Get(), m_szModelName, nullptr, m_lhcoords);
        }
        else
        {
            swprintf_s(m_szError, L"Unknown file type %ls", ext);
            m_model.reset();
            *m_szStatus = 0;
        }
    }
    catch(...)
    {
        swprintf_s(m_szError, L"Error loading model %ls%ls\n", fname, ext);
        m_model.reset();
        *m_szStatus = 0;
    }

    if (m_model)
    {
        size_t nmeshes = 0;
        size_t nverts = 0;
        size_t nfaces = 0;
        size_t nsubsets = 0;

        std::set<ID3D11Buffer*> vbs;
        for (auto it = m_model->meshes.cbegin(); it != m_model->meshes.cend(); ++it)
        {
            for (auto mit = (*it)->meshParts.cbegin(); mit != (*it)->meshParts.cend(); ++mit)
            {
                ++nsubsets;

                nfaces += ((*mit)->indexCount / 3);

                ID3D11Buffer* vbptr = (*mit)->vertexBuffer.Get();
                size_t vertexStride = (*mit)->vertexStride;

                if (vbptr && (vertexStride > 0) && vbs.find(vbptr) == vbs.end())
                {
                    D3D11_BUFFER_DESC desc;
                    vbptr->GetDesc(&desc);

                    nverts += (desc.ByteWidth / vertexStride);

                    vbs.insert(vbptr);
                }
            }
            ++nmeshes;
        }

        if (nmeshes > 1)
        {
            swprintf_s(m_szStatus, L"     Meshes: %6Iu   Verts: %6Iu   Faces: %6Iu   Subsets: %6Iu", nmeshes, nverts, nfaces, nsubsets);
        }
        else
        {
            swprintf_s(m_szStatus, L"     Verts: %6Iu   Faces: %6Iu   Subsets: %6Iu", nverts, nfaces, nsubsets);
        }
    }

    CameraHome();
}

void Game::DrawGrid()
{
    auto ctx = m_d3dContext.Get();
    ctx->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
    ctx->OMSetDepthStencilState( m_states->DepthDefault(), 0 );
    ctx->RSSetState( m_states->CullCounterClockwise() );

    m_lineEffect->SetView(m_view);

    m_lineEffect->Apply(ctx);

    m_d3dContext->IASetInputLayout(m_lineLayout.Get());

    m_lineBatch->Begin();

    Vector3 xaxis(m_gridScale, 0.f, 0.f);
    Vector3 yaxis(0.f, 0.f, m_gridScale);
    Vector3 origin = Vector3::Zero;

    XMVECTOR color = m_uiColor;

    for( size_t i = 0; i <= m_gridDivs; ++i )
    {
        float fPercent = float(i) / float(m_gridDivs);
        fPercent = ( fPercent * 2.0f ) - 1.0f;

        Vector3 scale = xaxis * fPercent + origin;

        VertexPositionColor v1( scale - yaxis, color );
        VertexPositionColor v2( scale + yaxis, color );
        m_lineBatch->DrawLine( v1, v2 );
    }

    for( size_t i = 0; i <= m_gridDivs; i++ )
    {
        float fPercent = float(i) / float(m_gridDivs);
        fPercent = ( fPercent * 2.0f ) - 1.0f;

        Vector3 scale = yaxis * fPercent + origin;

        VertexPositionColor v1( scale - xaxis, color );
        VertexPositionColor v2( scale + xaxis, color );
        m_lineBatch->DrawLine( v1, v2 );
    }

    m_lineBatch->End();
}

void Game::DrawCross()
{
    auto ctx = m_d3dContext.Get();
    ctx->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    ctx->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    ctx->RSSetState(m_states->CullCounterClockwise());

    m_lineEffect->SetView(m_view);

    m_lineEffect->Apply(ctx);

    m_d3dContext->IASetInputLayout(m_lineLayout.Get());

    XMVECTOR color = m_uiColor;

    m_lineBatch->Begin();

    float cross = m_distance / 100.f;

    Vector3 xaxis(cross, 0.f, 0.f);
    Vector3 yaxis(0.f, cross, 0.f);
    Vector3 zaxis(0.f, 0.f, cross);

    VertexPositionColor v1(m_cameraFocus - xaxis, color);
    VertexPositionColor v2(m_cameraFocus + xaxis, color);

    m_lineBatch->DrawLine(v1, v2);

    VertexPositionColor v3(m_cameraFocus - yaxis, color);
    VertexPositionColor v4(m_cameraFocus + yaxis, color);

    m_lineBatch->DrawLine(v3, v4);

    VertexPositionColor v5(m_cameraFocus - zaxis, color);
    VertexPositionColor v6(m_cameraFocus + zaxis, color);

    m_lineBatch->DrawLine(v5, v6);

    m_lineBatch->End();
}

void Game::CameraHome()
{
    m_mouse->ResetScrollWheelValue();
    m_zoom = 1.f;
    m_viewRot = m_cameraRot = Quaternion::Identity;
    m_ballCamera.Reset();

    if (!m_model)
    {
        m_cameraFocus = Vector3::Zero;
        m_distance = 10.f;
        m_gridScale = 1.f;
    }
    else
    {
        BoundingSphere sphere;
        BoundingBox box;
        for( auto it = m_model->meshes.cbegin(); it != m_model->meshes.cend(); ++it )
        {
	        if ( it == m_model->meshes.cbegin() )
	        {
		        sphere = (*it)->boundingSphere;
		        box = (*it)->boundingBox;
	        }
	        else
	        {
		        BoundingSphere::CreateMerged( sphere, sphere, (*it)->boundingSphere );
		        BoundingBox::CreateMerged( box, box, (*it)->boundingBox );
	        }
        }

        if ( sphere.Radius < 1.f )
        {
	        sphere.Center = box.Center;
	        sphere.Radius = std::max( box.Extents.x, std::max( box.Extents.y, box.Extents.z ) );
        }

        if ( sphere.Radius < 1.f )
        {
	        sphere.Center = XMFLOAT3(0.f,0.f,0.f);
	        sphere.Radius = 10.f;
        }

        m_gridScale = sphere.Radius;

        m_distance = sphere.Radius * 2;

        m_cameraFocus = sphere.Center;
    }
}

void Game::CycleBackgroundColor()
{
    if (m_clearColor == Vector4(Colors::CornflowerBlue.v))
    {
        m_clearColor = Colors::Black.v;
        m_uiColor = Colors::Yellow.v;
    }
    else if (m_clearColor == Vector4(Colors::Black.v))
    {
        m_clearColor = Colors::White.v;
        m_uiColor = Colors::Black.v;
    }
    else
    {
        m_clearColor = Colors::CornflowerBlue.v;
        m_uiColor = Colors::White.v;
    }
}

#ifdef _XBOX_ONE
void Game::EnumerateModelFiles()
{
    m_selectFile = m_firstFile = 0;
    m_fileNames.clear();

    WIN32_FIND_DATA ffdata = { 0 };

    static const WCHAR* exts[] = { L"D:\\*.sdkmesh", L"D:\\*.cmo", L"D:\\*.vbo" };
    
    for (size_t j = 0; j < _countof(exts); ++j)
    {
        HANDLE hFind = FindFirstFileEx( exts[j], FindExInfoStandard, &ffdata, FindExSearchNameMatch, nullptr, 0);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            continue;
        }

        if ( !(ffdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
        {
            OutputDebugStringW(ffdata.cFileName);
            std::wstring fname = ffdata.cFileName;

            m_fileNames.emplace_back(fname);
        }

        while (FindNextFile(hFind, &ffdata))
        {
            if (!(ffdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                OutputDebugStringW(ffdata.cFileName);
                std::wstring fname = ffdata.cFileName;

                m_fileNames.emplace_back(fname);
            }
        }

        FindClose(hFind);
    }
}
#endif