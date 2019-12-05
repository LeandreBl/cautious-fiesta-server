#pragma once

#include <IComponent.hpp>
#include <Scene.hpp>
#include <GameObject.hpp>

namespace cf {
class CpnTimer : public sfs::IComponent {
      public:
	CpnTimer(float time) noexcept
		: _timer(time)
	{
	}
	void start(sfs::Scene &scene, sfs::GameObject &) noexcept
	{
		float t = scene.time();
		_timer = t + _timer;
	}
	void update(sfs::Scene &scene, sfs::GameObject &go) noexcept
	{
		if (scene.time() >= _timer)
			go.destroy();
	}

      private:
	float _timer;
};
} // namespace cf