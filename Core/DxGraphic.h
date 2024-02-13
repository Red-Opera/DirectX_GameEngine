#pragma once

#include "Win.h"

#include "Exception/BaseException.h"
#include "Exception/ExceptionInfo.h"

#include <d3d11.h>
#include <DirectXMath.h>
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
		static std::string TranslateErrorCode(HRESULT hr) noexcept;

	private:
		using BaseException::BaseException;
	};

	class HRException : public Exception
	{
	public:
		HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessage = {}) noexcept;
		HRException(int line, std::string file, HRESULT hr, std::vector<std::string> infoMessage = {}) noexcept;

		const char* what() const noexcept override;

		// 예외 정보에 관한 메소드
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetExptionContent() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT hr;
		std::string info;
	};

	class RemoveException : public HRException
	{
		using HRException::HRException;

	public:
		const char* GetType() const noexcept override;

	private:
		std::string info;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMessage = {});
		InfoException(int line, std::string file, std::vector<std::string> infoMessage = {});

		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;

	private:
		std::string info;
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
	bool					GetMsaaUsage() const noexcept { return isMSAAUsage; }
	UINT					GetMsaaQuality() const noexcept { return msaaQuality; }

	DirectX::XMMATRIX GetCamera() { return camera; }

	// Set Method
	void SetProjection(DirectX::XMMATRIX projection) noexcept;
	void SetCamera(DirectX::FXMMATRIX camera) noexcept { this->camera = camera; }

	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();

	// ImGui 관련 메소드
	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnable() noexcept;

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
	DirectX::XMMATRIX camera;			

	// ImGui
	bool imGuiEnable = true;
};