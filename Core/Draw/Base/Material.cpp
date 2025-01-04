#include "stdafx.h"
#include "Material.h"

namespace Gdiplus
{
    using std::min;
    using std::max;
}

#include <windows.h>
#include <algorithm>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace std;

Material::Exception::Exception(int line, const char* file, string note) noexcept
    : BaseException(line, file), note(move(note))
{

}

const char* Material::Exception::what() const noexcept
{
    ostringstream out;

    out << BaseException::what() << endl << "[Note]" << GetNote();
    exceptionText = out.str();

    return exceptionText.c_str();
}

const char* Material::Exception::GetType() const noexcept
{
    return "그래픽 생성 예외";
}

const string& Material::Exception::GetNote() const noexcept
{
    return note;
}

Material::Material(UINT width, UINT height) noexcept : width(width), height(height), color(make_unique<Color[]>(width * height))
{

}

Material::Material(Material&& source) noexcept : color(std::move(source.color)), width(source.width), height(source.height)
{

}

Material& Material::operator=(Material&& donor) noexcept
{
    width = donor.width;
    height = donor.height;
    color = move(donor.color);

    donor.color = nullptr;
    return *this;
}

Material::~Material()
{

}

void Material::Clear(Color fillColor) noexcept
{
    memset(color.get(), fillColor.colorHex, width * height * sizeof(Color));
}

void Material::SetColorPixel(UINT x, UINT y, Color c) NOEXCEPTRELEASE
{
    assert("지정할 픽셀의 위치를 벗어났습니다!" && x >= 0 && y >= 0 && x < width && y < height);

    color[y * width + x] = c;
}

Material::Color Material::GetColorPixel(UINT x, UINT y) const NOEXCEPTRELEASE
{
    assert("지정할 픽셀의 위치를 벗어났습니다!" && x >= 0 && y >= 0 && x < width && y < height);

    return color[y * width + x];
}

UINT Material::GetWidth() const noexcept
{
    return width;
}

UINT Material::GetHeight() const noexcept
{
    return height;
}

Material::Color* Material::get() noexcept
{
    return color.get();
}

const Material::Color* Material::get() const noexcept
{
    return color.get();
}

const Material::Color* Material::getConst() const noexcept
{
    return color.get();
}

Material Material::FromFile(const std::string& name)
{
    // 너비, 높이, 피치
    UINT width = 0, height = 0;
    std::unique_ptr<Color[]> color = nullptr;
    bool hasAlpha = false;

    wchar_t toWidename[512];
    mbstowcs_s(nullptr, toWidename, name.c_str(), _TRUNCATE);

    Gdiplus::Bitmap bitmap(toWidename);
    if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
    {
        std::stringstream out;
        out << name << "의 파일을 로드할 수 없음!";

        throw Exception(__LINE__, __FILE__, out.str());
    }

    height = bitmap.GetHeight();
    width = bitmap.GetWidth();
    color = make_unique<Color[]>(width * height);

    // 읽어온 파일 이미지의 픽셀을 색깔 버퍼에 저장함
    for (UINT y = 0; y < height; y++)
    {
        for (UINT x = 0; x < width; x++)
        {
            Gdiplus::Color c;
            bitmap.GetPixel(x, height - y - 1, &c);
            color[y * width + x] = c.GetValue();

            if (c.GetAlpha() != 255)
                hasAlpha = true;
        }
    }
    
    return Material(width, height, std::move(color), hasAlpha);
}

void Material::Save(const string& fileName) const
{
    auto GetEncodeID = [&fileName](const WCHAR* format, CLSID* clsid) -> void
    {
        // 암호화할 이미지의 개수와 바이트 크기
        UINT num = 0, size = 0;

        Gdiplus::ImageCodecInfo* imageCodexInfo = nullptr;
        Gdiplus::GetImageEncodersSize(&num, &size);

        // 암호화할 이미지가 존재하지 않는 경우
        if (size == 0)
        {
            std::stringstream out;
            out << fileName << "의 파일 이름을 암호화할 수 없습니다. (이미지 개수 : 0)";

            throw Exception(__LINE__, __FILE__, out.str());
        }

        imageCodexInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

        // 할당할 메모리가 부족할 경우
        if (imageCodexInfo == nullptr)
        {
            std::stringstream out;
            out << fileName << "의 파일 이름을 암호화할 수 없습니다. (메모리 부족)";

            throw Exception(__LINE__, __FILE__, out.str());
        }

        // 이미지를 암호할 암호 데이터를 가져옴
        GetImageEncoders(num, size, imageCodexInfo);

        for (UINT i = 0; i < num; i++)
        {
            if (wcscmp(imageCodexInfo[i].MimeType, format) == 0)
            {
                *clsid = imageCodexInfo[i].Clsid;
                free(imageCodexInfo);

                return;
            }
        }

        free(imageCodexInfo);

        std::stringstream out;
        out << fileName << "의 파일 이름을 암호화할 수 없습니다. (매칭된 암호화를 찾을 수 없음)";

        throw Exception(__LINE__, __FILE__, out.str());
    };

    CLSID dmpID;
    GetEncodeID(L"image/bmp", &dmpID);

    wchar_t toWidename[512];
    mbstowcs_s(nullptr, toWidename, fileName.c_str(), _TRUNCATE);

    Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)color.get());

    if (bitmap.Save(toWidename, &dmpID, nullptr) != Gdiplus::Status::Ok)
    {
        std::stringstream out;
        out << fileName << "의 파일 이름으로 저장할 수 없음.";

        throw Exception(__LINE__, __FILE__, out.str());
    }
}

void Material::Copy(const Material& src) NOEXCEPTRELEASE
{
    assert("복사할 표면의 너비 또는 높이가 다릅니다." && width == src.width && height == src.height);
    memcpy(color.get(), src.color.get(), width * height * sizeof(Color));
}

bool Material::HasAlpha() const noexcept
{
    return hasAlpha;
}

Material::Material(UINT width, UINT height, std::unique_ptr<Color[]> color, bool hasAlpha) noexcept 
    : width(width), height(height), color(std::move(color)), hasAlpha(hasAlpha)
{

}

DirectX::XMVECTOR Material::Color::ConvertVector(Material::Color color)
{
    using namespace DirectX;

    auto vector = XMVectorSet((float)color.GetR(), (float)color.GetG(), (float)color.GetB(), 0.0f);
    vector = vector * XMVectorReplicate(2.0f / 255.0f);
    vector = vector - Vector::allDirV;

    return vector;
}