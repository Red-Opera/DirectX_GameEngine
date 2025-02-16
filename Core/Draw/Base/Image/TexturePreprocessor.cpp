#include "stdafx.h"
#include "TexturePreprocessor.h"

#include "Core/Exception/ModelException.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <sstream>

void TexturePreprocessor::FilpYAllNormalMapsInObject(const std::string& objectPath)
{
	const auto rootPath = std::filesystem::path(objectPath).parent_path().string() + "\\";

	Assimp::Importer importer;
	const auto scene = importer.ReadFile(objectPath.c_str(), 0u);

	if (scene == nullptr)
		throw ModelException(__LINE__, __FILE__, importer.GetErrorString());

	for (auto i = 0u; i < scene->mNumMaterials; i++)
	{
		auto& material = *scene->mMaterials[i];
		aiString textureFileName;

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto fullPath = rootPath + textureFileName.C_Str();
			FilpYNormalMap(fullPath, fullPath);
		}
	}
}

void TexturePreprocessor::FilpYNormalMap(const std::string& pathIn, const std::string& pathOut)
{
	using namespace DirectX;
	const auto flipper = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	const auto process = [flipper](XMVECTOR vector, int x, int y) -> XMVECTOR { return vector * flipper; };

	TransformFile(pathIn, pathOut, process);
}

void TexturePreprocessor::VaildateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax)
{
    // 입력된 경로의 노말 맵을 검증 시작
    OutputDebugStringA(("Validating normal map : [" + pathIn + "]\n").c_str());

    using namespace DirectX;
    auto sum = XMVectorZero();

    // 각 벡터를 처리하는 람다 함수 정의
    const auto process = [thresholdMin, thresholdMax, &sum](XMVECTOR vector, int x, int y) -> XMVECTOR
    {
        // 벡터의 길이를 계산
        const auto length = XMVectorGetX(XMVector3Length(vector));
        const float z = XMVectorGetZ(vector);

        // 벡터의 길이가 임계값 범위를 벗어나는지 확인
        if (length < thresholdMin || length > thresholdMax)
        {
            XMFLOAT3 vec;
            XMStoreFloat3(&vec, vector);

            // 잘못된 노말 벡터에 대한 디버그 메시지 출력
            std::ostringstream oss;
            oss << "Bad normal at (" << x << ", " << y << ") with length " << length << "\n";
            OutputDebugStringA(oss.str().c_str());
        }

        // 벡터의 합을 계산
        sum = XMVectorAdd(sum, vector);

        return vector;
    };

    // 파일에서 이미지를 로드
    auto image = GraphicResource::Image::FromFile(pathIn);
    TransformImage(image, process);           // 머티리얼에 대해 람다 함수를 적용

    {
        XMFLOAT2 sumVector;
        XMStoreFloat2(&sumVector, sum);

        // 노말 벡터의 합에 대한 디버그 메시지 출력
        std::ostringstream oss;
        oss << "Sum of normals: (" << sumVector.x << ", " << sumVector.y << ")\n";
        OutputDebugStringA(oss.str().c_str());
    }
}

void TexturePreprocessor::MakeStripes(const std::string& pathOut, int size, int stripeWidth)
{
    auto power = log2(size);
	assert("텍스처 크기가 2의 거듭제곱이 아닙니다!" && modf(power, &power) == 0.0);
	assert("스트라이프 너비가 텍스처 크기보다 큽니다!" && stripeWidth < size / 2);

	GraphicResource::Image material(size, size);

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
            GraphicResource::Image::Color color = { 0, 0, 0 };

			if ((x / stripeWidth) % 2 == 0)
				color = { 255, 255, 255 };

			material.SetColorPixel(x, y, color);
		}
	}

	material.Save(pathOut);
}