#include "stdafx.h"
#include "FolderViewInspector.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/Exception/ExceptionInfo.h"
#include "Core/Exception/EngineUIException.h"
#include "Core/DxGraphic.h"
#include "Core/Window.h"
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"

#include "Utility/StringConverter.h"

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

    std::unique_ptr<FolderViewInspector> FolderViewInspector::instance = nullptr;

    FolderViewInspector::FolderViewInspector() : selectedName("")
    {
        if (!folderTree)
        {
            folderTree = CreateFileSystem();

            LoadIconTexture("Images/Engine/FolderIcon.png", IconType::folder);
            LoadIconTexture("Images/Engine/GoParentFolder.png", IconType::ParentFolder);

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
                            LoadIconTexture(path, IconType::file);
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

    std::shared_ptr<FolderViewInspector::FileItemTree> FolderViewInspector::CreateFileSystem()
    {
        // 프로젝트 폴더(최상위 폴더)를 가져옴
        fileSystem::path rootPath = fileSystem::current_path();

        if (!fileSystem::exists(rootPath))
        {
            std::string errorMessage = "[" + rootPath.string() + "] 위치에 프로젝트 폴더가 존재하지 않습니다.";

            throw ENGINE_UI_EXCEPTION(errorMessage.c_str());
        }

        // 최상위 폴더부터 Tree를 만듬
        std::filesystem::directory_entry entry(rootPath);
        return BuildFileItemTree(entry);
    }

    std::shared_ptr<FolderViewInspector::FileItemTree> FolderViewInspector::BuildFileItemTree(const std::filesystem::directory_entry& rootPath)
    {
        // 해당 위치에서 현재 파일 정보를 가져옴
        auto itemTree = std::make_shared<FileItemTree>();
        itemTree->name = rootPath.path().filename().string();
        itemTree->isFolder = rootPath.is_directory();

        // 해당 파일이 폴더인 경우
        if (itemTree->isFolder)
        {
            // 모든 자식들도 자식을 찾고 자신의 자식들을 트리에 넣음
            for (const auto& child : fileSystem::directory_iterator(rootPath.path()))
            {
                std::shared_ptr<FileItemTree> newFileItem = BuildFileItemTree(child);

                newFileItem->parent = itemTree;
                itemTree->chlidren.push_back(newFileItem);
            }
        }

        return itemTree;
    }

    void FolderViewInspector::RenderFolderView(std::shared_ptr<FileItemTree> itemTree)
    {
        if (!ImGui::Begin("Folder View"))
        {
            ImGui::End();
            return;
        }

        // [0] 현재 경로 출력
        RenderCurrentPath(itemTree);

        // [1] 상위 폴더 이동 버튼 출력
        RenderParentFolderButton();

        // itemTree가 nullptr이면 folderTree 사용
        if (!itemTree)
            itemTree = folderTree;

        // [2] 폴더와 파일 목록 출력
        RenderFolderAndFileItems(itemTree);

        ImGui::End();
    }

    void FolderViewInspector::RenderInspector()
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

    ID3D11ShaderResourceView* FolderViewInspector::GetFileTextureResourceView(std::string fileName)
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

    void FolderViewInspector::LoadIconTexture(std::string fileName, IconType iconType)
    {
        using namespace Graphic;
        using namespace DirectX;

        CREATEINFOMANAGER(Window::GetDxGraphic());

        ScratchImage image;
        hr = LoadFromWICFile(std::wstring(fileName.begin(), fileName.end()).c_str(), WIC_FLAGS_NONE, nullptr, image); // PNG 파일 로드 (WIC 사용)

        GRAPHIC_THROW_INFO(hr);

        const TexMetadata& metadata = image.GetMetadata();

        Microsoft::WRL::ComPtr<ID3D11Resource> texture;
        hr = CreateTexture(GetDevice(Window::GetDxGraphic()), image.GetImages(), image.GetImageCount(), metadata, texture.GetAddressOf());

        GRAPHIC_THROW_INFO(hr);

        switch (iconType)
        {
        case Engine::FolderViewInspector::IconType::folder:
            hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), nullptr, folderIconTexture.GetAddressOf());
            break;

        case Engine::FolderViewInspector::IconType::file:
        {
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newIconTexture;

            hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), nullptr, newIconTexture.GetAddressOf());
            GRAPHIC_THROW_INFO(hr);

            // 파일명 추출 (확장자 없이)
            std::string fileNameString = std::filesystem::path(fileName).stem().string();

            if (fileNameString.compare("FileIcon") == 0)
                fileNameString = "other";

            // fileIconTextures에 저장
            fileIconTextures[fileNameString] = newIconTexture;

            break;
        }

        case Engine::FolderViewInspector::IconType::ParentFolder:
            hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), nullptr, goParentFolderTexture.GetAddressOf());
            break;

        default:
            ENGINE_UI_EXCEPTION("해당 Icon 파일 타입은 존재하지 않음");
            hr = S_FALSE;
            break;
        }

        GRAPHIC_THROW_INFO(hr);
    }

    std::string FolderViewInspector::GetRelativePath(const std::shared_ptr<FileItemTree>& tree)
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

    std::string FolderViewInspector::GetAbsolutePath(const std::shared_ptr<FileItemTree>& tree)
    {
        if (!tree)
            return "";

        std::string absolutePath = fileSystem::current_path().string() + "\\" + GetRelativePath(tree);

        absolutePath = absolutePath.replace(absolutePath.find("/"), 1, "\\");

        return absolutePath;
    }

    void FolderViewInspector::RenderCurrentPath(std::shared_ptr<FileItemTree> itemTree)
    {
        // itemTree가 nullptr이면 기본 folderTree 사용
        std::shared_ptr<FileItemTree> currentFolder = itemTree ? itemTree : folderTree;
        std::string currentPath = GetRelativePath(currentFolder);

        ImGui::Text("Current Path : %s", currentPath.c_str());
    }

    void FolderViewInspector::RenderParentFolderButton()
    {
        ImVec2 iconSize(32.0f, 32.0f);

        float totalWidth = ImGui::GetContentRegionAvail().x;
        float buttonPosX = totalWidth - iconSize.x;

        if (buttonPosX < 0.0f)
            buttonPosX = 0.0f;

        ImGui::SameLine(buttonPosX);

        bool canGoParent = (folderTree && folderTree->parent != nullptr);

        if (!canGoParent)
            ImGui::BeginDisabled();

        if (ImGui::ImageButton("goParentFolderButton", reinterpret_cast<ImTextureID>(goParentFolderTexture.Get()), iconSize))
        {
            if (canGoParent)
                folderTree = folderTree->parent;
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Go to parent folder");

        if (!canGoParent)
            ImGui::EndDisabled();
    }

    void FolderViewInspector::RenderFolderAndFileItems(std::shared_ptr<FileItemTree> itemTree)
    {
        ImGui::BeginChild("View", ImVec2(0, 0), true);  // if문 제거
        {
            const int columns = 4;
            ImGui::Columns(columns, nullptr, false);

            // 폴더 아이템 출력
            for (const auto& child : itemTree->chlidren)
            {
                if (child->isFolder)
                {
                    RenderFolderItem(child);
                    ImGui::NextColumn();
                }
            }

            // 파일 아이템 출력
            for (const auto& child : itemTree->chlidren)
            {
                if (!child->isFolder)
                {
                    RenderFileItem(child);
                    ImGui::NextColumn();
                }
            }

            ImGui::Columns(1);
        }

        ImGui::EndChild();
    }

    void FolderViewInspector::RenderFolderItem(const std::shared_ptr<FileItemTree>& child)
    {
        ImGui::PushID(child->name.c_str());

        // 폴더 아이콘 및 더블클릭으로 폴더 이동 처리
        ImVec2 startPos = ImGui::GetCursorScreenPos();

        ImGui::Image(reinterpret_cast<ImTextureID>(folderIconTexture.Get()), ImVec2(64, 64));
        ImGui::SetCursorScreenPos(startPos);

        if (ImGui::InvisibleButton("##folder_image_button", ImVec2(64, 64)))
            selectedName = child->name;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            folderTree = child;

        // 폴더 이름 출력
        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + 64));
        ImGui::TextWrapped("%s", child->name.c_str());
        ImGui::PopID();
    }

    void FolderViewInspector::RenderFileItem(const std::shared_ptr<FileItemTree>& child)
    {
        // 파일 확장자 추출
        std::string fileName = child->name;
        size_t dotPos = fileName.rfind('.');

        std::string fileExtension;

        if (dotPos != std::string::npos)
            fileExtension = fileName.substr(dotPos + 1);

        ImGui::Image(reinterpret_cast<ImTextureID>(GetFileTextureResourceView(fileExtension)), ImVec2(64, 64));

        if (ImGui::Selectable(child->name.c_str(), selectedName == child->name))
            selectedName = child->name;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            OpenFile(child);
    }

    void FolderViewInspector::OpenFile(const std::shared_ptr<FileItemTree>& child)
    {
#ifdef _WIN32
        std::filesystem::path relativePath = GetRelativePath(child);

        // 절대 경로로 변환
        std::string absolutePath = StringConverter::GetAbsolutePath(relativePath);

        ShellExecuteA(nullptr, "open", absolutePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
        // 리눅스에서 파일 열기
        std::string command = "xdg-open " + path;
        system(command.c_str());
#endif
    }

    void FolderViewInspector::SetRenderPipeline() NOEXCEPTRELEASE
    {

    }
}
