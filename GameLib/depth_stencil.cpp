#include "depth_stencil.h"
#include "misc.h"

DepthStencil::DepthStencil(ID3D11Device* device)
	:modes() {
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	HRESULT hr{ S_OK };

	// MODE::NONE
	depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	hr = device->CreateDepthStencilState(&depth_stencil_desc,
		modes[static_cast<size_t>(MODE::NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), GameLib::hr_trace(hr));

    // MODE::DEPTH
    depth_stencil_desc = {};
    depth_stencil_desc.DepthEnable = TRUE;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL; // Allow writing to the depth buffer.
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; // Only pass if the current depth value is greater than or equal to the stored depth value.
    depth_stencil_desc.StencilEnable = FALSE; // Stencil test disabled for simplicity.
    hr = device->CreateDepthStencilState(&depth_stencil_desc,
        modes[static_cast<size_t>(MODE::DEPTH)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), GameLib::hr_trace(hr));

	// MODE::MASK
	depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = TRUE;//�[�x�e�X�g���s��
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//�[�x�l���������܂Ȃ�
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;//�[�x�e�X�g�ɕK�����s����i�`��͂��Ȃ����X�e���V���l�͏������ށj
	depth_stencil_desc.StencilEnable = TRUE;//�X�e���V���e�X�g���s��
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//�X�e���V���e�X�g�ɂ͕K�����i
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;//�[�x�e�X�g�Ɏ��s���ăX�e���V���e�X�g�ɐ��������ꍇ
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depth_stencil_desc,
		modes[static_cast<size_t>(MODE::MASK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), GameLib::hr_trace(hr));

	// MODE::APPLY_MASK
	depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//Turn off writes to the depth-stencil buffer.
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//Never pass the comparison.
	depth_stencil_desc.StencilEnable = TRUE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depth_stencil_desc,
		modes[static_cast<size_t>(MODE::APPLY_MASK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), GameLib::hr_trace(hr));

	// MODE::EXCLUSIVE
	depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//Turn off writes to the depth-stencil buffer.
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//Never pass the comparison.
	depth_stencil_desc.StencilEnable = TRUE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depth_stencil_desc,
		modes[static_cast<size_t>(MODE::EXCLUSIVE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), GameLib::hr_trace(hr));
}

// ���[�h���Z�b�g����
void DepthStencil::set(MODE mode) {
	GameLib::DirectX11::getContext()->OMSetDepthStencilState(
		modes[static_cast<size_t>(mode)].Get(), mode == MODE::MASK ? 1 : 0);
}

// �}�X�N���N���A����
void DepthStencil::clear() {
	GameLib::clearStencil();
}
