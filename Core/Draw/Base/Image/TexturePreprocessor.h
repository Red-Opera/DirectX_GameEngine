#pragma once


#include "../Material.h"

#include "Utility/MathInfo.h"
#include "Utility/Vector.h"

#include <string>
#include <DirectXMath.h>

class TexturePreprocessor
{
public:
	static void FilpYAllNormalMapsInObject(const std::string& objectPath);
	static void FilpYNormalMap(const std::string& pathIn, const std::string& pathOut);

	static void VaildateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax);

private:
	template<typename FUNCTION>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, FUNCTION&& func);

	template<typename FUNCTION>
	static void TransformMaterial(Material& material, FUNCTION&& func);
};

template<typename FUNCTION>
inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, FUNCTION&& func)
{
	auto material = Material::FromFile(pathIn);

	TransformMaterial(material, func);

	material.Save(pathOut);
}

template<typename FUNCTION>
inline void TexturePreprocessor::TransformMaterial(Material& material, FUNCTION&& func)
{
	// ��Ƽ������ �ʺ�� ���̸� ������
	const auto width = material.GetWidth();
	const auto height = material.GetHeight();

	// �� �ȼ��� ���� ��ȯ �Լ��� ����
	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			// ���� �ȼ��� ������ ���ͷ� ��ȯ
			const auto noraml = Material::Color::ConvertVector(material.GetColorPixel(x, y));

			// ��ȯ �Լ��� ������ �� �ٽ� �������� ��ȯ�Ͽ� �ȼ��� ����
			material.SetColorPixel(x, y, Vector::ConvertColor(func(noraml, x, y)));
		}
	}
}