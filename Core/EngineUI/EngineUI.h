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

		FileItemTree CreateFileSystem();										// 프로젝트 구조 반영한 파일 시스템 생성 시작 메소드
		FileItemTree BuildFileItemTree(const std::filesystem::path& rootPath);	// 프로젝트 구조 반영하여 파일 트리를 만드는 메소드

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