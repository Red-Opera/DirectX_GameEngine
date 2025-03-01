#pragma once

#include "Core/Exception/WindowException.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Engine
{
	class EngineUI
	{
		struct FileItemTree;

	public:
		static std::unique_ptr<EngineUI, void(*)(EngineUI*)> instance;

		EngineUI();
		EngineUI(const EngineUI&) = delete;
		EngineUI(EngineUI&&) = delete;
		EngineUI& operator=(const EngineUI&) = delete;
		EngineUI& operator=(EngineUI&&) = delete;

		FileItemTree CreateFileSystem();										// ������Ʈ ���� �ݿ��� ���� �ý��� ���� ���� �޼ҵ�
		FileItemTree BuildFileItemTree(const std::filesystem::path& rootPath);	// ������Ʈ ���� �ݿ��Ͽ� ���� Ʈ���� ����� �޼ҵ�

		void RenderFolderView();
		void RenderFolderViewLoop(const std::shared_ptr<FileItemTree>& itemTree);
		void RenderInspector();

		~EngineUI() = delete;

	private:
		struct FileItemTree
		{
			std::string name;
			std::vector<std::shared_ptr<FileItemTree>> chlidren;

			bool isFolder;
		};

		std::string selectedName;
		std::shared_ptr<FileItemTree> folderTree;
	};
}