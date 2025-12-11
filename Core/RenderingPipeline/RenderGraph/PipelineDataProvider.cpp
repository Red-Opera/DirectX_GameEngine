#include "stdafx.h"
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace RenderGraphNameSpace
{
    // 이름 유효성 검증을 포함한 생성자
    PipelineDataProvider::PipelineDataProvider(std::string nameIn)
        : name(std::move(nameIn))
    {
        // 빈 이름 검증
        if (name.empty())
            throw RENDER_GRAPHIC_EXCEPTION("렌더 그래프 오류: 데이터 제공자의 이름이 비어있습니다. 유효한 이름을 제공해주세요.");

        // 이름 형식 검증 (영숫자와 언더스코어만 허용)
        const bool isNameValid = std::all_of(name.begin(), name.end(), [](char c) { 
            return std::isalnum(c) || c == '_'; 
        });

        // 첫 글자가 숫자이거나 유효하지 않은 문자 포함 시 예외 발생
        if (!isNameValid)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 오류: 데이터 제공자 이름 [" << name << "]에 잘못된 문자가 포함되어 있습니다. "
                << "영문자, 숫자, 언더스코어(_)만 사용할 수 있습니다.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
        
        if (std::isdigit(name.front()))
        {
            std::ostringstream oss;
            oss << "렌더 그래프 오류: 데이터 제공자 이름 [" << name << "]이 숫자로 시작합니다. "
                << "이름은 영문자 또는 언더스코어(_)로 시작해야 합니다.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    // 렌더 객체 접근 시 예외 발생 (기본 구현)
    std::shared_ptr<Graphic::Render> PipelineDataProvider::GetRender()
    {
        std::ostringstream oss;
        oss << "렌더 그래프 오류: 데이터 제공자 [" << name << "]에서 렌더 객체에 접근하려고 했지만, "
            << "이 제공자는 렌더 객체를 제공하지 않습니다. 올바른 제공자 타입을 사용해주세요.";
        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 버퍼 데이터 접근 시 예외 발생 (기본 구현)
    std::shared_ptr<Graphic::BufferResource> PipelineDataProvider::GetData()
    {
        std::ostringstream oss;
        oss << "렌더 그래프 오류: 데이터 제공자 [" << name << "]에서 버퍼 리소스에 접근하려고 했지만, "
            << "이 제공자는 버퍼 리소스를 제공하지 않습니다. 올바른 제공자 타입을 사용해주세요.";
        throw RENDER_GRAPHIC_EXCEPTION(oss.str());
    }

    // 데이터 제공자의 이름 반환
    const std::string& PipelineDataProvider::GetName() const noexcept
    {
        return name;
    }
}