#pragma once

#include "Core/Exception/BaseException.h"
#include "Core/Exception/WindowException.h"

#include <string>
#include <optional>
#include <DirectXTex.h>

namespace GraphicResource
{
	class Image
	{
	public:
		// =================================
		//	Inner Class
		// =================================

		class Color
		{
		public:
			constexpr Color() noexcept : colorHex() {}
			constexpr Color(const Color& color) noexcept : colorHex(color.colorHex) {}
			constexpr Color(UINT colorHex) noexcept : colorHex(colorHex) {}

			constexpr Color(UCHAR x, UCHAR r, UCHAR g, UCHAR b) noexcept
				: colorHex((x << 24u) | (r << 16u) | (g << 8u) | b) {
			}
			constexpr Color(UCHAR r, UCHAR g, UCHAR b) noexcept
				: colorHex((255u << 24u) | (r << 16u) | (g << 8u) | b) {
			}
			constexpr Color(Color color, UCHAR x) noexcept
				: colorHex((x << 24u) | color.colorHex) {
			}

			Color& operator= (Color color) noexcept
			{
				colorHex = color.colorHex;
				return *this;
			}

			// Get Method
			constexpr UCHAR GetX() const noexcept { return colorHex >> 24u; }
			constexpr UCHAR GetA() const noexcept { return GetX(); }
			constexpr UCHAR GetR() const noexcept { return (colorHex >> 16u) & 0xFFu; }
			constexpr UCHAR GetG() const noexcept { return (colorHex >> 8u) & 0xFFu; }
			constexpr UCHAR GetB() const noexcept { return colorHex & 0xFFu; }

			// Set Method
			void SetX(UCHAR x) noexcept { colorHex = (colorHex & 0xFFFFFFu) | (x << 24u); }
			void SetA(UCHAR a) noexcept { SetX(a); }
			void SetR(UCHAR r) noexcept { colorHex = (colorHex & 0xFF00FFFFu) | (r << 16u); }
			void SetG(UCHAR g) noexcept { colorHex = (colorHex & 0xFFFF00FFu) | (g << 8u); }
			void SetB(UCHAR b) noexcept { colorHex = (colorHex & 0xFFFFFF00u) | b; }

			static DirectX::XMVECTOR ConvertVector(Image::Color color);

			UINT colorHex;
		};

		class Exception : public BaseException
		{
		public:
			Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr = {}) noexcept;
			Exception(int line, const char* file, std::string fileName, std::string note, std::optional<HRESULT> hr = {}) noexcept;

			const char* what() const noexcept override;
			const char* GetType() const noexcept override;
			const std::string& GetNote() const noexcept;

		private:
			std::optional<HRESULT> hr;
			std::string note;
		};

		// =================================
		//	Inner Method
		// =================================

		Image(UINT width, UINT height);
		Image(Image&) = delete;
		Image(Image&& source) noexcept = default;

		Image& operator=(Image&& donor) noexcept = default;
		Image& operator=(const Image&) = delete;
		~Image() = default;

		void Clear(Color fillColor) noexcept;

		void SetColorPixel(UINT x, UINT y, Color c) NOEXCEPTRELEASE;
		Color GetColorPixel(UINT x, UINT y) const NOEXCEPTRELEASE;

		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
		UINT GetBytePitch() const noexcept;

		Color* get() noexcept;
		const Color* get() const noexcept;
		const Color* GetConst() const noexcept;

		static Image FromFile(const std::string& name);

		void Save(const std::string& fileName) const;
		bool HasAlpha() const noexcept;

	private:
		Image(DirectX::ScratchImage scratch) noexcept;

		static HRESULT LoadTempDDSTexture(const std::wstring& filePath, DirectX::ScratchImage& image);

		static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		DirectX::ScratchImage scratch;
	};
}