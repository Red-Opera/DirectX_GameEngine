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

		std::shared_ptr<FileItemTree> CreateFileSystem();										// ������Ʈ ���� �ݿ��� ���� �ý��� ���� ���� �޼ҵ�
		std::shared_ptr<FileItemTree> BuildFileItemTree(const std::filesystem::path& rootPath);	// ������Ʈ ���� �ݿ��Ͽ� ���� Ʈ���� ����� �޼ҵ�

		void RenderFolderView(std::shared_ptr<FileItemTree> itemTree = { });
		void RenderInspector();

		// Render��(��) ���� ��ӵ�
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

		// ���� �Լ�: ���� ������ ��ü ��� ���ڿ� ����
		std::string GetRelativePath(const std::shared_ptr<FileItemTree>& tree);

		std::shared_ptr<FileItemTree> folderTree;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fileIconTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> folderIconTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> goParentFolderTexture;

		std::string selectedName;
	};
}