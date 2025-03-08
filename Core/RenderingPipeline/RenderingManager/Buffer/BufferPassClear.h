#pragma once

#include "../Pass/Base/RenderPass.h"

namespace Graphic { class BufferResource; }

namespace RenderGraphNameSpace
{
	class BufferPassClear : public RenderPass
	{
	public:
		BufferPassClear(std::string name);

		void Execute() const NOEXCEPTRELEASE override;

	private:
		std::shared_ptr<Graphic::BufferResource> buffer;
	};
}
