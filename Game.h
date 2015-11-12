//
// Game.h
//

#pragma once

#include <vector>
#include <string>

#include "StepTimer.h"
#include "ArcBall.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop
class Game
{
public:

    Game();

    // Initialization and management
#ifdef _XBOX_ONE
    void Initialize(IUnknown* window);
#else
    void Initialize(HWND window, int width, int height);
#endif

    // Basic game loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();
    void Present();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);
    void OnFileOpen(const WCHAR* filename);

    // Properites
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);

    void CreateDevice();
    void CreateResources();
    
    void OnDeviceLost();

    void LoadModel();
    void DrawGrid();
    void DrawCross();

    void CameraHome();

    void CycleBackgroundColor();

#ifdef _XBOX_ONE
    void EnumerateModelFiles();
#endif

    // Application state
#ifdef _XBOX_ONE
    IUnknown*                                       m_window;
#else
    HWND                                            m_window;
#endif
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    // Direct3D Objects
    D3D_FEATURE_LEVEL                               m_featureLevel;
#ifdef _XBOX_ONE
    Microsoft::WRL::ComPtr<ID3D11DeviceX>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContextX>    m_d3dContext;
#else
    Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;
#endif

    // Rendering resources
#ifdef _XBOX_ONE
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
#else
    Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
#endif
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencil;

    // Game state
    DX::StepTimer                                   m_timer;

    std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch;
    std::unique_ptr<DirectX::SpriteFont>            m_fontConsolas;
    std::unique_ptr<DirectX::SpriteFont>            m_fontComic;
    std::unique_ptr<DirectX::Model>                 m_model;
    std::unique_ptr<DirectX::CommonStates>          m_states;
    std::unique_ptr<DirectX::BasicEffect>           m_lineEffect;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_lineLayout;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_lineBatch;

    std::unique_ptr<DirectX::GamePad>               m_gamepad;
    std::unique_ptr<DirectX::Keyboard>              m_keyboard;
    std::unique_ptr<DirectX::Mouse>                 m_mouse;

    DirectX::Keyboard::KeyboardStateTracker         m_keyboardTracker;
    DirectX::Mouse::ButtonStateTracker              m_mouseButtonTracker;
    DirectX::GamePad::ButtonStateTracker            m_gamepadButtonTracker;

    DirectX::SimpleMath::Matrix                     m_world;
    DirectX::SimpleMath::Matrix                     m_view;
    DirectX::SimpleMath::Matrix                     m_proj;

    DirectX::SimpleMath::Vector3                    m_cameraFocus;
    DirectX::SimpleMath::Quaternion                 m_cameraRot;
    DirectX::SimpleMath::Quaternion                 m_viewRot;
    DirectX::SimpleMath::Color                      m_clearColor;
    DirectX::SimpleMath::Color                      m_uiColor;

    DirectX::SimpleMath::Quaternion                 m_modelRot;

    float                                           m_gridScale;
    float                                           m_fov;
    float                                           m_zoom;
    float                                           m_distance;
    float                                           m_farPlane;
    float                                           m_sensitivity;
    size_t                                          m_gridDivs;

    bool                                            m_showHud;
    bool                                            m_showCross;
    bool                                            m_showGrid;
    bool                                            m_usingGamepad;
    bool                                            m_wireframe;
    bool                                            m_reloadModel;
    bool                                            m_lhcoords;
    bool                                            m_fpscamera;

    WCHAR                                           m_szModelName[MAX_PATH];
    WCHAR                                           m_szStatus[ 512 ];
    WCHAR                                           m_szError[ 512 ];

    ArcBall                                         m_ballCamera;
    ArcBall                                         m_ballModel;

    int                                             m_selectFile;
    int                                             m_firstFile;
    std::vector<std::wstring>                       m_fileNames;
};