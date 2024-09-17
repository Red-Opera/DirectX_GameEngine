#pragma once
#include "../Render.h"
#include "../RenderManager.h"

namespace Graphic
{
	class IndexBuffer : public Render
	{
	public:
		IndexBuffer(DxGraphic& graphic, const std::vector<unsigned short>& indices);
		IndexBuffer(DxGraphic& graphic, std::string path, const std::vector<USHORT>& indices);

		// Bindable을(를) 통해 상속됨
		void PipeLineSet(DxGraphic& graphic) noexcept override;
		UINT GetIndexCount() const noexcept;

		static std::shared_ptr<IndexBuffer> GetRender(DxGraphic& graphic, const std::string& path, const std::vector<USHORT>& indices);
		
		template<typename...Ignore>
		static std::string CreateID(const std::string& path, Ignore&&...ignore) { return CreateID_(path); }
		std::string GetID() const noexcept override;

	protected:
		std::string path;
		UINT indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	private:
		static std::string CreateID_(const std::string& path);
	};
}