#include "stdafx.h"

// ========================================================
//	���� ����
// ========================================================

/*
	- DirectX 3D�� �ϳ��� ���� ���(State Machine)�̱� ������ �Է� ��ġ�� ���� ����, ���� ���۰� �ѹ� ���̸� �ٸ� ������ ���� ������ �����ȴ�.
	- ���� ����(Render State)��� �Ҹ��� �������� �� � ������� �������� �����Ұ��� �����ϴ� ���� ������ �̿��Ͽ� ������ ������������ ������ ������ �� �ִ�.

	���� ���� ����
		- ID3D11RaterizerState : ������ȸ�� �ܰ��� �������� ������ �� �ִ� ���� ����
		- ID3D11BlendState : ȥ�� ������ ������ �����ϴ� ���� ����
		- ID3D11DepthStencilState : ���� �Ǵ� ���ٽ� ���� ������ �����ϴ� ����

		ID3D11RaterizerState �����ϴ� ���
			1. D3D11_RASTERIZER_DESC ����ü�� �ۼ��Ѵ�.
			2. D3D11_RASTERIZER_DESC�� �̿��Ͽ� ID3D11Device�� �ִ� CreateRaterizerState�� ȣ���Ѵ�.
			3. ID3D11DeviceContext�� �ִ� RSSetState�� �̿��Ͽ� RaterizerState�� ������ ���������ο� ����Ѵ�.

		D3D11_RASTERIZER_DESC �ɹ�
			- FillMode : ������Ʈ�� Ʋ�� ����Ұ��� �Ϲ������� ����Ұ��� ����
				1. D3D11_FILL_WIREFRAME : ���̾� �����Ӹ� �������� ���
				2. D3D11_FILL_SOLID : �Ϲ� ����ü�� �������� ���

			- CullMode : �ո� �Ǵ� �޸� ������ ���� ����
				1. D3D11_CULL_NONE : �ո� �޸� ��� �������� ���
				2. D3D11_CULL_BACK : �޸��� ���������� ���� ���
				3. D3D11_CULL_FRONT : �ո��� ���������� ���� ���

			- FrontCounterClockwise : ������ �ð�������� �� ��� �������� �����ϴ� ����
				1. true : �ð�������� ������ ������ ��� �ո�
				2. false : �ݽð�������� ������ ������ ��� �ո�

		- ���� �پ��� ���� ���� ������ ���α׷��� �ʱ�ȭ �������� ������ �� ��Ȳ�� ���� RaterizerState�� �ٲ��ֱ⸸ �ϸ� �ȴ�.
		- �ѹ� �Է��� RaterizerState�� �ٸ� RaterizerState�� �ٲܶ����� ���������� �ش� ������Ʈ�� �׸��� ����ȴ�.
		- RSSetState�� ������ RaterizerState���� �ٽ� �ʱ� ���·� ������ ���ؼ� RaterizerState ���� ��ſ� 0�� �Է����ָ� �ȴ�.
*/

namespace Sample
{
	// D3D11_RASTERIZER_DESC�� ����
	typedef struct D3D11_RASTERIZER_DESC
	{
		D3D11_FILL_MODE FillMode;	// Default : D3D11_FILL_SOLID
		D3D11_CULL_MODE CullMode;	// Default : D3D11_CULL_BACK
		BOOL FrontCounterClockwise;	// Default : false
		INT DepthBias;				// Default : 0
		FLOAT DepthBiasClamp;		// Default : 0.0f
		FLOAT SlopeScaledDepthBias;	// Default : 0.0f
		BOOL DepthCilpEnable;		// Default : true
		BOOL ScissorEnable;			// Default : false
		BOOL MultisampleEnable;		// Default : false
		BOOL AntialiasedLineEnable;	// Default : false
	} D3D11_RASTERIZER_DESC;

	class ID3D11Device
	{
		// CreateRasterizerState ����
		HRESULT CreateRasterizerState(
			const D3D11_RASTERIZER_DESC * pRasterizerDesc,	// D3D11_RASTERIZER_DESC ����
			ID3D11RasterizerState **ppRasterizerState		// ��ȯ�� RasterizerState ����
		);
	};
	
	class ID3D11DeviceContext
	{
		// RasterizerState ����
		void RSSetState(ID3D11RasterizerState * pRasterizerState);
	};
}