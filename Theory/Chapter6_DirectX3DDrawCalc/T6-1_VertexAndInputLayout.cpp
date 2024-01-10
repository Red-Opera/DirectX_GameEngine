#include "stdafx.h"

// ========================================================
//	정점과 입력 배치
// ========================================================

/*
	- 정점의 정보 중 위치 정보 이외에 다른 자료를 담을 수 있는데 이러한 정보를 담는 구조체를 만들어야 한다.
	- 정점 구조체를 만들고 구조체의 맴버가 무슨 용도로 사용하는지 알려주기 위해 I3D11InputLayout라는 입력 배치(Input Layout) 객체를 만들어야 한다.
	- Input Layout을 만들기 위해서 구조체 맴버가 어떠한 구조로 되어 있는지 알려주는 D3D11_INPUT_ELEMENT_DESC를 작성해야 한다.

	D3D11_INPUT_ELEMENT_DESC 맴버
		- SemanticName : 정점 셰이더 단계에서 사용할 이름을 이 맴버에 적음
		- SemanticIndex : name1, name2와 같이 같은 이름을 사용할 수 있도록 만든 인덱스
		- InputSlot : 0 ~ 15까지 숫자로 입력될 수 있으며, 정점 정보가 맴버가 다른 구조체에 있을 때 사용한다.
		- AlignedByteOffset : 앞에 있는 맴버의 용량을 모두 더한 값이 현재 오프셋

	- D3D11_INPUT_ELEMENT_DESC를 작성하고 나서 ID3D11Device에 있는 CreateInputLayout을 이용하여 InputLayout을 생성한다.

		pShaderBytecodeWithInputSignature
			- 정점 셰이더에서 입력으로 받을 데이터의 구조나 형식을 입력 서명(Input Signature)라고 한다.
			- 이 Input Signature는 정점 셰이더의 : 뒤에 오는 변수명과 입력 배치때 적은 이름과 대응되어야 한다.
			- 입력 배치 생성 및 매칭 후 서로 다른 셰이더에서 재사용할 수 있다.

			정점 정보 구조체를 Input Signature(Vertex Shader 코드)에 적용
				정점 정보 구조체
					struct Vertex
					{
						XMFLOAT3 Pos,
						XMFLOAT4 Color
					};
				
				Input Signature
					VertexOut VS(int3 pos : POSITION, float4 color : COLOR) { }

	- InputLaout을 생성한 후 deviceContext에 있는 IASetInputLaout을 이용하여 렌더링 파이프라인에 묶는다
	- 해당 InputLayout을 이용하여 렌더링을하면 다른 InputLayout으로 바뀔때까지 지속적으로 해당 InputLayout으로 그린다.
*/

namespace Sample
{
	// 정점 정보 구조체 선언
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Texture
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 lt;
		XMFLOAT2 rb;
	};

	// D3D11_INPUT_ELEMENT_DESC의 구조
	typedef struct D3D11_INPUT_ELEMENT_DESC
	{
		LPCSTR						SemanticName;			// 정점 셰이더 단계에 쓰일 이름
		UINT						SemanticIndex;			// SematicName의 인덱스
		DXGI_FORMAT					Format;					// 해당 맴버의 자료 형식
		UINT						InputSlot;				// 정점 버퍼 슬롯
		UINT						AlignedByteOffset;		// 해당 맴버가 위치하는 byte위치
		D3D11_INPUT_CLASSIFICATION	InputSlotClass;			// Vertex 값 또는 인스턴싱으로 사용하는지 여부
		UINT						InstanceDataStepRate;	// 인스턴싱에 쓰일때 업데이트가 되는지 여부
	};

	// CreateInputLayout 메소드 구성
	class ID3D11Device
	{
		HRESULT CreateInputLayout(
			const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,	// 정점 구조체 (D3D11_INPUT_ELEMENT_DESC)
			UINT NumElement,									// D3D11_INPUT_ELEMENT_DESC의 원소 개수
			const void* pShaderBytecodeWithInputSignature,		// 정점 셰이더 컴파일 코드를 가리는 포인터
			SIZE_T BytecodeLenth,								// 정점 셰이더 컴파일 코드의 크기
			ID3D11InputLayout** ppInputLayout					// InputLayout을 반환될 변수
		);
	};

	// D3D11_INPUT_ELEMENT_DESC 작성법
	// Vertex 버전
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Texture 버전
	D3D11_INPUT_ELEMENT_DESC textureDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// ID3D11Effect로 CreateInputLaout 사용법
	void Sample()
	{
		ID3DX11Effect* fx = nullptr;

		ID3DX11EffectTechnique* tech;
		ID3D11InputLayout* inputLaout;

		tech = fx->GetTechniqueByName("Tech");

		D3DX11_PASS_DESC passDesc;
		tech->GetPassByIndex(0)->GetDesc(&passDesc);

		//HR(device->CreateInputLaout(vertexDesc, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, *inputLaout);
	}
}