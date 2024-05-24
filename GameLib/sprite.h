#ifndef INCLUDED_SPRITE
#define INCLUDED_SPRITE
//******************************************************************************
//
//
//      �X�v���C�g�`��p�N���X
//
//
//******************************************************************************

//------< �C���N���[�h >---------------------------------------------------------
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>

#include "vector.h"
#include "resource_manager.h"

namespace GameLib
{

    // �����`��̃A���C��
    enum class TEXT_ALIGN
    {
        UPPER_LEFT = 0, // ����
        UPPER_MIDDLE,   // �㒆��
        UPPER_RIGHT,    // �E��
        MIDDLE_LEFT,    // ������
        MIDDLE,         // ����
        MIDDLE_RIGHT,   // �E����
        LOWER_LEFT,     // ����
        LOWER_MIDDLE,   // ������
        LOWER_RIGHT,    // �E��
    };

    //==========================================================================
    //
    //      Sprite�N���X
    //
    //==========================================================================
    class Sprite
    {
    private:
        ID3D11VertexShader*         vertexShader        = nullptr;
        ID3D11PixelShader*          pixelShader         = nullptr;
        ID3D11InputLayout*          inputLayout         = nullptr;
        ID3D11Buffer*               buffer              = nullptr;
        ID3D11RasterizerState*      rasterizerState     = nullptr;

        ID3D11ShaderResourceView*   shaderResourceView  = nullptr;
        D3D11_TEXTURE2D_DESC        tex2dDesc           = {};
        ID3D11SamplerState*         samplerState        = nullptr;
        //ID3D11DepthStencilState*    depthStencilState   = nullptr;

    public:

        //--------------------------------------------------------------
        //  �R���X�g���N�^
        //--------------------------------------------------------------
        Sprite(ID3D11Device*, const wchar_t*, int nBufSize = ( 4 ));

        //--------------------------------------------------------------
        //  �f�X�g���N�^
        //--------------------------------------------------------------
        ~Sprite();

        //--------------------------------------------------------------
        //  �X�v���C�g�`��
        //--------------------------------------------------------------
        void render(ID3D11DeviceContext*, 
            const VECTOR2&, const VECTOR2&, 
            const VECTOR2&, const VECTOR2&,
            const VECTOR2&, float,
            const VECTOR4&, 
            float z = (0.5f)
            ) const;

        //--------------------------------------------------------------
        //  �X�v���C�g�`��
        //--------------------------------------------------------------
        void render(ID3D11DeviceContext*,
            float, float, float, float, 
            float, float, float, float,
            float, float, float,
            float, float, float, float, 
            float z = (0.5f)
            ) const;

        //--------------------------------------------------------------
        //  �e�L�X�g�`��
        //--------------------------------------------------------------
        float textout(ID3D11DeviceContext*, 
            std::string, const VECTOR2&,
            const VECTOR2&, const VECTOR4&, 
            float z = ( 0.5f ), 
            bool world = ( false )) const;

        //--------------------------------------------------------------
        //  �e�L�X�g�`��
        //--------------------------------------------------------------
        float textout(ID3D11DeviceContext*, 
            std::string, float, float,
            float, float,
            float, float, float, float, 
            float z = (0.5f),
            bool world = ( false )) const;

        //--------------------------------------------------------------
        //  �l�p�|���S���`��i���_�w��j
        //--------------------------------------------------------------
        void quad(ID3D11DeviceContext*, 
            const VECTOR2(&v)[4],
            const VECTOR2&, const VECTOR2&,
            const VECTOR4&, 
            float z = (0.5f)
            ) const;

        //--------------------------------------------------------------
        //  �l�p�|���S���`��i���_�w��j
        //--------------------------------------------------------------
        void quad(ID3D11DeviceContext*, 
            const VECTOR2(&v)[4],
            float, float, float, float,
            float, float, float, float, 
            float z = (0.5f)
        ) const;

