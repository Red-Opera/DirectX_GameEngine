#pragma once
#include "DynamicConstantBuffer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"
#include "Core/Window.h"

namespace Graphic
{
    // 동적 상수 버퍼를 지원하는 확장된 상수 버퍼 추상 클래스
    class ConstantBufferEx : public Render
    {
    public:
        // 동적 상수 버퍼의 데이터를 업데이트
        void Update(const DynamicConstantBuffer::Buffer& buffer)
        {
            assert(&buffer.GetLayout() == &GetLayout()); // 레이아웃 일치성 검증

            // GPU 메모리를 CPU에서 접근 가능하게 매핑
            D3D11_MAPPED_SUBRESOURCE map;
            HRESULT hr = GetDeviceContext(Window::GetDxGraphic())->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			Require::Check(hr, ErrorCode::GRAPHICS_MapUnmapFailed, "동적 상수 버퍼 데이터 업데이트를 위한 매핑 실패");

            // 동적 버퍼의 데이터를 GPU 메모리에 복사
            memcpy(map.pData, buffer.data(), buffer.size());
            GetDeviceContext(Window::GetDxGraphic())->Unmap(constantBuffer.Get(), 0);
        }

        // 현재 상수 버퍼의 레이아웃을 반환하는 순수 가상 함수
        virtual const DynamicConstantBuffer::LayoutType& GetLayout() const noexcept = 0;

    protected:
        // 동적 레이아웃을 사용하는 상수 버퍼 생성자
        ConstantBufferEx(
            const DynamicConstantBuffer::LayoutType& layoutIn,
            UINT slot,
            const DynamicConstantBuffer::Buffer* buffer) : slot(slot)
        {
            // 동적 레이아웃 크기 기반 상수 버퍼 설정
            D3D11_BUFFER_DESC constantBufferDesc;
            constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            constantBufferDesc.ByteWidth = (UINT)layoutIn.GetLayoutByteSize(); // 동적 레이아웃 크기 사용
            constantBufferDesc.StructureByteStride = 0;
            constantBufferDesc.MiscFlags = 0;

            // 초기 데이터가 제공된 경우
            if (buffer != nullptr)
            {
                D3D11_SUBRESOURCE_DATA initData = {};
                initData.pSysMem = buffer->data();

                HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
                Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "초기 데이터로 동적 상수 버퍼 생성 실패");
            }

            // 빈 버퍼 생성
            else
            {
                HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
                Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "빈 데이터로 동적 상수 버퍼 생성 실패");
            }
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;    // DirectX 11 상수 버퍼 객체
        UINT slot;                                              // 셰이더 슬롯 번호
    };

    // 픽셀 셰이더용 확장 상수 버퍼 클래스
    class PixelConstantBufferEx : public ConstantBufferEx
    {
    public:
        using ConstantBufferEx::ConstantBufferEx; // 부모 클래스의 생성자들을 그대로 사용

        // 픽셀 셰이더 파이프라인에 상수 버퍼를 바인딩
        void SetRenderPipeline() NOEXCEPTRELEASE override 
        { 
			Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()); }, ErrorCode::GRAPHICS_BindFailed, "픽셀 셰이더에 상수 버퍼 바인딩 실패");
        }
    };

    // 버텍스 셰이더용 확장 상수 버퍼 클래스
    class VertexConstantBufferEx : public ConstantBufferEx
    {
    public:
        using ConstantBufferEx::ConstantBufferEx; // 부모 클래스의 생성자들을 그대로 사용

        // 버텍스 셰이더 파이프라인에 상수 버퍼를 바인딩
        void SetRenderPipeline() NOEXCEPTRELEASE override
        {
			Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()); }, ErrorCode::GRAPHICS_BindFailed, "버텍스 셰이더에 상수 버퍼 바인딩 실패");
        }
    };

    // 캐싱 기능을 제공하는 동적 상수 버퍼 템플릿 클래스
    template<class T>
    class CachingConstantBufferEx : public T
    {
    public:
        // 완전한 레이아웃으로 캐싱 상수 버퍼를 생성
        CachingConstantBufferEx(const DynamicConstantBuffer::CompleteLayout& layout, UINT slot)
            : T(*layout.GetLayout(), slot, nullptr), 
              buffer(DynamicConstantBuffer::Buffer(layout))
        {
        
        }
        
        // 초기 데이터와 함께 캐싱 상수 버퍼를 생성
        CachingConstantBufferEx(const DynamicConstantBuffer::Buffer& buffer, UINT slot)
            : T(buffer.GetLayout(), slot, &buffer),
              buffer(buffer)
        {
        
        }
        
        // 현재 상수 버퍼의 레이아웃 반환
        const DynamicConstantBuffer::LayoutType& GetLayout() const noexcept override
        {
            return buffer.GetLayout();
        }
        
        // 내부 캐싱 버퍼 데이터 반환
        const DynamicConstantBuffer::Buffer& GetBuffer() const noexcept
        {
            return buffer;
        }
        
        // 새로운 버퍼 데이터로 교체
        void SetBuffer(const DynamicConstantBuffer::Buffer& bufferIn)
        {
            buffer.set(bufferIn);
            IsNotMatch = true; // 변경 플래그 설정
        }
        
        // 캐싱 로직을 포함한 렌더 파이프라인 설정
        void SetRenderPipeline() noexcept override
        {
            // 데이터가 변경된 경우에만 업데이트
            if (IsNotMatch)
            {
                T::Update(buffer);
                IsNotMatch = false;
            }
        
            T::SetRenderPipeline(); // 실제 셰이더 파이프라인에 바인딩
        }
        
        // Technique 객체의 방문을 받아들이는 Visitor 패턴 구현
        void Accept(TechniqueBase& tech) override
        {
            if (tech.VisitBuffer(buffer)) // Technique이 버퍼를 방문하고 수정 여부를 반환
                IsNotMatch = true;
        }

    private:
        bool IsNotMatch = false; // 데이터 변경 여부를 추적하는 플래그
        DynamicConstantBuffer::Buffer buffer; // 캐싱된 동적 상수 버퍼 데이터
    };

    // 픽셀 셰이더용 캐싱 동적 상수 버퍼 타입 별칭
    using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
    
    // 버텍스 셰이더용 캐싱 동적 상수 버퍼 타입 별칭
    using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;
}