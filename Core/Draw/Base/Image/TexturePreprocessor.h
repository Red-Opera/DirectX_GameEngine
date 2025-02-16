#pragma once


#include "Image.h"

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
	static void MakeStripes(const std::string& pathOut, int size, int stripeWidth);

private:
	template<typename FUNCTION>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, FUNCTION&& func);

	template<typename FUNCTION>
	static void TransformImage(GraphicResource::Image& image, FUNCTION&& func);
};

template<typename FUNCTION>
inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, FUNCTION&& func)
{
	auto image = GraphicResource::Image::FromFile(pathIn);

	TransformImage(image, func);

	image.Save(pathOut);
}

template<typename FUNCTION>
inline void TexturePreprocessor::TransformImage(GraphicResource::Image& image, FUNCTION&& func)
{
	// ��Ƽ������ �ʺ�� ���̸� ������
	const auto width = image.GetWidth();
	const auto height = image.GetHeight();

	// �� �ȼ��� ���� ��ȯ �Լ��� ����
	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			// ���� �ȼ��� ������ ���ͷ� ��ȯ
			const auto noraml = GraphicResource::Image::Color::ConvertVector(image.GetColorPixel(x, y));

			// ��ȯ �Լ��� ������ �� �ٽ� �������� ��ȯ�Ͽ� �ȼ��� ����
			image.SetColorPixel(x, y, Vector::ConvertColor(func(noraml, x, y)));
		}
	}
}