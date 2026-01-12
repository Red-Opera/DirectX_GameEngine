#pragma once

#include "Core/Exception/GraphicsException.h"

#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

namespace Graphic
{
    // DirectX 상수 버퍼를 관리하는 템플릿 기본 클래스
    template<typename BufferType>
    class ConstantBuffer : public Render
    {
    public:
        // 초기 데이터와 함께 상수 버퍼를 생성하는 생성자
        ConstantBuffer(const BufferType& bufferList, UINT slot = 0u) : slot(slot)
        {
            // DirectX 11 상수 버퍼 설정 구조체 초기화
            D3D11_BUFFER_DESC constantBufferDesc;
            constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            constantBufferDesc.ByteWidth = sizeof(bufferList);
            constantBufferDesc.StructureByteStride = 0;
            constantBufferDesc.MiscFlags = 0;

            // 초기 데이터 설정
            D3D11_SUBRESOURCE_DATA initData = {};
            initData.pSysMem = &bufferList;

            // DirectX 디바이스를 통해 상수 버퍼 생성
            HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
			Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "초기 데이터와 상수 버퍼 설정 정보로 상수 버퍼 생성 실패");
        }

        // 초기 데이터 없이 상수 버퍼를 생성하는 생성자
        ConstantBuffer(UINT slot = 0u) : slot(slot)
        {
            // DirectX 11 상수 버퍼 설정 (초기 데이터 없음)
            D3D11_BUFFER_DESC constantBufferDesc;
            constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            constantBufferDesc.ByteWidth = sizeof(BufferType);
            constantBufferDesc.MiscFlags = 0;
            constantBufferDesc.StructureByteStride = 0;

            // 초기 데이터 없이 버퍼 생성
            HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
			Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "초기 데이터 없이 상수 버퍼 설정 정보로 상수 버퍼 생성 실패");
        }

        // 상수 버퍼의 데이터를 업데이트
        void Update(const BufferType& consts)
        {
            // GPU 메모리를 CPU에서 접근 가능하게 매핑
            D3D11_MAPPED_SUBRESOURCE map;
            HRESULT hr = GetDeviceContext(Window::GetDxGraphic())->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			Require::Check(hr, ErrorCode::GRAPHICS_MapUnmapFailed, "상수 버퍼 세이더 데이터 업데이트를 위한 매핑 실패");

            // 새로운 데이터를 GPU 메모리에 복사
            memcpy(map.pData, &consts, sizeof(consts));
            
            // 매핑 해제 (GPU가 다시 접근 가능하게 함)
            GetDeviceContext(Window::GetDxGraphic())->Unmap(constantBuffer.Get(), 0);
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;    // DirectX 11 상수 버퍼 객체
        UINT slot;                                              // 셰이더에서 바인딩될 슬롯 번호
    };

    // 버텍스 셰이더용 상수 버퍼 클래스
    template<typename BufferType>
    class VertexConstantBuffer : public ConstantBuffer<BufferType>
    {
        // 부모 클래스의 protected 멤버들을 현재 클래스에서 사용하기 위한 선언
        using ConstantBuffer<BufferType>::constantBuffer;
        using ConstantBuffer<BufferType>::slot;
        using ConstantBuffer<BufferType>::GetInfoManager;
        using Render::GetDeviceContext;

    public:
        using ConstantBuffer<BufferType>::ConstantBuffer; // 부모 클래스의 생성자들을 그대로 사용

        // 버텍스 셰이더 파이프라인에 상수 버퍼를 바인딩
        void SetRenderPipeline() NOEXCEPTRELEASE override
        {
			Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()); }, ErrorCode::GRAPHICS_BindFailed, "버텍스 셰이더에 상수 버퍼 바인딩 실패");
        }

        // 초기 데이터와 함께 버텍스 상수 버퍼를 생성하거나 기존 객체 반환
        static std::shared_ptr<VertexConstantBuffer> GetRender(const BufferType& constBuffers, UINT slot = 0)
        {
            return RenderManager::GetRender<VertexConstantBuffer>(constBuffers, slot);
        }

        // 빈 버텍스 상수 버퍼를 생성하거나 기존 객체 반환
        static std::shared_ptr<VertexConstantBuffer> GetRender(UINT slot = 0)
        {
            return RenderManager::GetRender<VertexConstantBuffer>(slot);
        }

        // 초기 데이터를 포함한 고유 식별자 생성 (내부 구현용)
        static std::string CreateID(const BufferType&, UINT slot) { return CreateID(slot); }

        // RenderManager에서 사용할 고유 식별자 생성
        static std::string CreateID(UINT slot = 0) 
        { 
            using namespace std::string_literals;
            return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
        }

        // 현재 객체의 고유 식별자 반환
        std::string GetID() const noexcept override { return CreateID(slot); }
    };

    // 픽셀 셰이더용 상수 버퍼 클래스
    template<typename BufferType>
    class PixelConstantBuffer : public ConstantBuffer<BufferType>
    {
        // 부모 클래스의 protected 멤버들을 현재 클래스에서 사용하기 위한 선언
        using ConstantBuffer<BufferType>::constantBuffer;
        using ConstantBuffer<BufferType>::slot;
        using ConstantBuffer<BufferType>::GetInfoManager;
        using Render::GetDeviceContext;

    public:
        using ConstantBuffer<BufferType>::ConstantBuffer; // 부모 클래스의 생성자들을 그대로 사용

        // 픽셀 셰이더 파이프라인에 상수 버퍼를 바인딩
        void SetRenderPipeline() NOEXCEPTRELEASE override
        {
			Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()); }, ErrorCode::GRAPHICS_BindFailed, "픽셀 셰이더에 상수 버퍼 바인딩 실패");
        }

        // 초기 데이터와 함께 픽셀 상수 버퍼를 생성하거나 기존 객체 반환
        static std::shared_ptr<PixelConstantBuffer> GetRender(const BufferType& constBuffers, UINT slot = 0)
        {
            return RenderManager::GetRender<PixelConstantBuffer>(constBuffers, slot);
        }

        // 빈 픽셀 상수 버퍼를 생성하거나 기존 객체 반환
        static std::shared_ptr<PixelConstantBuffer> GetRender(UINT slot = 0)
        {
            return RenderManager::GetRender<PixelConstantBuffer>(slot);
        }

        // 초기 데이터를 포함한 고유 식별자 생성 (내부 구현용)
        static std::string CreateID(const BufferType&, UINT slot) { return CreateID(slot); }

        // RenderManager에서 사용할 고유 식별자 생성
        static std::string CreateID(UINT slot = 0)
        {
            using namespace std::string_literals;
            return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
        }

        // 현재 객체의 고유 식별자 반환
        std::string GetID() const noexcept override { return CreateID(slot); }
    };
}