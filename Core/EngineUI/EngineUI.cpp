#include "stdafx.h"
#include "EngineUI.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/Exception/ExceptionInfo.h"
#include "Core/Exception/EngineUIException.h"
#include "Core/DxGraphic.h"
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

	std::unique_ptr<EngineUI> EngineUI::instance = nullptr;

	EngineUI::EngineUI(DxGraphic& graphic) : selectedName("")
	{
		if (!folderTree)
		{
			folderTree = CreateFileSystem();

			LoadIconTexture(graphic, "Images/Engine/FolderIcon.png", IconType::folder);
			LoadIconTexture(graphic, "Images/Engine/GoParentFolder.png", IconType::ParentFolder);

            // ���� ���� �̹��� ���
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

                            // �̹��� �ε�
                            LoadIconTexture(graphic, path, IconType::file);
                        }
                    }
                }

                else
                    throw ENGINE_UI_EXCEPTION("�ش� ��δ� ������ �ƴ�.");
            }

            catch (const std::filesystem::filesystem_error& e)
            {
                throw ENGINE_UI_EXCEPTION(e.what());
            }
		}
	}

	std::shared_ptr<EngineUI::FileItemTree> EngineUI::CreateFileSystem()
	{
		// ������Ʈ ����(�ֻ��� ����)�� ������
		fileSystem::path rootPath = fileSystem::current_path();

		if (!fileSystem::exists(rootPath))
		{
			std::string errorMessage = "[" + rootPath.string() + "] ��ġ�� ������Ʈ ������ �������� �ʽ��ϴ�.";

			throw ENGINE_UI_EXCEPTION(errorMessage.c_str());
		}

		// �ֻ��� �������� Tree�� ����
        std::filesystem::directory_entry entry(rootPath);
		return BuildFileItemTree(entry);
	}

	std::shared_ptr<EngineUI::FileItemTree> EngineUI::BuildFileItemTree(const std::filesystem::directory_entry& rootPath)
    {
		// �ش� ��ġ���� ���� ���� ������ ������
		auto itemTree = std::make_shared<FileItemTree>();
		itemTree->name = rootPath.path().filename().string();
		itemTree->isFolder = rootPath.is_directory();

		// �ش� ������ ������ ���
		if (itemTree->isFolder)
		{
			// ��� �ڽĵ鵵 �ڽ��� ã�� �ڽ��� �ڽĵ��� Ʈ���� ����
			for (const auto& child : fileSystem::directory_iterator(rootPath.path()))
			{
				std::shared_ptr<FileItemTree> newFileItem = BuildFileItemTree(child);

                newFileItem->parent = itemTree;
				itemTree->chlidren.push_back(newFileItem);
			}
		}

		return itemTree;
    }

    void EngineUI::RenderFolderView(std::shared_ptr<FileItemTree> itemTree)
    {
        if (!ImGui::Begin("Folder View"))
        {
            ImGui::End();
            return;
        }

        // [0] ���� ��� ���
        RenderCurrentPath(itemTree);

        // [1] ���� ���� �̵� ��ư ���
        RenderParentFolderButton();

        // itemTree�� nullptr�̸� folderTree ���
        if (!itemTree)
            itemTree = folderTree;

        // [2] ������ ���� ��� ���
        RenderFolderAndFileItems(itemTree);

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
        // �빮�ڸ� ��� �ҹ��ڷ�
        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);

        if (fileName.compare("cso") == 0 || fileName.compare("fx") == 0 || fileName.compare("hlsli") == 0)
            fileName = "hlsl";

        else if (fileName.compare("jpg") == 0 || fileName.compare("bmp") == 0 || fileName.compare("jpge") == 0)
            fileName = "png";

        else if (fileName.compare("fbx") == 0 || fileName.compare("stl") == 0 || fileName.compare("gltf") == 0 || fileName.compare("mtl") == 0)
            fileName = "obj";

        // �ش� Ȯ������� �̹��� ������ ���� ���
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
		hr = LoadFromWICFile(std::wstring(fileName.begin(), fileName.end()).c_str(), WIC_FLAGS_NONE, nullptr, image); // PNG ���� �ε� (WIC ���)

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

            // ���ϸ� ���� (Ȯ���� ����)
            std::string fileNameString = std::filesystem::path(fileName).stem().string();

            if (fileNameString.compare("FileIcon") == 0)
                fileNameString = "other";

            // fileIconTextures�� ����
            fileIconTextures[fileNameString] = newIconTexture;

            break;
        }


		case Engine::EngineUI::IconType::ParentFolder:
			hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), nullptr, goParentFolderTexture.GetAddressOf());
			break;

		default:
			ENGINE_UI_EXCEPTION("�ش� Icon ���� Ÿ���� �������� ����");
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

    std::string EngineUI::GetAbsolutePath(const std::shared_ptr<FileItemTree>& tree)
    {
		if (!tree)
			return "";

        std::string absolutePath = fileSystem::current_path().string() + "\\" + GetRelativePath(tree);

		absolutePath = absolutePath.replace(absolutePath.find("/"), 1, "\\");

		return absolutePath;
    }

    void EngineUI::RenderCurrentPath(std::shared_ptr<FileItemTree> itemTree)
    {
        // itemTree�� nullptr�̸� �⺻ folderTree ���
        std::shared_ptr<FileItemTree> currentFolder = itemTree ? itemTree : folderTree;
        std::string currentPath = GetRelativePath(currentFolder);

        ImGui::Text("Current Path : %s", currentPath.c_str());
    }

    void EngineUI::RenderParentFolderButton()
    {
        ImVec2 iconSize(32.0f, 32.0f);

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

    void EngineUI::RenderFolderAndFileItems(std::shared_ptr<FileItemTree> itemTree)
    {
        ImGui::BeginChild("View", ImVec2(0, 0), true);  // if�� ����
        {
            const int columns = 4;
            ImGui::Columns(columns, nullptr, false);

            // ���� ������ ���
            for (const auto& child : itemTree->chlidren)
            {
                if (child->isFolder)
                {
                    RenderFolderItem(child);
                    ImGui::NextColumn();
                }
            }

            // ���� ������ ���
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

    void EngineUI::RenderFolderItem(const std::shared_ptr<FileItemTree>& child)
    {
        ImGui::PushID(child->name.c_str());

        // ���� ������ �� ����Ŭ������ ���� �̵� ó��
        ImVec2 startPos = ImGui::GetCursorScreenPos();

        ImGui::Image(reinterpret_cast<ImTextureID>(folderIconTexture.Get()), ImVec2(64, 64));
        ImGui::SetCursorScreenPos(startPos);

        if (ImGui::InvisibleButton("##folder_image_button", ImVec2(64, 64)))
            selectedName = child->name;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            folderTree = child;

        // ���� �̸� ���
        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + 64));
        ImGui::TextWrapped("%s", child->name.c_str());
        ImGui::PopID();
    }

    void EngineUI::RenderFileItem(const std::shared_ptr<FileItemTree>& child)
    {
        // ���� Ȯ���� ����
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

    void EngineUI::OpenFile(const std::shared_ptr<FileItemTree>& child)
    {
#ifdef _WIN32
        std::filesystem::path relativePath = GetRelativePath(child);

        // ���� ��η� ��ȯ
        std::string absolutePath = StringConverter::GetAbsolutePath(relativePath);

        ShellExecuteA(nullptr, "open", absolutePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
		// ���������� ���� ����
		std::string command = "xdg-open " + path;
		system(command.c_str());
#endif
    }

	void EngineUI::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{

	}
}