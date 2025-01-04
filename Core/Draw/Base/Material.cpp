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
    return "�׷��� ���� ����";
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
    assert("������ �ȼ��� ��ġ�� ������ϴ�!" && x >= 0 && y >= 0 && x < width && y < height);

    color[y * width + x] = c;
}

Material::Color Material::GetColorPixel(UINT x, UINT y) const NOEXCEPTRELEASE
{
    assert("������ �ȼ��� ��ġ�� ������ϴ�!" && x >= 0 && y >= 0 && x < width && y < height);

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
    // �ʺ�, ����, ��ġ
    UINT width = 0, height = 0;
    std::unique_ptr<Color[]> color = nullptr;
    bool hasAlpha = false;

    wchar_t toWidename[512];
    mbstowcs_s(nullptr, toWidename, name.c_str(), _TRUNCATE);

    Gdiplus::Bitmap bitmap(toWidename);
    if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
    {
        std::stringstream out;
        out << name << "�� ������ �ε��� �� ����!";

        throw Exception(__LINE__, __FILE__, out.str());
    }

    height = bitmap.GetHeight();
    width = bitmap.GetWidth();
    color = make_unique<Color[]>(width * height);

    // �о�� ���� �̹����� �ȼ��� ���� ���ۿ� ������
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
        // ��ȣȭ�� �̹����� ������ ����Ʈ ũ��
        UINT num = 0, size = 0;

        Gdiplus::ImageCodecInfo* imageCodexInfo = nullptr;
        Gdiplus::GetImageEncodersSize(&num, &size);

        // ��ȣȭ�� �̹����� �������� �ʴ� ���
        if (size == 0)
        {
            std::stringstream out;
            out << fileName << "�� ���� �̸��� ��ȣȭ�� �� �����ϴ�. (�̹��� ���� : 0)";

            throw Exception(__LINE__, __FILE__, out.str());
        }

        imageCodexInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

        // �Ҵ��� �޸𸮰� ������ ���
        if (imageCodexInfo == nullptr)
        {
            std::stringstream out;
            out << fileName << "�� ���� �̸��� ��ȣȭ�� �� �����ϴ�. (�޸� ����)";

            throw Exception(__LINE__, __FILE__, out.str());
        }

        // �̹����� ��ȣ�� ��ȣ �����͸� ������
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
        out << fileName << "�� ���� �̸��� ��ȣȭ�� �� �����ϴ�. (��Ī�� ��ȣȭ�� ã�� �� ����)";

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
        out << fileName << "�� ���� �̸����� ������ �� ����.";

        throw Exception(__LINE__, __FILE__, out.str());
    }
}

void Material::Copy(const Material& src) NOEXCEPTRELEASE
{
    assert("������ ǥ���� �ʺ� �Ǵ� ���̰� �ٸ��ϴ�." && width == src.width && height == src.height);
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