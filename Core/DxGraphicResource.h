#pragma once

#include "DxGraphic.h"

class DxGraphic;

/**
 * DirectX 그래픽 리소스에 대한 접근을 제공하는 헬퍼 클래스
 * 
 * 이 클래스를 상속받은 클래스들만이 DirectX 핵심 객체들에 접근할 수 있습니다.
 */
class DxGraphicResource
{
protected:
    /**
     * DirectX 11 Device Context에 대한 접근을 제공
     * 
     * 매개변수 : graphic DxGraphic 객체의 참조
     * 
     * Device Context는 GPU 명령어 실행, 리소스 바인딩, 렌더링 상태 설정에 사용됩니다.
     */
    static ID3D11DeviceContext* GetDeviceContext(DxGraphic& graphic) noexcept;
    
    /**
     * DirectX 11 Device에 대한 접근을 제공
     * 
     * 매개변수 : graphic DxGraphic 객체의 참조
     * 
     * Device는 버퍼, 텍스처, 셰이더 등의 DirectX 리소스 생성에 사용됩니다.
     */
    static ID3D11Device* GetDevice(DxGraphic& graphic) noexcept;
    
    /**
     * DirectX 예외 정보 관리자에 대한 접근을 제공
     * 
     * 매개변수 : graphic DxGraphic 객체의 참조
     * 반환 값 : ExceptionInfo& 예외 정보 관리자의 참조
     * 
     * @note DirectX API 호출 시 발생하는 오류 정보를 추적하고 디버깅하는 데 사용 (Debug 모드에서만 사용 가능)
     */
    static ExceptionInfo& GetInfoManager(DxGraphic& graphic);
};