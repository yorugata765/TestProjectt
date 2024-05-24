#ifndef SYSTEM_H_
#define SYSTEM_H_

namespace GameLib
{
    namespace system
    {
        static const float BOX2D_SCALE                  = 100.0f;   // Box2D�p
        static const float FRAME_RATE                   = 60.0f;    // �t���[�����[�g

        static const int TEXTURE_NUM                    = 256U;     // �e�N�X�`���̍ő喇��
        static const int DEBUG_INSTANCE_NUM             = 8192U;    // �f�o�b�O�����̍ő�\����
        static const int PRIMITIVE_BATCH_INSTANCE_NUM   = 4096U;    // �v���~�e�B�u�o�b�`�̍ő�C���X�^���X��
    }
}

#endif//SYSTEM_H_
