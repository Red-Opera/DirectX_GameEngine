#pragma once

#include "stdafx.h"
#include "Win.h"

#include "Exception/BaseException.h"
#include "Exception/ExceptionInfo.h"

#include <wrl.h>
#include <dxgidebug.h>

using Microsoft::WRL::ComPtr;

class DxGraphic
{
public:
	// =================================
	//	Inner Class
	// =================================

	class Exception : public BaseException
	{
	public:
		static string TranslateErrorCode(HRESULT hr) noexcept;

	private:
		using BaseException::BaseException;
	};

	class HRException : public Exception
	{
	public:
		HRException(int line, const char* file, HRESULT hr, vector<string> infoMessage = {}) noexcept;
		HRException(int line, string file, HRESULT hr, vector<string> infoMessage = {}) noexcept;

		const char* what() const noexcept override;

		// 예외 정보에 관한 메소드
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		string GetExptionContent() const noexcept;
		string GetErrorDescription() const noexcept;
		string GetErrorInfo() const noexcept;

	private:
		HRESULT hr;
		string info;
	};

	class RemoveException : public HRException
	{
		using HRException::HRException;

	public:
		const char* GetType() const noexcept override;

	private:
		string info;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, vector<string> infoMessage = {});
		InfoException(int line, string file, vector<string> infoMessage = {});

		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		string GetErrorInfo() const noexcept;

	private:
		string info;
	};

	DxGraphic(HWND hWnd);
	~DxGraphic() = default;

	// 복사 대입 생성자 제거
	DxGraphic(const DxGraphic&) = delete;
	DxGraphic& operator=(const DxGraphic&) = delete;

	void DrawTestTriangle(float angle, float x, float z);
	void DrawIndexed(UINT count) noexcept(!_DEBUG);

	// Get Method
	ID3D11Device*			GetDevice() { return device.Get(); }
	ID3D11DeviceContext*	GetDeviceContext() { return deviceContext.Get(); }
	ExceptionInfo&			GetInfoManager() { return infoManager; }
	DirectX::XMMATRIX		GetProjection() const noexcept { return projection; }

	// Set Method
	void SetProjection(DirectX::XMMATRIX projection) noexcept;

	void EndFrame();

	// 화면 색깔 초기화 메소드
	void ClearBuffer(float red, float green, float blue) noexcept;

private:
#ifndef NDEBUG
	ExceptionInfo infoManager;
#endif

	HRESULT CreateDevice();
	void CheckMSAAQuality();
	void SwapChainSettings(HWND hWnd);
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilBuffer();

	bool		isMSAAUsage;		// 4X MSAA 사용 여부
	UINT		msaaQuality;		// 4X MSAA의 품질 수준

	DXGI_SWAP_CHAIN_DESC swapChainDesc;			// Swap Chain Desc

	ComPtr<ID3D11Device>			device;				// D3D11 장치
	ComPtr<ID3D11DeviceContext>		deviceContext;		// D3D11 Context
	ComPtr<IDXGISwapChain>			swapChain;			// 페이지 전환을 위한 교환 사슬
	ComPtr<ID3D11Texture2D>			depthStencilBuffer;	// 깊이 º 스텐실 버퍼를 위한 2차원 텍스처
	ComPtr<ID3D11RenderTargetView>	renderTargetView;	// 렌더 대상용 2차원 텍스처
	ComPtr<ID3D11DepthStencilView>	depthStencilView;	// 깊이 º 스텐실 뷰
	D3D11_VIEWPORT					viewport;			// 뷰포트

	DirectX::XMMATRIX projection;
};