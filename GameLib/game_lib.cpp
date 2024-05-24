//******************************************************************************
//
//      GameLib
//
//      ECC College of Computer & Multimedia
//
//******************************************************************************

//------< ���C�u���� >----------------------------------------------------------
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "winmm.lib" )

#ifdef _DEBUG
#pragma comment( lib, "./GameLib/Lib/Debug/DirectXTK.lib" )
#pragma comment( lib, "./GameLib/Lib/Debug/DirectXTKAudioWin8.lib" )
#else
#pragma comment( lib, "./GameLib/Lib/Release/DirectXTK.lib" )
#pragma comment( lib, "./GameLib/Lib/Release/DirectXTKAudioWin8.lib" )
#endif

//------< �C���N���[�h >--------------------------------------------------------
#include "game_lib.h"
#include <ctime>
#include <crtdbg.h>

#include "blender.h"
#include "misc.h"
#include "util.h"

#include "high_resolution_timer.h"
#include "frame_rate.h"

#include "debug.h"
#include "primitive.h"
#include "input_manager.h"

#include "view_settings.h"
#include "depth_stencil.h"

namespace GameLib
{

    // �����ϐ��p�\���́����̐錾
    struct Members
    {
        // �E�C���h�E�֘A
        HWND hwnd                                   = 0;  // �E�C���h�E�n���h��
        int  width                                  = 0;    // �E�C���h�E�̕�
        int  height                                 = 0;    // �E�C���h�E�̍���

        // DirectX11�֘A
        ID3D11Device*           device              = nullptr;
        ID3D11DeviceContext*    context             = nullptr;
        IDXGISwapChain*         swapChain           = nullptr;
        ID3D11RenderTargetView* renderTargetView    = nullptr;
        ID3D11DepthStencilView* depthStencilView    = nullptr;
        ID3D11BlendState*       blendState          = nullptr;

        // ���̑�
        Blender*                blender             = nullptr;
        Debug*                  debug               = nullptr;
        Primitive*              primitive           = nullptr;
        PrimitiveBatch*         primitiveBatch      = nullptr;
        HighResolutionTimer     hrTimer             = {};
        Blender::BLEND_STATE    bs                  = Blender::BS_NONE;

        TextureManager*         textureManager      = nullptr;
        InputManager*           inputManager        = nullptr;
        FontManager*            fontManager         = nullptr;
        audio::DXTKAudio*       dxtkAudio           = nullptr;
        ViewSettings*           viewSettings        = nullptr;
    };

    static Members m;

    void startHrTimer()
    {
        m.hrTimer.start();
    }

    void stopHrTimer()
    {
        m.hrTimer.stop();
    }

    //--------------------------------------------------------------------------

    //**************************************************************************
    //
    //
    //      GameLib�C���^�[�t�F�C�X
    //
    //
    //**************************************************************************

    //==========================================================================
    //
    //      ���C�u���������ݒ聕�I�������AWindows API�Ȃ�
    //
    //==========================================================================

    //--------------------------------------------------------------
    //  ���C�u���������ݒ�
    //--------------------------------------------------------------
    void init(LPCTSTR caption, int width, int height, bool isFullscreen, double frameRate)
    {
        // �����n��̐ݒ�
        srand((unsigned int)time(NULL));

#ifdef _DEBUG
        // �t���X�N���[���̐ݒ�i�O�̂��߃f�o�b�O���[�h�ł͕s�j
        isFullscreen = false;
#endif

        // �E�C���h�E�̏����ݒ�
        m.hwnd = window::init(caption, width, height);

        // DirectX11�̏�����
        DirectX11::init(m.hwnd, width, height, isFullscreen);

        // �t���[�����[�g�̐ݒ�
        m.hrTimer.setFrameRate(frameRate);

        // �r���[�̐ݒ�
        m.viewSettings = new ViewSettings;
        view::init();

        m.width = width;
        m.height = height;
    }

    //--------------------------------------------------------------
    //  ���C�u�����I������
    //--------------------------------------------------------------
    void uninit()
    {
        // DirectX11�̏I������
        DirectX11::uninit();

        // �E�C���h�E�̏I������
        window::uninit();
    }