        void draw_triangle_strip(ID3D11DeviceContext*,
            const VECTOR3* pVertex, const VECTOR4* pColor, const VECTOR2* pUvs,
            int nVert, int nPolygon, float z = (0.5f)
            ) const;

        void draw_triangles_indexed(ID3D11DeviceContext*,
            const VECTOR3* pVertex, const VECTOR4* pColor, const VECTOR2* pUvs,
            short* pIndices, int nVert, int nPolygon, float z = (0.5f)
            ) const;

        void draw_triangle_fan_indexed(ID3D11DeviceContext*,
            const VECTOR3* pVertex, const VECTOR4* pColor, const VECTOR2* pUvs,
            short* pIndices, int nPolygon, float z = (0.5f)
            ) const;

        //--------------------------------------------------------------
        //  �X�v���C�g�̃T�C�Y�擾
        //--------------------------------------------------------------
        bool getSize(int& w, int& h);

        //--------------------------------------------------------------
        //  �\���̒�`
        //--------------------------------------------------------------
        struct vertex { VECTOR3 position; VECTOR4 color; VECTOR2 texcoord; };
    };

    //==========================================================================
    //
    //      SpriteBatch�N���X
    //
    //==========================================================================
    class SpriteBatch
    {
    private:
        ID3D11VertexShader*         vertexShader        = nullptr;
        ID3D11PixelShader*          pixelShader         = nullptr;
        ID3D11InputLayout*          inputLayout         = nullptr;
        ID3D11Buffer*               buffer              = nullptr;
        ID3D11RasterizerState*      rasterizerState     = nullptr;

        ID3D11ShaderResourceView*   shaderResourceView  = nullptr;
        D3D11_TEXTURE2D_DESC        tex2dDesc           = {};
        ID3D11SamplerState*         samplerState        = nullptr;
//        ID3D11DepthStencilState*    depthStencilState   = nullptr;

        size_t MAX_INSTANCES;

        //--------------------------------------------------------------
        //  �\���̒�`
        //--------------------------------------------------------------
        struct instance
        {
            DirectX::XMFLOAT4X4 ndcTransform            = {};
            VECTOR4 texcoordTransform                   = {};
            VECTOR4 color                               = {};
        };
        ID3D11Buffer* instanceBuffer;

    public:

        //--------------------------------------------------------------
        //  �\���̒�`
        //--------------------------------------------------------------
        struct vertex
        {
            VECTOR3 position                            = {};
            VECTOR2 texcoord                            = {};
        };

        //--------------------------------------------------------------
        //  �R���X�g���N�^
        //--------------------------------------------------------------
        SpriteBatch(ID3D11Device*, const wchar_t*, size_t);

        //--------------------------------------------------------------
        //  �f�X�g���N�^
        //--------------------------------------------------------------
        ~SpriteBatch();

        //--------------------------------------------------------------
        //  �O����
        //--------------------------------------------------------------
        void begin(ID3D11DeviceContext*);

        //--------------------------------------------------------------
        //  �X�v���C�g�o�b�`�`��
        //--------------------------------------------------------------
        void render(
            const VECTOR2&, const VECTOR2&, 
            const VECTOR2&, const VECTOR2&,
            const VECTOR2&, float angle, 
            const VECTOR4&, float z = (0.5f)
            );

        //--------------------------------------------------------------
        //  �e�L�X�g�`��
        //--------------------------------------------------------------
        float textout(
            std::string, const VECTOR2&,
            const VECTOR2&, const VECTOR4&, 
            TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT ), 
            float z = (0.5f), 
            bool world = ( false )
            );

        //--------------------------------------------------------------
        //  �㏈��
        //--------------------------------------------------------------
        void end(ID3D11DeviceContext*);

    private:

        D3D11_VIEWPORT viewport;

        UINT instanceCount = 0;
        instance* instances = nullptr;
    };

}

//******************************************************************************

#endif // !INCLUDED_SPRITE
