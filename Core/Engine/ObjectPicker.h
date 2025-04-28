#pragma once

class Object;

namespace Engine
{
    class ObjectPicker
    {
    public:
        // 싱글톤 인스턴스 얻기
        static ObjectPicker& Get()
        {
            static ObjectPicker instance;
            return instance;
        }

        // 화면 좌표에서 레이를 생성하는 함수
        void CreateRayFromScreenPoint
        (
            int screenX, int screenY,
            DirectX::XMVECTOR& rayOrigin,
            DirectX::XMVECTOR& rayDirection,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projMatrix,
            int screenWidth, int screenHeight
        );

        // 모든 오브젝트에 대해 레이 캐스팅 수행
        std::shared_ptr<Object> PickObjectAtScreenPoint
        (
            int screenX, int screenY,
            const std::vector<std::shared_ptr<Object>>& objects,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projMatrix,
            int screenWidth, int screenHeight
        );

    private:
        ObjectPicker() = default;
        ~ObjectPicker() = default;

        // 레이-오브젝트 교차 검사 함수
        bool IntersectRayWithObject
        (
            const DirectX::XMVECTOR& rayOrigin,
            const DirectX::XMVECTOR& rayDirection,
            const std::shared_ptr<Object>& object,
            float& outDistance
        );
    };
}