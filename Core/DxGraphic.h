#pragma once

#include "Win.h"

#include "Exception/BaseException.h"
#include "Exception/ExceptionInfo.h"
#include "Exception/WindowException.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <dxgidebug.h>

#include <memory>
#include <random>

using Microsoft::WRL::ComPtr;

namespace Graphic 
{ 
	class Render;
	class RenderTarget;
}

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

		// ���� ������ ���� �޼ҵ�
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

	// ���� ���� ������ ����
	DxGraphic(const DxGraphic&) = delete;
	DxGraphic& operator=(const DxGraphic&) = delete;

	void DrawTestTriangle(float angle, float x, float z);
	void DrawIndexed(UINT count) NOEXCEPTRELEASE;

	// Get Method
	ID3D11Device*			GetDevice() { return device.Get(); }
	ID3D11DeviceContext*	GetDeviceContext() { return deviceContext.Get(); }
	ExceptionInfo&			GetInfoManager() { return infoManager; }
	DirectX::XMMATRIX		GetProjection() const noexcept { return projection; }
	bool					GetMsaaUsage() const noexcept { return isMSAAUsage; }
	UINT					GetMsaaQuality() const noexcept { return msaaQuality; }

	void CreateSwapChain();
	void CreateRenderTargetView();

	DirectX::XMMATRIX GetCamera() { return camera; }

	// Set Method
	void SetProjection(DirectX::XMMATRIX projection) noexcept;
	void SetCamera(DirectX::FXMMATRIX camera) noexcept { this->camera = camera; }

	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	std::shared_ptr<Graphic::RenderTarget> GetRenderTarget();

	// ImGui ���� �޼ҵ�
	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnable() noexcept;

private:
#ifndef NDEBUG
	ExceptionInfo infoManager;
#endif

	friend class DxGraphicResource;

	HRESULT CreateDevice();
	void CheckMSAAQuality();
	void SwapChainSettings(HWND hWnd);

	bool		isMSAAUsage;		// 4X MSAA ��� ����
	UINT		msaaQuality;		// 4X MSAA�� ǰ�� ����

	DXGI_SWAP_CHAIN_DESC swapChainDesc;						// Swap Chain Desc
	std::shared_ptr<Graphic::RenderTarget> renderTarget;	// ���� Ÿ��

	ComPtr<ID3D11Device>			device;					// D3D11 ��ġ
	ComPtr<ID3D11DeviceContext>		deviceContext;			// D3D11 Context
	ComPtr<IDXGISwapChain>			swapChain;				// ������ ��ȯ�� ���� ��ȯ �罽
	ComPtr<ID3D11Texture2D>			depthStencilBuffer;		// ���� �� ���ٽ� ���۸� ���� 2���� �ؽ�ó
	ID3D11ShaderResourceView*	shaderResourceView;		// ���� ���� 2���� �ؽ�ó
	ComPtr<ID3D11DepthStencilView>	depthStencilView;		// ���� �� ���ٽ� ��
	D3D11_VIEWPORT					viewport;				// ����Ʈ

	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;			

	UINT width;
	UINT height;

	// ImGui
	bool imGuiEnable = true;
};