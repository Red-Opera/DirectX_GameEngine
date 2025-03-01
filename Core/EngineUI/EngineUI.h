#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/RenderingPipeline/Render.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class DxGraphic;
namespace Graphic { class Render; }

namespace Engine
{
	class EngineUI : public Graphic::Render
	{
		struct FileItemTree;

	public:
		enum class IconType { folder, file, ParentFolder, };

		static std::unique_ptr<EngineUI> instance;

		static EngineUI& GetInstance(DxGraphic& graphic)
		{
			if (!instance)
			{
				instance = std::make_unique<EngineUI>(graphic);
			}
			return *instance;
		}

		EngineUI(DxGraphic& graphic);
		EngineUI(const EngineUI&) = delete;
		EngineUI(EngineUI&&) = delete;
		EngineUI& operator=(const EngineUI&) = delete;
		EngineUI& operator=(EngineUI&&) = delete;

		std::shared_ptr<FileItemTree> CreateFileSystem();										// 프로젝트 구조 반영한 파일 시스템 생성 시작 메소드
		std::shared_ptr<FileItemTree> BuildFileItemTree(const std::filesystem::path& rootPath);	// 프로젝트 구조 반영하여 파일 트리를 만드는 메소드

		void RenderFolderView(std::shared_ptr<FileItemTree> itemTree = { });
		void RenderInspector();

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		~EngineUI() = default;

	private:
		struct FileItemTree
		{
			std::string name;
			std::vector<std::shared_ptr<FileItemTree>> chlidren;
			std::shared_ptr<FileItemTree> parent;

			bool isFolder;
		};

		void LoadIconTexture(DxGraphic& graphic, std::string fileName, IconType iconType);

		// 헬퍼 함수: 현재 폴더의 전체 경로 문자열 생성
		std::string GetRelativePath(const std::shared_ptr<FileItemTree>& tree);

		std::shared_ptr<FileItemTree> folderTree;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fileIconTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> folderIconTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> goParentFolderTexture;

		std::string selectedName;
	};
}