    //--------------------------------------------------------------
    //  �Q�[�����[�v�i���Ԃ̓��b�Z�[�W���[�v�j
    //--------------------------------------------------------------
    bool gameLoop(bool isShowFrameRate)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) return false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //�t���[�����[�g�p
        while (!m.hrTimer.tick());
        if (isShowFrameRate)
        {
            calculateFrameStats(m.hwnd, &m.hrTimer);//�t���[�����[�g�v�Z�E�^�C�g���o�[�ɕ\��
        }

        return true;
    }

    //--------------------------------------------------------------
    //  ��ʃN���A
    //--------------------------------------------------------------
    void clear(const VECTOR4& color)
    {
        m.context->ClearRenderTargetView(m.renderTargetView, (const float *)&color);
        m.context->ClearDepthStencilView(m.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    }

    //--------------------------------------------------------------
    //  ��ʃN���A
    //--------------------------------------------------------------
    void clear(float r, float g, float b)
    {
        clear(VECTOR4(r, g, b, 1.0f));
    }

    //--------------------------------------------------------------
    //  ���̃o�b�N�o�b�t�@�̓��e��\��
    //--------------------------------------------------------------
    HRESULT present(UINT SyncInterval, UINT Flags)
    {
        return m.swapChain->Present(SyncInterval, Flags);
    }

    //--------------------------------------------------------------
    //  �u�����h���[�h�ݒ�
    //--------------------------------------------------------------
    void setBlendMode(Blender::BLEND_STATE blend)
    {
        m.context->OMSetBlendState(m.blender->states[blend], nullptr, 0xFFFFFFFF);
        m.bs = blend;
    }

    Blender::BLEND_STATE getBlendMode()
    {
        return m.bs;
    }

    //--------------------------------------------------------------
    //  �X�e���V�����N���A
    //--------------------------------------------------------------
    void clearStencil() {
        m.context->ClearDepthStencilView(m.depthStencilView, D3D11_CLEAR_STENCIL, 1, 0);
    }

    //--------------------------------------------------------------
    //  �X�v���C�g�ǂݍ���
    //--------------------------------------------------------------
    //void sprite_load(Sprite** ppSpr, const wchar_t* fileName, int nBufSize)
    //{
    //    *ppSpr = new Sprite(m.device, fileName, nBufSize);
    //}
    Sprite* sprite_load(const wchar_t* fileName, int nBufSize)
    {
        return new Sprite(m.device, fileName, nBufSize);
    }

    //--------------------------------------------------------------
    //  �X�v���C�g�`��
    //--------------------------------------------------------------
    void sprite_render(Sprite* pSpr,
        float x, float y, float sx, float sy,
        float tx, float ty, float tw, float th,
        float cx, float cy, float angle,
        float r, float g, float b, float a, 
        float z, 
        bool world)
    {
        if (pSpr)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                VECTOR2 s = VECTOR2(sx, sy) * scaleFactor;
                VECTOR2 pos = view::worldToScreen(VECTOR2(x, y));
                pSpr->render(m.context, pos.x, pos.y, s.x, s.y, tx, ty, tw, th, cx, cy, angle, r, g, b, a, z);
            }
            else
            {
                pSpr->render(m.context, x, y, sx, sy, tx, ty, tw, th, cx, cy, angle, r, g, b, a, z);
            }
        }
    }

    //--------------------------------------------------------------
    //  �l�p�|���S���`��i���_�w��j
    //--------------------------------------------------------------
    void sprite_quad(Sprite* pSpr,
        const VECTOR2(&v)[4],
        float tx, float ty, float tw, float th,
        float r, float g, float b, float a, 
        float z, 
        bool world)
    {
        if (pSpr)
        {
            if (world)
            {
                VECTOR2 q[4] = {};
                for (int i = 0; i < 4; i++) q[i] = v[i];
                for (VECTOR2* p = q; p < q + 4; p++) *p = view::worldToScreen(*p);
                pSpr->quad(m.context, q, VECTOR2(tx, ty), VECTOR2(tw, th), VECTOR4(r, g, b, a), z);
            }
            else
            {
                pSpr->quad(m.context, v, VECTOR2(tx, ty), VECTOR2(tw, th), VECTOR4(r, g, b, a), z);
            }
        }
    }

    //--------------------------------------------------------------
    //  �g���C�A���O���X�g���b�v
    //--------------------------------------------------------------
    void draw_triangle_strip(Sprite* pSpr,
        VECTOR3* pVertex, VECTOR4* pColor, VECTOR2* pUvs,
        int nVert, int nPolygon, float z, bool world)
    {
        if (pSpr)
        {
            if (world)
            {
                for (VECTOR3* p = pVertex; p < pVertex + nVert; p++) *p = view::worldToScreen(*p);
                pSpr->draw_triangle_strip(m.context, pVertex, pColor, pUvs, nVert, nPolygon, z);
            }
            else
            {
                pSpr->draw_triangle_strip(m.context, pVertex, pColor, pUvs, nVert, nPolygon, z);
            }
        }
    }

    //--------------------------------------------------------------
    //  �g���C�A���O���Y�i�C���f�b�N�X�h�j
    //--------------------------------------------------------------
    void draw_triangles_indexed(Sprite* pSpr,
        VECTOR3* pVertex, VECTOR4* pColor, VECTOR2* pUvs,
        short* pIndices, int nVertex, int nPolygon, float z, 
        bool world)
    {
        if (pSpr)
        {
            if (world)
            {
                for (VECTOR3* p = pVertex; p < pVertex + nVertex; p++) *p = view::worldToScreen(*p);
                pSpr->draw_triangles_indexed(m.context, pVertex, pColor, pUvs, pIndices, nPolygon * 3, nPolygon, z);
            }
            else
            {
                pSpr->draw_triangles_indexed(m.context, pVertex, pColor, pUvs, pIndices, nPolygon * 3, nPolygon, z);
            }
        }
    }

    //--------------------------------------------------------------
    //  �g���C�A���O���t�@���i�C���f�b�N�X�h�j
    //--------------------------------------------------------------
    void draw_triangle_fan_indexed(Sprite* pSpr,
        VECTOR3* pVertex, VECTOR4* pColor, VECTOR2* pUvs,
        short* pIndices, int nVertex, int nPolygon, float z, 
        bool world)
    {
        if (pSpr)
        {
            if (world)
            {
                for (VECTOR3* p = pVertex; p < pVertex + nVertex; p++) *p = view::worldToScreen(*p);
                pSpr->draw_triangle_fan_indexed(m.context, pVertex, pColor, pUvs, pIndices, nPolygon, z);
            }
            else
            {
                pSpr->draw_triangle_fan_indexed(m.context, pVertex, pColor, pUvs, pIndices, nPolygon, z);
            }
        }
    }

    //--------------------------------------------------------------
    //  �e�L�X�g�\��
    //--------------------------------------------------------------
    float text_out(int fontNo, std::string str,
        float x, float y, float scaleX, float scaleY,
        float r, float g, float b, float a, float z, 
        TEXT_ALIGN align)
    {
        return font::textOut(fontNo, str, x, y, scaleX, scaleY, r, g, b, a, align, z);
    }

    //==========================================================================
    //
    //      DirectX11
    //
    //==========================================================================

    //--------------------------------------------------------------
    //  DirectX11�̏�����
    //--------------------------------------------------------------
    HRESULT DirectX11::init(HWND hwnd, int width, int height, bool isFullscreen)
    {
        HRESULT hr = S_OK;

        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif//_DEBUG

        D3D_DRIVER_TYPE driverTypes[] = {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
            D3D_DRIVER_TYPE_UNKNOWN,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        //bool enable_4x_msaa = false;  // ���g�p

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
            D3D_FEATURE_LEVEL featureLevel;
            hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels,
                numFeatureLevels, D3D11_SDK_VERSION, &m.device, &featureLevel, &m.context);
            if (SUCCEEDED(hr)) break;
        }
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr)) return E_FAIL;

        DXGI_SWAP_CHAIN_DESC sd = { 0 };
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 0;
        sd.BufferDesc.RefreshRate.Denominator = 0;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.Windowed = !isFullscreen;
        sd.OutputWindow = hwnd;

