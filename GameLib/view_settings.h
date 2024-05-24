#pragma once

//******************************************************************************
//
//
//      ViewSettings
//
//
//******************************************************************************

//------< �C���N���[�h >----------------------------------------------------------
#include "view_settings.h"
#include "vector.h"
#include "game_lib.h"

//#include <Box2D/Box2D.h>

namespace GameLib
{

    //==========================================================================
    //
    //      ViewSettings�N���X
    //
    //==========================================================================
    class ViewSettings
    {
    private:
        float   scaleFactor;    // ��ʂɕ`�悳���C���[�W�̃X�P�[��
        float   box2dScale;     // Box2D��1m��scaleFactor1.0f�̏�Ԃŉ��s�N�Z���ł��邩
        VECTOR2 vCenter;        // ��ʂ̒��S���W
        VECTOR2 vLeftTop;       // ��ʍ���̍��W
        VECTOR2 vLeftBottom;    // ��ʍ����̍��W

        VECTOR2 vRightTop;       // ��ʉE��̍��W
        VECTOR2 vRightBottom;    // ��ʉE���̍��W

    public:
        ViewSettings();
        ~ViewSettings();

        void initialize();

        void setScaleFactor(float s);
        void setBox2dScale(float b)                 { box2dScale = b; }
        void setViewCenter(VECTOR2);
        void setViewLeftTop(VECTOR2);
        void setViewLeftBottom(VECTOR2);
        void setViewByPosition(VECTOR2, VECTOR2);

        float getScaleFactor()                      { return scaleFactor; }
        float getBox2dScale()                       { return box2dScale; }
        VECTOR2 getViewCenter()                     { return vCenter; }
        VECTOR2 getViewLeftTop()                    { return vLeftTop; }
        VECTOR2 getViewLeftBottom()                 { return vLeftBottom; }
        VECTOR2 getViewRightTop()                   { return vRightTop; }
        VECTOR2 getViewRightBottom()                { return vRightBottom; }

        VECTOR2 worldToScreen(const VECTOR2&);
        VECTOR3 worldToScreen(const VECTOR3&);
        VECTOR2 screenToWorld(VECTOR2);

        //b2Vec2  worldToBox2d(VECTOR2);
        //VECTOR2 box2dToWorld(b2Vec2);
    };
}
