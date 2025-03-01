#include "stdafx.h"
#include "EngineUI.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/Exception/ExceptionInfo.h"
#include "Core/Exception/EngineUIException.h"
#include "Core/DxGraphic.h"
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"

#include <d3d11.h>
#include <filesystem>
#include <vector>
#include <string>

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
			LoadIconTexture(graphic, "Images/Engine/FileIcon.png", IconType::file);
			LoadIconTexture(graphic, "Images/Engine/GoParentFolder.png", IconType::ParentFolder);
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
		return BuildFileItemTree(rootPath);
	}

	std::shared_ptr<EngineUI::FileItemTree> EngineUI::BuildFileItemTree(const std::filesystem::path& rootPath)
    {
		// �ش� ��ġ���� ���� ���� ������ ������
		auto itemTree = std::make_shared<FileItemTree>();
		itemTree->name = rootPath.filename().string();
		itemTree->isFolder = fileSystem::is_directory(rootPath);

		// �ش� ������ ������ ���
		if (itemTree->isFolder)
		{
			// ��� �ڽĵ鵵 �ڽ��� ã�� �ڽ��� �ڽĵ��� Ʈ���� ����
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
            // [0] ���� ��θ� ��� ���ʿ� ���
            std::shared_ptr<FileItemTree> currentFolder = itemTree ? itemTree : folderTree;
            std::string currentPath = GetRelativePath(currentFolder);

            ImGui::Text("Current Path : %s", currentPath.c_str());

            // [1] ���� ������ �̵� ��ư (��� ������, ���� ��Ȱ��ȭ ó��)
            {
                ImVec2 iconSize(32.0f, 32.0f);

                // ��ü ���� ������ ���� �̿��� ��ư�� x ��ġ ���
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

            // [2] ���� ���� Ʈ�� ���� (������ folderTree ���)
            if (!itemTree)
                itemTree = folderTree;

            // [3] ����/���� ��� ǥ�� (���� �켱 ��ġ)
            if (ImGui::BeginChild("View", ImVec2(0, 0), true))
            {
                const int columns = 4;
                ImGui::Columns(columns, nullptr, true);

                // ���� ���
                for (const auto& child : itemTree->chlidren)
                {
                    if (child->isFolder)
                    {
                        ImGui::PushID(child->name.c_str());

                        // ���� ������ �׸��� �� ���� Ŭ�� ó�� (������ ������ ����)
                        ImVec2 startPos = ImGui::GetCursorScreenPos();
                        ImGui::Image(reinterpret_cast<ImTextureID>(folderIconTexture.Get()), ImVec2(64, 64));

                        ImGui::SetCursorScreenPos(startPos);

                        if (ImGui::InvisibleButton("##folder_image_button", ImVec2(64, 64)))
                            selectedName = child->name;

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                            folderTree = child;

                        // ������ �Ʒ��� ���� �̸� ���
                        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + 64));
                        ImGui::TextWrapped("%s", child->name.c_str());

                        ImGui::PopID();

                        // �� �켱: �� ������ �� ������ ���� ���� �̵�
                        ImGui::NextColumn();
                    }
                }

                // ���� ���
                for (const auto& child : itemTree->chlidren)
                {
                    if (!child->isFolder)
                    {
                        ImGui::Image(reinterpret_cast<ImTextureID>(fileIconTexture.Get()), ImVec2(64, 64));

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
			hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), nullptr, fileIconTexture.GetAddressOf());
			break;

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

	void EngineUI::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{

	}
}