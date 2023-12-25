#pragma once
#include "stdafx.h"

// Device 생성 (반환 값 : createDeviceFlags, featureLevel, device, deviceImmediateContext)
HRESULT CreateDevice(UINT* createDeviceFlags, D3D_FEATURE_LEVEL* featureLevel, ID3D11Device** device, ID3D11DeviceContext** deviceContext);

// 4X MSAA 품질 수준 지원 점검 (반환 값 : massQuality)
void GetMSAAQuality(ID3D11Device* device, UINT* msaaQuality);

// 교환 사슬의 설정 (반환 값 : backBufferModeDESC, massDESC, swapChainDESC)
void SwapChainSettings(HWND hWnd, UINT msaaQuality, DXGI_MODE_DESC* backBufferModeDESC, DXGI_SAMPLE_DESC* massDESC, DXGI_SWAP_CHAIN_DESC* swapChainDESC);

// 교환 사슬의 생성 (반환 값 : swapChain)
void CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC swapChainDESC, IDXGISwapChain** swapChain);

// 렌더 타겟 뷰 생성 (반환 값 : renderTargetView)
void CreateRenderTargetView(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11RenderTargetView** renderTargetView);

// 깊이 º 스텐실 버퍼와 뷰 생성 (반환 값 : depthStencilBuffer, depthStencilView)
void CreateDepthStencilView(ID3D11Device* device, UINT msaaQuality, ID3D11Texture2D** depthStencilBuffer, ID3D11DepthStencilView** depthStencilView);

// 뷰포트 설정 (반환 값 : viewPort)
void CreateViewPort(ID3D11DeviceContext* deviceContext, D3D11_VIEWPORT* viewPort);