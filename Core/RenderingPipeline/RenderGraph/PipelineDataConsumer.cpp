#include "stdafx.h"
#include "PipelineDataConsumer.h"
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"
#include "Utility/MathInfo.h"

#include <sstream>
#include <cctype>
#include <algorithm>

namespace RenderGraphNameSpace
{
    // 등록 이름 유효성 검증을 포함한 생성자
    PipelineDataConsumer::PipelineDataConsumer(std::string registeredNameIn)
        : registeredName(std::move(registeredNameIn))
    {
        // 빈 이름 검증
        if (registeredName.empty())
        {
            throw RENDER_GRAPHIC_EXCEPTION("렌더 그래프 오류: 데이터 소비자의 등록 이름이 비어있습니다. 유효한 이름을 제공해주세요.");
        }

        // 이름 형식 검증 (영숫자와 언더스코어만 허용)
        const bool isNameValid = std::all_of(registeredName.begin(), registeredName.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        });

        // 잘못된 문자 포함 시 예외 발생
        if (!isNameValid)
        {
            std::ostringstream oss;
            oss << "렌더 그래프 오류: 데이터 소비자 등록 이름 [" << registeredName << "]에 잘못된 문자가 포함되어 있습니다. "
                << "영문자, 숫자, 언더스코어(_)만 사용할 수 있습니다.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 첫 글자가 숫자인 경우 예외 발생
        if (std::isdigit(registeredName.front()))
        {
            std::ostringstream oss;
            oss << "렌더 그래프 오류: 데이터 소비자 등록 이름 [" << registeredName << "]이 숫자로 시작합니다. "
                << "이름은 영문자 또는 언더스코어(_)로 시작해야 합니다.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    // 등록된 이름 반환
    const std::string& PipelineDataConsumer::GetRegisteredName() const noexcept
    {
        return registeredName;
    }

    // 연결된 패스 이름 반환
    const std::string& PipelineDataConsumer::GetPassName() const noexcept
    {
        return passName;
    }

    // 연결된 출력 이름 반환
    const std::string& PipelineDataConsumer::GetOutputName() const noexcept
    {
        return outputName;
    }

    // 타겟 패스와 출력 이름 설정 (각각 유효성 검증 포함)
    void PipelineDataConsumer::SetTarget(std::string passNameIn, std::string outputNameIn)
    {
        // 패스 이름 검증 및 설정
        {
            if (passNameIn.empty())
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 패스 이름이 비어있습니다. "
                    << "유효한 패스 이름을 제공해주세요.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 이름 형식 검증 (영숫자와 언더스코어만 허용, "$"는 특별히 허용 - 글로벌 출력 의미)
            const bool isNameValid = std::all_of(passNameIn.begin(), passNameIn.end(), [](char c) {
                return std::isalnum(c) || c == '_';
            });

            if (passNameIn != "$" && !isNameValid)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 패스 이름 [" << passNameIn 
                    << "]에 잘못된 문자가 포함되어 있습니다. 영문자, 숫자, 언더스코어(_)만 사용하거나 글로벌 출력을 위해 '$'를 사용할 수 있습니다.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            if (passNameIn != "$" && std::isdigit(passNameIn.front()))
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 패스 이름 [" << passNameIn 
                    << "]이 숫자로 시작합니다. 패스 이름은 영문자 또는 언더스코어(_)로 시작해야 합니다.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            this->passName = std::move(passNameIn);
        }

        // 출력 이름 검증 및 설정
        {
            if (outputNameIn.empty())
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 출력 이름이 비어있습니다. "
                    << "유효한 출력 이름을 제공해주세요.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 이름 형식 검증 (영숫자와 언더스코어만 허용)
            const bool isNameValid = std::all_of(outputNameIn.begin(), outputNameIn.end(), [](char c) {
                return std::isalnum(c) || c == '_';
            });

            if (!isNameValid)
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 출력 이름 [" << outputNameIn 
                    << "]에 잘못된 문자가 포함되어 있습니다. 영문자, 숫자, 언더스코어(_)만 사용할 수 있습니다.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            if (std::isdigit(outputNameIn.front()))
            {
                std::ostringstream oss;
                oss << "렌더 그래프 오류: 데이터 소비자 [" << registeredName << "]에 설정하려는 출력 이름 [" << outputNameIn 
                    << "]이 숫자로 시작합니다. 출력 이름은 영문자 또는 언더스코어(_)로 시작해야 합니다.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            this->outputName = std::move(outputNameIn);
        }
    }
}