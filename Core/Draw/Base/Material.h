#pragma once

#include "Core/Exception/BaseException.h"
#include <memory>

class Material
{
public:
	// =================================
	//	Inner Class
	// =================================

	class Color
	{
	public:
		constexpr Color() noexcept						: colorHex() { }
		constexpr Color(const Color& color) noexcept	: colorHex(color.colorHex) { }
		constexpr Color(UINT colorHex) noexcept			: colorHex(colorHex) { }

		constexpr Color(UCHAR x, UCHAR r, UCHAR g, UCHAR b) noexcept
			: colorHex((x << 24u) | (r << 16u) | (g << 8u) | b) { }
		constexpr Color(UCHAR r, UCHAR g, UCHAR b) noexcept
			: colorHex((r << 16u) | (g << 8u) | b) { }
		constexpr Color(Color color, UCHAR x) noexcept
			: colorHex((x << 24u) | color.colorHex) { }

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

		UINT colorHex;
	};

	class Exception : public BaseException
	{
	public:
		Exception(int line, const char* file, std::string note) noexcept;

		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;

	private:
		std::string note;
	};

	// =================================
	//	Inner Method
	// =================================

	Material(UINT width, UINT height) noexcept;
	Material(Material&& source) noexcept;
	Material& operator=(Material&& donor) noexcept;

	Material(Material&) = delete;
	Material& operator=(const Material&) = delete;
	~Material();

	void Clear(Color fillColor) noexcept;

	void SetColorPixel(UINT x, UINT y, Color c) noexcept(!_DEBUG);
	Color GetColorPixel(UINT x, UINT y) const noexcept(!_DEBUG);

	UINT GetWidth() const noexcept;
	UINT GetHeigth() const noexcept;

	Color* get() noexcept;
	const Color* get() const noexcept;
	const Color* getConst() const noexcept;

public:
	static Material FromFile(const std::string& name);

	void Save(const std::string& fileName) const;
	void Copy(const Material& src) noexcept(!_DEBUG);

private:
	Material(UINT width, UINT height, std::unique_ptr<Color[]> color) noexcept;

	std::unique_ptr<Color[]> color;
	UINT width;
	UINT height;
};