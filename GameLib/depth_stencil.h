#pragma once

#include "game_lib.h"
#include <wrl.h>

class DepthStencil {
public:
    enum class MODE {
        NONE,       // �ʏ�
        DEPTH,      // �[�x
        MASK,       // �}�X�N����
        APPLY_MASK, // �}�X�N�ɕ`��
        EXCLUSIVE,  // �}�X�N�ȊO�ɕ`��
        NUM         // ���[�h�̑���
    };

public:
    static DepthStencil& instance() {
        static DepthStencil inst(GameLib::DirectX11::getDevice());
        return inst;
    }
    void set(MODE mode);    // ���[�h�̐ݒ�
    void clear();           // �}�X�N���N���A����

private:
    DepthStencil(ID3D11Device* device);
    ~DepthStencil() {}
    DepthStencil(const DepthStencil&) = delete; // �R�s�[�R���X�g���N�^�����݂��Ȃ����Ƃ𖾎�

private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> modes[static_cast<size_t>(MODE::NUM)];
};
