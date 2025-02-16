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
	// 머티리얼의 너비와 높이를 가져옴
	const auto width = image.GetWidth();
	const auto height = image.GetHeight();

	// 각 픽셀에 대해 변환 함수를 적용
	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			// 현재 픽셀의 색상을 벡터로 변환
			const auto noraml = GraphicResource::Image::Color::ConvertVector(image.GetColorPixel(x, y));

			// 변환 함수를 적용한 후 다시 색상으로 변환하여 픽셀에 설정
			image.SetColorPixel(x, y, Vector::ConvertColor(func(noraml, x, y)));
		}
	}
}