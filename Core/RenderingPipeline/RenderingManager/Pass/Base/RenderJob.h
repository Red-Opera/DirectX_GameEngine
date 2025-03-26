#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/Draw/OcclusionCulling.h"
#include "Core/Camera/CameraViewFrustumCulling.h"

#include <memory>

class Drawable;
class DxGraphic;
class RenderStep;

namespace RenderGraphNameSpace
{
	class RenderJob
	{
	public:
		RenderJob(const RenderStep* renderStep, const Drawable* drawable);

		void Excute(bool isPassFrustumCulling = true) NOEXCEPTRELEASE;

		// Occlusion Culling 활성화/비활성화
		void EnableOcclusionCulling(bool enable);

		// Occlusion Culling 상태 확인
		bool IsOcclusionCullingEnabled() const;

		// Occlusion Query 시작
		void BeginOcclusionQuery();

		// Occlusion Query 종료
		void EndOcclusionQuery();

		// 오브젝트가 보이는지 확인
		bool IsVisibleAfterOcclusionTest() const;

		bool UpdateOcclusionVisibility();

		// Frustum Culling 관련 기능 추가
		void EnableFrustumCulling(bool enable);
		bool IsFrustumCullingEnabled() const;

		static void SetViewFrustum(const CameraViewFrustumCulling& frustum);
		static CameraViewFrustumCulling& GetViewFrustum();
		bool IsInViewFrustum() const;

	private:
		const class Drawable* drawable;
		const class RenderStep* renderStep;

		static std::unique_ptr<class OcclusionCulling> m_occlusionCulling;
		bool m_useOcclusionCulling = false;

		// Frustum Culling 관련 추가
		static CameraViewFrustumCulling viewFrustumCulling;
		static bool useViewFrustum;
	};
}