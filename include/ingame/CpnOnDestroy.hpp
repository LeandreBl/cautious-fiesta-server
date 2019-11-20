#pragma once

#include <functional>

#include <IComponent.hpp>

namespace cf {
class CpnOnDestroy : public sfs::IComponent {
      public:
	CpnOnDestroy(const std::function<void()> &&callback)
		: _callbackOnDestroy(callback)
	{
	}
	~CpnOnDestroy()
	{
		_callbackOnDestroy();
	}

      protected:
	std::function<void()> _callbackOnDestroy;
};
} // namespace cf