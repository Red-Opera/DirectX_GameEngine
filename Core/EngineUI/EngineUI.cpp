#include "stdafx.h"
#include "EngineUI.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/Exception/ExceptionInfo.h"
#include "Core/Exception/EngineUIException.h"
#include "Core/DxGraphic.h"
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <string>

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXTex.h>

namespace Engine
{
	namespace fileSystem = std::filesystem;

	std::unique_ptr<EngineUI> EngineUI::instance = nullptr;

	EngineUI::EngineUI(DxGraphic& graphic) : selectedName("")
	{
		if (!folderTree)
		{
			folderTree = CreateFileSystem();

			LoadIconTexture(graphic, "Images/Engine/FolderIcon.png", IconType::folder);
			LoadIconTexture(graphic, "Images/Engine/GoParentFolder.png", IconType::ParentFolder);

            // 파일 폴더 이미지 경로
            std::string folderPath = "Images/Engine/FileIcon";

            try 
            {
                if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
                {
                    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
                    {
                        if (std::filesystem::is_regular_file(entry.path()))
                        {
                            std::string path = entry.path().string();
                            path = path.replace(path.find("\\"), 1, "/");

                            // 이미지 로드
                            LoadIconTexture(graphic, path, IconType::file);
                        }
                    }
                }

                else
                    throw ENGINE_UI_EXCEPTION("해당 경로는 폴더가 아님.");
            }

            catch (const std::filesystem::filesystem_error& e)
            {
                throw ENGINE_UI_EXCEPTION(e.what());
            }
		}
	}

	std::shared_ptr<EngineUI::FileItemTree> EngineUI::CreateFileSystem()
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

	std::shared_ptr<EngineUI::FileItemTree> EngineUI::BuildFileItemTree(const std::filesystem::path& rootPath)
    {
		// 해당 위치에서 현재 파일 정보를 가져옴
		auto itemTree = std::make_shared<FileItemTree>();
		itemTree->name = rootPath.filename().string();
		itemTree->isFolder = fileSystem::is_directory(rootPath);

		// 해당 파일이 폴더인 경우
		if (itemTree->isFolder)
		{
			// 모든 자식들도 자식을 찾고 자신의 자식들을 트리에 넣음
			for (const auto& child : fileSystem::directory_iterator(rootPath))
			{
				std::shared_ptr<FileItemTree> newFileItem = BuildFileItemTree(child.path());

                newFileItem->parent = itemTree;
				itemTree->chlidren.push_back(newFileItem);
			}
		}

		return itemTree;
    }

    void EngineUI::RenderFolderView(std::shared_ptr<FileItemTree> itemTree)
    {
        if (ImGui::Begin("Folder View"))
        {
            // [0] 현재 경로를 상단 왼쪽에 출력
            std::shared_ptr<FileItemTree> currentFolder = itemTree ? itemTree : folderTree;
            std::string currentPath = GetRelativePath(currentFolder);

            ImGui::Text("Current Path : %s", currentPath.c_str());

            // [1] 상위 폴더로 이동 버튼 (상단 오른쪽, 수동 비활성화 처리)
            {
                ImVec2 iconSize(32.0f, 32.0f);

                // 전체 내용 영역의 폭을 이용해 버튼의 x 위치 계산
                float totalWidth = ImGui::GetWindowContentRegionWidth();
                float buttonPosX = totalWidth - iconSize.x;

                if (buttonPosX < 0.0f)
                    buttonPosX = 0.0f;

                ImGui::SameLine(buttonPosX);

                bool canGoParent = (folderTree && folderTree->parent != nullptr);

                if (!canGoParent)
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }

                if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(goParentFolderTexture.Get()), iconSize))
                {
                    if (canGoParent)
                        folderTree = folderTree->parent;
                }

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Go to parent folder");

