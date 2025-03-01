#include "stdafx.h"
#include "EngineUI.h"

#include "Core/Exception/EngineUIException.h"
#include "External/Imgui/imgui.h"

#include <vector>
#include <string>
#include <filesystem>

namespace Engine
{
	namespace fileSystem = std::filesystem;

	void InstanceDelect(EngineUI* ptr) noexcept { }

	std::unique_ptr<EngineUI, void(*)(EngineUI*)> EngineUI::instance(new EngineUI(), InstanceDelect);

	EngineUI::EngineUI() : selectedName("")
	{
		if (!folderTree)
			folderTree = std::make_shared<FileItemTree>(CreateFileSystem());
	}

	EngineUI::FileItemTree EngineUI::CreateFileSystem()
	{
		// ������Ʈ ����(�ֻ��� ����)�� ������
		fileSystem::path rootPath = fileSystem::current_path();

		if (!fileSystem::exists(rootPath))
		{
			std::string errorMessage = "[" + rootPath.string() + "] ��ġ�� ������Ʈ ������ �������� �ʽ��ϴ�.";

			throw ENGINE_UI_EXCEPTION(errorMessage.c_str());
		}

		// �ֻ��� �������� Tree�� ����
		return BuildFileItemTree(rootPath);
	}

	EngineUI::FileItemTree EngineUI::BuildFileItemTree(const std::filesystem::path& rootPath)
    {
		// �ش� ��ġ���� ���� ���� ������ ������
		FileItemTree itemTree;
		itemTree.name = rootPath.filename().string();
		itemTree.isFolder = fileSystem::is_directory(rootPath);

		// �ش� ������ ������ ���
		if (itemTree.isFolder)
		{
			// ��� �ڽĵ鵵 �ڽ��� ã�� �ڽ��� �ڽĵ��� Ʈ���� ����
			for (const auto& child : fileSystem::directory_iterator(rootPath))
			{
				std::shared_ptr<FileItemTree> newFileItem = std::make_shared<FileItemTree>(BuildFileItemTree(child.path()));

				itemTree.chlidren.push_back(newFileItem);
			}
		}

		return itemTree;
    }

	void EngineUI::RenderFolderView()
	{
		if (ImGui::Begin("Folder View"))
		{
			// �ֻ��� Ʈ��(��Ʈ)���� ������ ����
			RenderFolderViewLoop(folderTree);

			ImGui::End();
		}
	}

	void EngineUI::RenderFolderViewLoop(const std::shared_ptr<FileItemTree>& itemTree)
	{
		// ������ ���
		if (itemTree->isFolder)
		{
			// Ʈ�� ���� ǥ�� -> Ŭ���ϸ� �ڽ� ��� ��ĥ �� ����
			if (ImGui::TreeNode(itemTree->name.c_str()))
			{
				// �ڽĵ� ��� ������
				for (const auto& child : itemTree->chlidren)
				{
					if (child->isFolder)
						RenderFolderViewLoop(child);
				}

				for (const auto& child : itemTree->chlidren)
				{
					if (!child->isFolder)
						RenderFolderViewLoop(child);
				}

				ImGui::TreePop();
			}
		}

		else
		{
			// ������ ��� -> ȭ��ǥ(Ʈ�� ���) ���� Selectable�θ� ǥ��
			if (ImGui::Selectable(itemTree->name.c_str(), selectedName == itemTree->name))
				selectedName = itemTree->name;
		}
	}

	void EngineUI::RenderInspector()
	{
		if (ImGui::Begin("Inspector"))
		{
			if (!selectedName.empty())
				ImGui::Text("Selected Item : %s", selectedName.c_str());

			else
				ImGui::Text("No items selected.");

			ImGui::End();
		}
	}
}