#ifdef	MSAA
        UINT msaa_quality_level;
        device->CheckMultisampleQualityLevels(sd.BufferDesc.Format, 16, &msaa_quality_level);
        enable_4x_msaa = msaa_quality_level > 0 ? true : false;
        sd.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
        sd.SampleDesc.Quality = enable_4x_msaa ? msaa_quality_level - 1 : 0;
#else //MSAA
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
#endif//MSAA
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#if 1
        IDXGIDevice *dxgi;
        hr = m.device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi);
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;

        IDXGIAdapter1 *adapter;
        hr = dxgi->GetAdapter((IDXGIAdapter **)&adapter);
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;

        IDXGIFactory1 *factory;
        hr = adapter->GetParent(__uuidof(IDXGIFactory1), (void **)&factory);
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;

        hr = factory->CreateSwapChain(m.device, &sd, &m.swapChain);

        // �t���X�N���[������
        m.swapChain->SetFullscreenState(isFullscreen, nullptr);
        if (!isFullscreen)
        {
            //������Ԃ��t���X�N���[���łȂ���΁A���[�h�ؑւ��֎~����
            factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
        }

        factory->Release();
        adapter->Release();
        dxgi->Release();
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;
#else
        IDXGIFactory1 *factory;
        CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;
        hr = factory->CreateSwapChain(Device, &sd, &SwapChain);
        factory->Release();
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr))	return E_FAIL;
#endif

        //Create a render target view
        D3D11_TEXTURE2D_DESC back_buffer_desc;
        ID3D11Texture2D *pBackBuffer = nullptr;
        hr = m.swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            (LPVOID *)&pBackBuffer
            );
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr)) return E_FAIL;
        hr = m.device->CreateRenderTargetView(
            pBackBuffer,
            nullptr,
            &m.renderTargetView
            );
        pBackBuffer->GetDesc(&back_buffer_desc);
        pBackBuffer->Release();
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr)) return E_FAIL;

        //Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth = back_buffer_desc;
        ID3D11Texture2D *DepthStencil;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = m.device->CreateTexture2D(&descDepth, nullptr, &DepthStencil);
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

        //Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        SecureZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
#ifdef MSAA
        descDSV.ViewDimension = enable_4x_msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