                if (!canGoParent)
                {
                    ImGui::PopStyleVar();
                    ImGui::PopItemFlag();
                }
            }

            // [2] 현재 폴더 트리 지정 (없으면 folderTree 사용)
            if (!itemTree)
                itemTree = folderTree;

            // [3] 폴더/파일 목록 표시 (가로 우선 배치)
            if (ImGui::BeginChild("View", ImVec2(0, 0), true))
            {
                const int columns = 4;
                ImGui::Columns(columns, nullptr, false);

                // 폴더 출력
                for (const auto& child : itemTree->chlidren)
                {
                    if (child->isFolder)
                    {
                        ImGui::PushID(child->name.c_str());

                        // 폴더 아이콘 그리기 및 더블 클릭 처리 (아이콘 영역만 감지)
                        ImVec2 startPos = ImGui::GetCursorScreenPos();
                        ImGui::Image(reinterpret_cast<ImTextureID>(folderIconTexture.Get()), ImVec2(64, 64));

                        ImGui::SetCursorScreenPos(startPos);

                        if (ImGui::InvisibleButton("##folder_image_button", ImVec2(64, 64)))
                            selectedName = child->name;

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                            folderTree = child;

                        // 아이콘 아래에 폴더 이름 출력
                        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + 64));
                        ImGui::TextWrapped("%s", child->name.c_str());

                        ImGui::PopID();

                        // 행 우선: 각 아이템 후 무조건 다음 열로 이동
                        ImGui::NextColumn();
                    }
                }

                // 파일 출력
                for (const auto& child : itemTree->chlidren)
                {
                    // 파일 확장자만 추출 (마지막 . 이후 문자열, . 제외)
                    std::string fileName = child->name;
                    size_t dotPos = fileName.rfind('.');  // 마지막 .의 위치 찾기

                    std::string fileExtension;

                    if (dotPos != std::string::npos)  // .이 있는 경우
                        fileExtension = fileName.substr(dotPos + 1);  // . 이후 부분 추출 (점 제외)

                    if (!child->isFolder)
                    {
                        ImGui::Image(reinterpret_cast<ImTextureID>(GetFileTextureResourceView(fileExtension)), ImVec2(64, 64));

                        if (ImGui::Selectable(child->name.c_str(), selectedName == child->name))
                            selectedName = child->name;

                        ImGui::NextColumn();
                    }
                }

                ImGui::Columns(1);
            }

            ImGui::EndChild();
        }

        ImGui::End();
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

    ID3D11ShaderResourceView* EngineUI::GetFileTextureResourceView(std::string fileName)
    {
        // 대문자를 모두 소문자로
        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);

        if (fileName.compare("cso") == 0 || fileName.compare("fx") == 0 || fileName.compare("hlsli") == 0)
            fileName = "hlsl";

        else if (fileName.compare("jpg") == 0 || fileName.compare("bmp") == 0 || fileName.compare("jpge") == 0)
            fileName = "png";

        else if (fileName.compare("fbx") == 0 || fileName.compare("stl") == 0 || fileName.compare("gltf") == 0 || fileName.compare("mtl") == 0)
            fileName = "obj";

        // 해당 확장명으로 이미지 파일이 없는 경우
        if (fileIconTextures.find(fileName) == fileIconTextures.end())
            return fileIconTextures["other"].Get();

        return fileIconTextures[fileName].Get();
    }

    void EngineUI::LoadIconTexture(DxGraphic& graphic, std::string fileName, IconType iconType)
	{
		using namespace Graphic;
		using namespace DirectX;

		CREATEINFOMANAGER(graphic);

		ScratchImage image;
		hr = LoadFromWICFile(std::wstring(fileName.begin(), fileName.end()).c_str(), WIC_FLAGS_NONE, nullptr, image); // PNG 파일 로드 (WIC 사용)

		GRAPHIC_THROW_INFO(hr);
		
		const TexMetadata& metadata = image.GetMetadata();

		Microsoft::WRL::ComPtr<ID3D11Resource> texture;
		hr = CreateTexture(GetDevice(graphic), image.GetImages(), image.GetImageCount(), metadata, texture.GetAddressOf());

		GRAPHIC_THROW_INFO(hr);

		switch (iconType)
		{
		case Engine::EngineUI::IconType::folder:
			hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), nullptr, folderIconTexture.GetAddressOf());
			break;

        case Engine::EngineUI::IconType::file:
        {
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newIconTexture;

            hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), nullptr, newIconTexture.GetAddressOf());
            GRAPHIC_THROW_INFO(hr);

            // 파일명 추출 (확장자 없이)
            std::string fileNameString = std::filesystem::path(fileName).stem().string();

            if (fileNameString.compare("FileIcon") == 0)
                fileNameString = "other";

            // fileIconTextures에 저장
            fileIconTextures[fileNameString] = newIconTexture;

            break;
        }


		case Engine::EngineUI::IconType::ParentFolder:
			hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), nullptr, goParentFolderTexture.GetAddressOf());
			break;

		default:
			ENGINE_UI_EXCEPTION("해당 Icon 파일 타입은 존재하지 않음");
			hr = S_FALSE;
			break;
		}

		GRAPHIC_THROW_INFO(hr);
	}

    std::string EngineUI::GetRelativePath(const std::shared_ptr<FileItemTree>& tree)
    {
        if (!tree)
            return "";

        std::string path = tree->name;
        auto parent = tree->parent;

        while (parent)
        {
            path = parent->name + "/" + path;
            parent = parent->parent;
        }

        return path;
    }

	void EngineUI::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{

	}
}