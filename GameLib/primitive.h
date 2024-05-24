#ifndef INCLUDED_PRIMITIVE
#define INCLUDED_PRIMITIVE
//******************************************************************************
//
//
//      �v���~�e�B�u�`��p
//
//
//******************************************************************************

//------< include >-------------------------------------------------------------
#include <d3d11.h>
#include <DirectXMath.h>
#include "vector.h"

namespace GameLib
{

    const int PRIMITIVE_VERTEX_NUM      = 130;
    const int PRIMITIVE_CIRCLE_SIDE_NUM = 32;

    //==========================================================================
    //
    //      Primitive�N���X
    //
    //==========================================================================
    class Primitive
    {

        ID3D11VertexShader*         vertexShader        = nullptr;
        ID3D11PixelShader*          pixelShader         = nullptr;
        ID3D11InputLayout*          inputLayout         = nullptr;
        ID3D11Buffer*               buffer              = nullptr;
        ID3D11RasterizerState*      rasterizerState     = nullptr;
        //ID3D11DepthStencilState*    depthStencilState   = nullptr;

        ID3D11Device*               device              = nullptr;
    public:
        Primitive(ID3D11Device*);
        ~Primitive();

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(ID3D11DeviceContext*, const VECTOR2&, const VECTOR2&,
            const VECTOR2&, float, const VECTOR4&, 
            float z = (0.5f)) const;

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(ID3D11DeviceContext*, const VECTOR2&, const VECTOR2&,
            const VECTOR4&, float, 
            float z = (0.5f)) const;

        //--------------------------------------------------------------
        //  �~�`��
        //--------------------------------------------------------------
        void circle(ID3D11DeviceContext*, const VECTOR2&, float, 
            const VECTOR2&, float, 
            const VECTOR4&, int, 
            float z = (0.5f)) const;

        //--------------------------------------------------------------
        //  �l�p�|���S���`��i���_�w��j
        //--------------------------------------------------------------
        void quad(ID3D11DeviceContext*, const VECTOR2(&)[4],
            const VECTOR4&, float z = (0.5f)) const;

        void draw_triangle_fan(ID3D11DeviceContext*,
            const VECTOR2[], const VECTOR4&, int, float z = (0.5f)) const;

        //--------------------------------------------------------------
        //  �\���̒�`
        //--------------------------------------------------------------
        struct vertex { VECTOR3 position; VECTOR4 color; };

    };

    //==========================================================================
    //
    //      PrimitiveBatch�N���X
    //
    //==========================================================================
    class PrimitiveBatch
    {
    private:

        ID3D11VertexShader*         vertexShader        = nullptr;
        ID3D11PixelShader*          pixelShader         = nullptr;
        ID3D11InputLayout*          inputLayout         = nullptr;
        ID3D11Buffer*               buffer              = nullptr;
        ID3D11RasterizerState*      rasterizerState     = nullptr;
        //ID3D11DepthStencilState*    depthStencilState   = nullptr;

        size_t MAX_INSTANCES;
        struct instance
        {
            DirectX::XMFLOAT4X4 ndcTransform = {};
            VECTOR4 color;
        };
        ID3D11Buffer* instanceBuffer                    = nullptr;

    public:

        struct vertex
        {
            VECTOR3 position;
        };

        PrimitiveBatch(ID3D11Device*, size_t maxInstance = (256));
        ~PrimitiveBatch();

        //--------------------------------------------------------------
        //  �O����
        //--------------------------------------------------------------
        void begin(ID3D11DeviceContext*);

        //--------------------------------------------------------------
        //  ��`�`��
        //--------------------------------------------------------------
        void rect(const VECTOR2&, const VECTOR2&,
            const VECTOR2&, float,
            const VECTOR4&, float z = ( 0.5f ));

        //--------------------------------------------------------------
        //  ���`��
        //--------------------------------------------------------------
        void line(const VECTOR2&, const VECTOR2&,
            const VECTOR4&, float width, float z = ( 0.5f ));

        //--------------------------------------------------------------
        //  �㏈��
        //--------------------------------------------------------------
        void end(ID3D11DeviceContext*);

    private:
        D3D11_VIEWPORT viewport = {};

        UINT instanceCount      = 0;
        instance* instances     = nullptr;
    };

}

//******************************************************************************

#endif // !INCLUDED_PRIMITIVE