#else//MSAA
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif//MSAA
        descDSV.Flags = 0;
        descDSV.Texture2D.MipSlice = 0;
        hr = m.device->CreateDepthStencilView(DepthStencil, &descDSV, &m.depthStencilView);
        DepthStencil->Release();
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
        if (FAILED(hr)) return E_FAIL;

        //Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;//��
        vp.Height = (FLOAT)height;//����
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m.context->RSSetViewports(1, &vp);
        m.context->OMSetRenderTargets(1, &m.renderTargetView, m.depthStencilView);

        m.blender = new Blender(m.device);
        m.debug = new Debug(m.device, L"./Data/fonts/font4.png", system::DEBUG_INSTANCE_NUM, height);
        m.primitive = new Primitive(m.device);
        m.primitiveBatch = new PrimitiveBatch(m.device, system::PRIMITIVE_BATCH_INSTANCE_NUM);

        // �ő喇��256�Ńe�N�X�`��������
        m.textureManager = new TextureManager;

        // InputManager�̏�����
        m.inputManager = new InputManager;

        // FontManager�̏�����
        m.fontManager = new FontManager(m.device);

        // DXTKAudio
        m.dxtkAudio = new audio::DXTKAudio;

        DepthStencil::instance().set(DepthStencil::MODE::NONE);

        return hr;
    }

    //--------------------------------------------------------------
    //  DirectX11�̏I������
    //--------------------------------------------------------------
    void DirectX11::uninit()
    {
        m.swapChain->SetFullscreenState(false, nullptr);

        safe_delete(m.viewSettings);
        safe_delete(m.dxtkAudio);
        safe_delete(m.fontManager);
        safe_delete(m.inputManager);
        safe_delete(m.textureManager);

        safe_delete(m.primitiveBatch);
        safe_delete(m.primitive);
        safe_delete(m.debug);
        safe_delete(m.blender);

        safe_release(m.depthStencilView);
        safe_release(m.renderTargetView);
        safe_release(m.swapChain);
        safe_release(m.context);
        safe_release(m.device);
    }

    //--------------------------------------------------------------
    //  �f�o�C�X�̎擾
    //--------------------------------------------------------------
    ID3D11Device* DirectX11::getDevice()
    {
        return m.device;
    }

    //--------------------------------------------------------------
    //  �R���e�L�X�g�̎擾
    //--------------------------------------------------------------
    ID3D11DeviceContext* DirectX11::getContext()
    {
        return m.context;
    }

    //==========================================================================
    //
    //      window
    //
    //==========================================================================

    //----------------------------------------------------------------------------------
    static const LPCWSTR CLASS_NAME = L"2dgp";

    //------< �v���g�^�C�v�錾 >-------------------------------------------------
    LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //--------------------------------------------------------------
    //  �E�C���h�E�̏���������
    //--------------------------------------------------------------
    HWND window::init(LPCTSTR caption, int width, int height)
    {
#if defined(DEBUG) | defined(_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = fnWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = GetModuleHandle(NULL);
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = CLASS_NAME;
        wcex.hIconSm = 0;
        RegisterClassEx(&wcex);

        RECT rc = { 0, 0, width, height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        HWND hwnd = CreateWindow(CLASS_NAME, caption, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, GetModuleHandle(NULL), NULL);
        ShowWindow(hwnd, SW_SHOWDEFAULT);

        return hwnd;
    }

    //--------------------------------------------------------------
    //  �E�C���h�E�̏I������
    //--------------------------------------------------------------
    void window::uninit()
    {
        // �I������
        UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
    }

    //--------------------------------------------------------------
    //  �E�C���h�E�n���h���̎擾
    //--------------------------------------------------------------
    HWND window::getHwnd()
    {
        return m.hwnd;
    }

    //--------------------------------------------------------------
    //  �E�C���h�E�̕����擾
    //--------------------------------------------------------------
    float window::getWidth()
    {
        return static_cast<float>(m.width);
    }
    int window::getWidthInt()
    {
        return m.width;
    }

    //--------------------------------------------------------------
    //  �E�C���h�E�̍������擾
    //--------------------------------------------------------------
    float window::getHeight()
    {
        return static_cast<float>(m.height);
    }
    int window::getHeightInt()
    {
        return m.height;
    }

    //==========================================================================
    //
    //      texture_manager
    //
    //==========================================================================

    namespace texture
    {

        //--------------------------------------------------------------
        //  �e�N�X�`���}�l�[�W��������
        //--------------------------------------------------------------
        void init(size_t max)
        {
            m.textureManager->init(max);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���}�l�[�W���I������
        //--------------------------------------------------------------
        void uninit()
        {
            m.textureManager->uninit();
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̃��[�h�i�ԍ����w�肵�ă��[�h�j
        //--------------------------------------------------------------
        void load(int texNo, const wchar_t* fileName, size_t maxInstance)
        {
            m.textureManager->load(m.device, texNo, fileName, maxInstance);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̃��[�h�i�󂫔ԍ���T���ă��[�h�j
        //--------------------------------------------------------------
        int load(const wchar_t* fileName, size_t maxInstance)
        {
            return m.textureManager->load(m.device, fileName, maxInstance);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̃��[�h�i���[�h�e�N�X�`���̃f�[�^���g���ă��[�h�j
        //--------------------------------------------------------------
        void load(const LoadTexture* data)
        {
            m.textureManager->load(m.device, data);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̉���i�ԍ����w�肵�ĉ���j
        //--------------------------------------------------------------
        void release(int texNo)
        {
            m.textureManager->release(texNo);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̉���i���[�h�e�N�X�`���̃f�[�^���g���ĉ���j
        //--------------------------------------------------------------
        void release(const LoadTexture* data)
        {
            m.textureManager->release(data);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̉���i�S�āj
        //--------------------------------------------------------------
        void releaseAll()
        {
            m.textureManager->releaseAll();
        }

        //--------------------------------------------------------------
        //  �`��̑O����
        //--------------------------------------------------------------
        void begin(int texNo)
        {
            m.textureManager->begin(m.context, texNo);
        }

        //--------------------------------------------------------------
        //  �`��̌㏈��
        //--------------------------------------------------------------
        void end(int texNo)
        {
            m.textureManager->end(m.context, texNo);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̕`��
        //--------------------------------------------------------------
        void draw(int texNo,
            const VECTOR2& position, const VECTOR2& scale,
            const VECTOR2& texPos, const VECTOR2& texSize,
            const VECTOR2& center, float angle,
            const VECTOR4& color, float z, 
            bool world)
        {
            m.textureManager->draw(texNo,
                position, scale,
                texPos, texSize, 
                center, angle,
                color, z, 
                world);
        }

        //--------------------------------------------------------------
        //  �e�N�X�`���̕`��
        //--------------------------------------------------------------
        void draw(int texNo,
            float x, float y, float scaleX, float scaleY,
            float left, float top, float width, float height,
            float centerX, float centerY, float angle, 
            float r, float g, float b, float a,
            float z, 
            bool world)
        {
            draw(texNo,
                VECTOR2(x, y), VECTOR2(scaleX, scaleY),
                VECTOR2(left, top), VECTOR2(width, height),
                VECTOR2(centerX, centerY), angle,
                VECTOR4(r, g, b, a), z, 
                world);
        }

    }

    //==========================================================================
    //
    //      primitive
    //
    //==========================================================================

    namespace primitive
    {

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(
            float x, float y, float w, float h,
            float cx, float cy, float angle,
            float r, float g, float b, float a, 
            float z, 
            bool world)
        {
            rect(VECTOR2(x, y), VECTOR2(w, h), VECTOR2(cx, cy), angle, VECTOR4(r, g, b, a), z, world);
        }

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(
            const VECTOR2& position, const VECTOR2& size,
            const VECTOR2& center, float angle, 
            const VECTOR4& color, float z, 
            bool world)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                VECTOR2 s = size * scaleFactor;
                VECTOR2 c = center * scaleFactor;
                VECTOR2 pos = view::worldToScreen(position);
                m.primitive->rect(m.context, pos, s, c, angle, color, z);
            }
            else
            {
                m.primitive->rect(m.context, position, size, center, angle, color, z);
            }
        }

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(
            const VECTOR2& from, const VECTOR2& to,
            const VECTOR4& color, float width, float z, 
            bool world)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                float w = (std::max)(width * scaleFactor, 1.0f);
                VECTOR2 v1 = view::worldToScreen(from);
                VECTOR2 v2 = view::worldToScreen(to);
                m.primitive->line(m.context, v1, v2, color, w, z);
            }
            else
            {
                m.primitive->line(m.context, from, to, color, width, z);
            }
        }

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(
            float x1, float y1, float x2, float y2,
            float r, float g, float b, float a,
            float width, float z, 
            bool world)
        {
            line(VECTOR2(x1, y1), VECTOR2(x2, y2), VECTOR4(r, g, b, a), width, z, world);
        }

        //--------------------------------------------------------------
        //  �~�`��
        //--------------------------------------------------------------
        void circle(
            const VECTOR2& pos, float radius, const VECTOR2& scale, float angle, 
            const VECTOR4& color, int n, float z, bool world)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                VECTOR2 v1 = view::worldToScreen(pos);
                m.primitive->circle(m.context, v1, radius * scaleFactor, scale, angle, color, n, z);
            }
            else
            {
                m.primitive->circle(m.context, pos, radius, scale, angle, color, n, z);
            }
        }

        //--------------------------------------------------------------
        //  �~�`��
        //--------------------------------------------------------------
        void circle(
            float x, float y, float radius, float sx, float sy, float angle, 
            float r, float g, float b, float a, int n, float z, 
            bool world)
        {
            circle(VECTOR2(x, y), radius, VECTOR2(sx, sy), angle, 
                VECTOR4(r, g, b, a), n, z, world);
        }

        //--------------------------------------------------------------
        //  �l�p�|���S���`��i���_�w��j�i���o�[�W�����j
        //--------------------------------------------------------------
        void quad(const VECTOR2 (&v)[4], const VECTOR4& color, float z, 
            bool world)
        {
            if (world)
            {
                VECTOR2 q[4] = {};
                for (int i = 0; i < 4; i++) q[i] = view::worldToScreen(v[i]);
                m.primitive->quad(m.context, q, color, z);
            }
            else
            {
                m.primitive->quad(m.context, v, color, z);
            }
        }

        //--------------------------------------------------------------
        //  �l�p�|���S���`��i���_�w��j�i���o�[�W�����j
        //--------------------------------------------------------------
        void quad(const VECTOR2(&v)[4], float r, float g, float b, float a, float z, 
            bool world)
        {
            quad(v, VECTOR4(r, g, b, a), z, world);
        }

        void draw_triangle_fan(const VECTOR2 vertex[], const VECTOR4& color, int nPolygon, float z, bool world)
        {
            std::vector<VECTOR2> v;
            const int vertexNum = nPolygon + 2;
            v.resize(vertexNum);
            for (int i = 0; i < vertexNum; i++)
            {
                if (world) v[i] = view::worldToScreen(vertex[i]);
                else v[i] = vertex[i];
            }
            m.primitive->draw_triangle_fan(m.context, v.data(), color, nPolygon, z);
        }

    }

    //==========================================================================
    //
    //      primitive_batch
    //
    //==========================================================================

    namespace primitive_batch
    {

        //--------------------------------------------------------------
        //	�O����
        //--------------------------------------------------------------
        void begin()
        {
            m.primitiveBatch->begin(m.context);
        }

        //--------------------------------------------------------------
        //  �㏈��
        //--------------------------------------------------------------
        void end()
        {
            m.primitiveBatch->end(m.context);
        }

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(
            const VECTOR2& pos, const VECTOR2& size,
            const VECTOR2& center, float angle,
            const VECTOR4& color, float z, 
            bool world)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                VECTOR2 s = size * scaleFactor;
                VECTOR2 v = view::worldToScreen(pos);
                m.primitiveBatch->rect(v, s, center, angle, color, z);
            }
            else
            {
                m.primitiveBatch->rect(pos, size, center, angle, color, z);
            }
        }

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(
            float x, float y, float w, float h,
            float cx, float cy, float angle,
            float r, float g, float b, float a, 
            float z, 
            bool world)
        {
            rect(
                VECTOR2(x, y), VECTOR2(w, h), 
                VECTOR2(cx, cy), angle, 
                VECTOR4(r, g, b, a), 
                z, 
                world);
        }

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(
            const VECTOR2& from, const VECTOR2& to,
            const VECTOR4& color, 
            float width, float z, 
            bool world)
        {
            if (world)
            {
                float scaleFactor = view::getScale();
                float w = (std::max)(width * scaleFactor, 1.0f);
                VECTOR2 v1 = view::worldToScreen(from);
                VECTOR2 v2 = view::worldToScreen(to);
                m.primitiveBatch->line(v1, v2, color, w, z);
            }
            else
            {
                m.primitiveBatch->line(from, to, color, width, z);
            }
        }

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(
            float x1, float y1, float x2, float y2,
            float r, float g, float b, float a,
            float width, float z, 
            bool world)
        {
            line(VECTOR2(x1, y1), VECTOR2(x2, y2), VECTOR4(r, g, b, a), width, z, world);
        }

    }

    //==========================================================================
    //
    //      music
    //
    //==========================================================================

    namespace music
    {

        //--------------------------------------------------------------
        //  ���y�̃��[�h
        //--------------------------------------------------------------
        void load(int trackNo, const wchar_t* waveFileName, float volume)
        {
            m.dxtkAudio->musicLoad(trackNo, waveFileName, volume);
        }

        //--------------------------------------------------------------
        //  ���y�̉��
        //--------------------------------------------------------------
        void unload(int trackNo)
        {
            m.dxtkAudio->musicUnload(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y�̍Đ�
        //--------------------------------------------------------------
        void play(int trackNo, bool isLoop)
        {
            m.dxtkAudio->musicPlay(trackNo, isLoop);
        }

        //--------------------------------------------------------------
        //  ���y�̒�~
        //--------------------------------------------------------------
        void stop()
        {
            for (int i = 0; i < MUSIC_FILE_MAX; i++)
            {
                stop(i);
            }
        }

        //--------------------------------------------------------------
        //  ���y�i�ʁj�̒�~
        //--------------------------------------------------------------
        void stop(int trackNo)
        {
            m.dxtkAudio->musicStop(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y�̈ꎞ��~
        //--------------------------------------------------------------
        void pause()
        {
            for (int i = 0; i < MUSIC_FILE_MAX; i++)
            {
                pause(i);
            }
        }

        //--------------------------------------------------------------
        //  ���y�̍ĊJ(pause����̍ĊJ)
        //--------------------------------------------------------------
        void resume()
        {
            for (int i = 0; i < MUSIC_FILE_MAX; i++)
            {
                resume(i);
            }
        }

        //--------------------------------------------------------------
        //  ���y�i�ʁj�̈ꎞ��~
        //--------------------------------------------------------------
        void pause(int trackNo)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicPause(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y�i�ʁj�̍ĊJ(pause����̍ĊJ)
        //--------------------------------------------------------------
        void resume(int trackNo)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicResume(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y�̃{�����[���ݒ�
        //--------------------------------------------------------------
        void setVolume(int trackNo, float volume)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicSetVolume(trackNo, volume);
        }

        //--------------------------------------------------------------
        //  ���y�̃{�����[���擾
        //--------------------------------------------------------------
        float getVolume(int trackNo)
        {
            if (!isInUse(trackNo)) return 0.0f;
            return m.dxtkAudio->musicGetVolume(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y�̃t�F�[�h�C���E�t�F�[�h�A�E�g
        //--------------------------------------------------------------
        void fade(int trackNo, float sec, float volume)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicFade(trackNo, sec, volume);
        }

        //--------------------------------------------------------------
        //  ���y�̃A�b�v�f�[�g
        //--------------------------------------------------------------
        void update()
        {
            m.dxtkAudio->update();
        }

        //--------------------------------------------------------------
        //  ���y�̃N���A
        //--------------------------------------------------------------
        void clear()
        {
            stop();         // ���y���~�߂�

            for (int i = 0; i < MUSIC_FILE_MAX; i++)
            {
                unload(i);  // ���y���������
            }
        }

        //--------------------------------------------------------------
        //  SoundState�̎擾
        //--------------------------------------------------------------
        DirectX::SoundState getState(int trackNo)
        {
            return m.dxtkAudio->musicGetState(trackNo);
        }

        //--------------------------------------------------------------
        //  ���[�v�̗L���̎擾
        //--------------------------------------------------------------
        bool isLooped(int trackNo)
        {
            return m.dxtkAudio->musicIsLooped(trackNo);
        }

        //--------------------------------------------------------------
        //  pan�̐ݒ�
        //--------------------------------------------------------------
        void setPan(int trackNo, float pan)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicSetPan(trackNo, pan);
        }

        //--------------------------------------------------------------
        //  pan�̎擾
        //--------------------------------------------------------------
        float getPan(int trackNo)
        {
            if (!isInUse(trackNo)) return 0.0f;
            return m.dxtkAudio->musicGetPan(trackNo);
        }

        //--------------------------------------------------------------
        //  pan�̒���
        //--------------------------------------------------------------
        float adjustPan(int trackNo, float add)
        {
            if (!isInUse(trackNo)) return 0.0f;
            return m.dxtkAudio->musicAdjustPan(trackNo, add);
        }

        //--------------------------------------------------------------
        //  pitch�̐ݒ�
        //--------------------------------------------------------------
        void setPitch(int trackNo, float pitch)
        {
            if (!isInUse(trackNo)) return;
            m.dxtkAudio->musicSetPitch(trackNo, pitch);
        }

        //--------------------------------------------------------------
        //  pitch�̎擾
        //--------------------------------------------------------------
        float getPitch(int trackNo)
        {
            if (!isInUse(trackNo)) return 0.0f;
            return m.dxtkAudio->musicGetPitch(trackNo);
        }

        //--------------------------------------------------------------
        //  pitch�̒���
        //--------------------------------------------------------------
        float adjustPitch(int trackNo, float add)
        {
            if (!isInUse(trackNo)) return 0.0f;
            return m.dxtkAudio->musicAdjustPitch(trackNo, add);
        }

        //--------------------------------------------------------------
        //  �t�H�[�}�b�g�̎擾
        //--------------------------------------------------------------
        const WAVEFORMATEX* getFormat(int trackNo)
        {
            return m.dxtkAudio->musicGetFormat(trackNo);
        }

        //--------------------------------------------------------------
        //  ���y���g�p�����ǂ���
        //--------------------------------------------------------------
        bool isInUse(int trackNo)
        {
            return m.dxtkAudio->musicIsInUse(trackNo);
        }

    }

    //==========================================================================
    //
    //      sound
    //
    //==========================================================================

    namespace sound
    {

        //--------------------------------------------------------------
        //  ���ʉ��t�@�C���̃��[�h
        //--------------------------------------------------------------
        void load(int xwbNo, const wchar_t* xwbFileName, float volume)
        {
            m.dxtkAudio->soundLoad(xwbNo, xwbFileName, volume);
        }

        //--------------------------------------------------------------
        //  ���ʉ��̍Đ�
        //--------------------------------------------------------------
        void play(int xwbNo, int trackNo)
        {
            // XWB���ǂݍ��܂�Ă��Ȃ���΃��^�[��
            if (!m.dxtkAudio->xwbIsInUse(xwbNo))
            {
                assert(!"xwb���ǂݍ��܂�Ă��Ȃ�");
                return;
            }

            m.dxtkAudio->soundPlay(xwbNo, trackNo);
        }

        //--------------------------------------------------------------
        //  ���ʉ��̒�~
        //--------------------------------------------------------------
        void stop(int xwbNo, int trackNo)
        {
            // XWB���ǂݍ��܂�Ă��Ȃ���΃��^�[��
            if (!m.dxtkAudio->xwbIsInUse(xwbNo))
            {
                assert(!"xwb���ǂݍ��܂�Ă��Ȃ�");
                return;
            }

            m.dxtkAudio->soundStop(xwbNo, trackNo);
        }

        //--------------------------------------------------------------
        //  ���ʉ��̃{�����[���ݒ�
        //--------------------------------------------------------------
        void setVolume(int xwbNo, int trackNo, float volume)
        {
            m.dxtkAudio->soundSetVolume(xwbNo, trackNo, volume);
        }

    }

    //==========================================================================
    //
    //      debug
    //
    //==========================================================================

    namespace debug
    {

        //--------------------------------------------------------------
        //  �f�o�b�O��������Z�b�g����i�L���[�Ɋi�[����j
        //--------------------------------------------------------------
        void setString(const char *format, ...)
        {
            char str[256];
            vsprintf_s(str, format, (char *)(&format + 1));
            m.debug->setString(str);
        }

        //--------------------------------------------------------------
        //  �f�o�b�O�������\��������
        //--------------------------------------------------------------
        void display(float r, float g, float b, float sx, float sy)
        {
            m.debug->display(m.context, r, g, b, sx, sy);
        }

    }

    //==========================================================================
    //
    //      font
    //
    //==========================================================================

    namespace font
    {

        //--------------------------------------------------------------
        //  �����̕`��
        //--------------------------------------------------------------
        float textOut(
            int fontNo, std::string str,
            const VECTOR2& position, const VECTOR2& scale,
            const VECTOR4& color, TEXT_ALIGN align, 
            float z, bool world)
        {
            return m.fontManager->textOut(m.context,
                fontNo, str,
                position, scale, color, 
                align, z, 
                world);
        }

        //--------------------------------------------------------------
        //  �����̕`��
        //--------------------------------------------------------------
        float textOut(
            int fontNo, std::string str,
            float x, float y, float scaleX, float scaleY,
            float r, float g, float b, float a,
            TEXT_ALIGN align, float z, bool world)
        {
            return textOut(fontNo, str,
                VECTOR2(x, y), VECTOR2(scaleX, scaleY),
                VECTOR4(r, g, b, a), 
                align, z, world);
        }

        //--------------------------------------------------------------
        //  ������̃t�H�[�}�b�g
        //--------------------------------------------------------------
        LPCSTR FormatString(LPCSTR format, ...)
        {
            static CHAR buf[1024];
            va_list args;
            va_start(args, format);
            vsnprintf_s(buf, sizeof(buf), 1024, format, args);
            va_end(args);
            return buf;
        }

    }

    //==========================================================================
    //
    //      input
    //
    //==========================================================================

    namespace input
    {

        //--------------------------------------------------------------
        //  ���̓}�l�[�W���̏�����
        //--------------------------------------------------------------
        void init()
        {
            m.inputManager->init();
        }

        //--------------------------------------------------------------
        //  ���̓}�l�[�W���̍X�V
        //--------------------------------------------------------------
        void update()
        {
            m.inputManager->update(m.hwnd);
        }

        //--------------------------------------------------------------
        //  �g���K�[�i�������u�ԁj���̎擾
        //--------------------------------------------------------------
        int TRG(int player)
        {
            return m.inputManager->getPadAddress()[player].trigger;
        }

        //--------------------------------------------------------------
        //  �g���K�[�i�������u�ԁj���̎擾
        //--------------------------------------------------------------
        int TRG_RELEASE(int player)
        {
            return m.inputManager->getPadAddress()[player].triggerUp;
        }

        //--------------------------------------------------------------
        //  �X�e�[�g�i��ԁj���̎擾
        //--------------------------------------------------------------
        int STATE(int player)
        {
            return m.inputManager->getPadAddress()[player].state;
        }

        //--------------------------------------------------------------
        //  ���s�[�g�i�J�Ԃ��j���̎擾
        //--------------------------------------------------------------
        int REPEAT(int player)
        {
            return m.inputManager->getPadAddress()[player].repeat;
        }

        //--------------------------------------------------------------
        //  �}�E�X�J�[�\����x���W���擾
        //--------------------------------------------------------------
        int getCursorPosX()
        {
            return m.inputManager->getCursorPosX();
        }

        //--------------------------------------------------------------
        //  �}�E�X�J�[�\����y���W���擾
        //--------------------------------------------------------------
        int getCursorPosY()
        {
            return m.inputManager->getCursorPosY();
        }

        //--------------------------------------------------------------
        //  �R���g���[���̏�Ԃ��擾
        //--------------------------------------------------------------
        PadState* getPadState(int player)
        {
            return &m.inputManager->getPadAddress()[player];
        }

    }

    //==========================================================================
    //
    //      view
    //
    //==========================================================================

    namespace view
    {

        //--------------------------------------------------------------
        //  ViewSettings�̏�����
        //--------------------------------------------------------------
        void init()
        {
            m.viewSettings->initialize();
        }

        //--------------------------------------------------------------
        //  �X�P�[����ݒ�
        //--------------------------------------------------------------
        void setScale(float scale)
        {
            m.viewSettings->setScaleFactor(scale);
        }

        //--------------------------------------------------------------
        //  �X�P�[�����擾
        //--------------------------------------------------------------
        float getScale()
        {
            return m.viewSettings->getScaleFactor();
        }

        //--------------------------------------------------------------
        //  Box2D�̃X�P�[����ݒ�
        //--------------------------------------------------------------
        void setBox2dScale(float s)
        {
            m.viewSettings->setBox2dScale(s);
        }

        //--------------------------------------------------------------
        //  Box2D�̃X�P�[�����擾
        //--------------------------------------------------------------
        float getBox2dScale()
        {
            return m.viewSettings->getBox2dScale();
        }

        //--------------------------------------------------------------
        //  �����̃��[���h���W��ݒ�
        //--------------------------------------------------------------
        void setCenter(float cx, float cy)
        {
            m.viewSettings->setViewCenter(VECTOR2(cx, cy));
        }

        void setCenter(VECTOR2 v)
        {
            m.viewSettings->setViewCenter(v);
        }

        //--------------------------------------------------------------
        //  ��ʍ���̃��[���h���W��ݒ�
        //--------------------------------------------------------------
        void setLeftTop(float ltx, float lty)
        {
            m.viewSettings->setViewLeftTop(VECTOR2(ltx, lty));
        }

        void setLeftTop(VECTOR2 v)
        {
            m.viewSettings->setViewLeftTop(v);
        }

        //--------------------------------------------------------------
        //  ��ʍ����̃��[���h���W��ݒ�
        //--------------------------------------------------------------
        void setLeftBottom(float lbx, float lby)
        {
            m.viewSettings->setViewLeftBottom(VECTOR2(lbx, lby));
        }

        void setLeftBottom(VECTOR2 v)
        {
            m.viewSettings->setViewLeftBottom(v);
        }

        //--------------------------------------------------------------
        //  ��ʒ����̃��[���h���W���擾
        //--------------------------------------------------------------
        VECTOR2 getCenter()
        {
            return m.viewSettings->getViewCenter();
        }

        //--------------------------------------------------------------
        //  ��ʍ���̃��[���h���W���擾
        //--------------------------------------------------------------
        VECTOR2 getLeftTop()
        {
            return m.viewSettings->getViewLeftTop();
        }

        //--------------------------------------------------------------
        //  ��ʍ����̃��[���h���W���擾
        //--------------------------------------------------------------
        VECTOR2 getLeftBottom()
        {
            return m.viewSettings->getViewLeftBottom();
        }

        //--------------------------------------------------------------
        //  ��ʉE��̃��[���h���W���擾
        //--------------------------------------------------------------
        VECTOR2 getRightTop()
        {
            return m.viewSettings->getViewRightTop();
        }

        //--------------------------------------------------------------
        //  ��ʉE���̃��[���h���W���擾
        //--------------------------------------------------------------
        VECTOR2 getRightBottom()
        {
            return m.viewSettings->getViewRightBottom();
        }

        //--------------------------------------------------------------
        //  ��ʂ̔C�ӂ̍��W�Ƀ��[���h���W��ݒ�
        //--------------------------------------------------------------
        void setByPosition(VECTOR2 screen, VECTOR2 world)
        {
            m.viewSettings->setViewByPosition(screen, world);
        }

        //--------------------------------------------------------------
        //  Box2D�̍��W�����[���h���W�ɕϊ�
        //--------------------------------------------------------------
        //VECTOR2 box2dToWorld(b2Vec2 b2Pos)
        //{
        //    return m.viewSettings->box2dToWorld(b2Pos);
        //}

        //--------------------------------------------------------------
        //  ���[���h���W���X�N���[�����W�֕ϊ�
        //--------------------------------------------------------------
        VECTOR2 worldToScreen(VECTOR2 wPos)
        {
            return m.viewSettings->worldToScreen(wPos);
        }

        VECTOR3 worldToScreen(VECTOR3 wPos)
        {
            return m.viewSettings->worldToScreen(wPos);
        }

        //--------------------------------------------------------------
        //  �X�N���[�����W�����[���h���W�ɕϊ�
        //--------------------------------------------------------------
        VECTOR2 screenToWorld(VECTOR2 sPos)
        {
            return m.viewSettings->screenToWorld(sPos);
        }

        //--------------------------------------------------------------
        //  ���[���h���W��Box2D�̍��W�ɕϊ�
        //--------------------------------------------------------------
        //b2Vec2 worldToBox2d(VECTOR2 wPos)
        //{
        //    return m.viewSettings->worldToBox2d(wPos);
        //}

    }

}

//******************************************************************************
