#pragma once

#include "Core/Exception/WindowException.h"
#include "Core/RenderingPipeline/Render.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class DxGraphic;
namespace Graphic { class Render; }

namespace Engine
{
	class FolderViewInspector : public Graphic::Render
	{
		struct FileItemTree;

	public:
		enum class IconType { folder, file, ParentFolder, };

		static std::unique_ptr<FolderViewInspector> instance;

		static void GetInstance(DxGraphic& graphic)
		{
			if (!instance)
				instance = std::make_unique<FolderViewInspector>(graphic);
		}

		FolderViewInspector(DxGraphic& graphic);
		FolderViewInspector(const FolderViewInspector&) = delete;
		FolderViewInspector(FolderViewInspector&&) = delete;
		FolderViewInspector& operator=(const FolderViewInspector&) = delete;
		FolderViewInspector& operator=(FolderViewInspector&&) = delete;

		std::shared_ptr<FileItemTree> CreateFileSystem();													// ������Ʈ ���� �ݿ��� ���� �ý��� ���� ���� �޼ҵ�
		std::shared_ptr<FileItemTree> BuildFileItemTree(const std::filesystem::directory_entry& rootPath);	// ������Ʈ ���� �ݿ��Ͽ� ���� Ʈ���� ����� �޼ҵ�

		void RenderFolderView(std::shared_ptr<FileItemTree> itemTree = { });
		void RenderInspector();

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		~FolderViewInspector() = default;

	private:
		struct FileItemTree
		{
			std::string name;
			std::vector<std::shared_ptr<FileItemTree>> chlidren;
			std::shared_ptr<FileItemTree> parent;

			bool isFolder;
		};

		ID3D11ShaderResourceView* GetFileTextureResourceView(std::string fileName);
		void LoadIconTexture(DxGraphic& graphic, std::string fileName, IconType iconType);

		// ���� �Լ�: ���� ������ ��ü ��� ���ڿ� ����
		std::string GetRelativePath(const std::shared_ptr<FileItemTree>& tree);
		std::string GetAbsolutePath(const std::shared_ptr<FileItemTree>& tree);

		void RenderCurrentPath(std::shared_ptr<FileItemTree> itemTree);
		void RenderParentFolderButton();
		void RenderFolderAndFileItems(std::shared_ptr<FileItemTree> itemTree);
		void RenderFolderItem(const std::shared_ptr<FileItemTree>& child);
		void RenderFileItem(const std::shared_ptr<FileItemTree>& child);

		void OpenFile(const std::shared_ptr<FileItemTree>& child);

		std::shared_ptr<FileItemTree> folderTree;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> folderIconTexture;			// ���� �̹����� �����ϴ� ����
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> goParentFolderTexture;		// ���� ������ �̵��ϱ� ��ư �̹��� ����

		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> fileIconTextures;

		std::string selectedName;
	};
}