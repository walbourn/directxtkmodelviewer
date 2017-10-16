//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include "ArcBall.h"
#include "RenderTexture.h"

#if defined(_XBOX_ONE) && defined(_TITLE)
#include "DeviceResourcesXDK.h"
#else
#include "DeviceResourcesPC.h"
#endif


// A basic game implementation that creates a D3D11 device and
// provides a game loop
class Game
#if !defined(_XBOX_ONE) || !defined(_TITLE)
    : public DX::IDeviceNotify
#endif
{
public:

    Game();

    // Initialization and management
#if defined(_XBOX_ONE) && defined(_TITLE)
    void Initialize(IUnknown* window);
#else
    void Initialize(HWND window, int width, int height);
#endif

    // Basic game loop
    void Tick();

#if !defined(_XBOX_ONE) || !defined(_TITLE)
    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;
#endif

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
    void OnFileOpen(const wchar_t* filename);

    // Properites
    void GetDefaultSize( int& width, int& height ) const;
    bool RequestHDRMode() const { return m_deviceResources ? (m_deviceResources->GetDeviceOptions() & DX::DeviceResources::c_EnableHDR) != 0 : false; }

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    
    void LoadModel();
    void DrawGrid();
    void DrawCross();

    void CameraHome();

    void CycleBackgroundColor();
    void CycleToneMapOperator();

    void CreateProjection();

    void RotateView( DirectX::SimpleMath::Quaternion& q );

#if defined(_XBOX_ONE) && defined(_TITLE)
    void EnumerateModelFiles();
#endif

    // Device resources.
    std::unique_ptr<DX::DeviceResources>            m_deviceResources;
    std::unique_ptr<DX::RenderTexture>              m_hdrScene;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

#if defined(_XBOX_ONE) && defined(_TITLE)
    std::unique_ptr<DirectX::GraphicsMemory>        m_graphicsMemory;
#endif

    std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch;
    std::unique_ptr<DirectX::SpriteFont>            m_fontConsolas;
    std::unique_ptr<DirectX::SpriteFont>            m_fontComic;
    std::unique_ptr<DirectX::Model>                 m_model;
    std::unique_ptr<DirectX::CommonStates>          m_states;
    std::unique_ptr<DirectX::BasicEffect>           m_lineEffect;
    std::unique_ptr<DirectX::ToneMapPostProcess>    m_toneMap;

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
    DirectX::SimpleMath::Vector3                    m_lastCameraPos;
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
    bool                                            m_ccw;
    bool                                            m_reloadModel;
    bool                                            m_lhcoords;
    bool                                            m_fpscamera;

    int                                             m_toneMapMode;

    wchar_t                                         m_szModelName[MAX_PATH];
    wchar_t                                         m_szStatus[ 512 ];
    wchar_t                                         m_szError[ 512 ];

    ArcBall                                         m_ballCamera;
    ArcBall                                         m_ballModel;

    int                                             m_selectFile;
    int                                             m_firstFile;
    std::vector<std::wstring>                       m_fileNames;
};