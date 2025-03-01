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
		// 프로젝트 폴더(최상위 폴더)를 가져옴
		fileSystem::path rootPath = fileSystem::current_path();

		if (!fileSystem::exists(rootPath))
		{
			std::string errorMessage = "[" + rootPath.string() + "] 위치에 프로젝트 폴더가 존재하지 않습니다.";

			throw ENGINE_UI_EXCEPTION(errorMessage.c_str());
		}

		// 최상위 폴더부터 Tree를 만듬
		return BuildFileItemTree(rootPath);
	}

	EngineUI::FileItemTree EngineUI::BuildFileItemTree(const std::filesystem::path& rootPath)
    {
		// 해당 위치에서 현재 파일 정보를 가져옴
		FileItemTree itemTree;
		itemTree.name = rootPath.filename().string();
		itemTree.isFolder = fileSystem::is_directory(rootPath);

		// 해당 파일이 폴더인 경우
		if (itemTree.isFolder)
		{
			// 모든 자식들도 자식을 찾고 자신의 자식들을 트리에 넣음
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
			// 최상위 트리(루트)부터 렌더링 시작
			RenderFolderViewLoop(folderTree);

			ImGui::End();
		}
	}

	void EngineUI::RenderFolderViewLoop(const std::shared_ptr<FileItemTree>& itemTree)
	{
		// 폴더인 경우
		if (itemTree->isFolder)
		{
			// 트리 노드로 표시 -> 클릭하면 자식 노드 펼칠 수 있음
			if (ImGui::TreeNode(itemTree->name.c_str()))
			{
				// 자식들 재귀 렌더링
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
			// 파일인 경우 -> 화살표(트리 노드) 없이 Selectable로만 표시
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