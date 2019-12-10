#pragma once

#include <IComponent.hpp>

#include "GameManager.hpp"

namespace cf {
class CpnWandering : public sfs::IComponent {
      public:
	CpnWandering(GameManager &manager, float delta) noexcept
		: _manager(manager)
		, _delta(delta)
	{
	}
	void start(sfs::Scene &scene, sfs::GameObject &go) noexcept
	{
		_previous = scene.time();
	}
	void update(sfs::Scene &scene, sfs::GameObject &go) noexcept
	{
		float dt = scene.time();

		if (dt - _previous > _delta) {
			_previous = dt;
			auto v = go.getComponents<sfs::Velocity>();
			for (auto &&c : v) {
				c->speed.x *= -1;
				Serializer s;
				s << go.getId() << c->speed.x << c->speed.y << c->acceleration.x
				  << c->acceleration.y;
				_manager.updateUdp(s, UdpPrctl::Type::VELOCITY);
			}
		}
		Serializer s;
		s << go.getId() << go.getPosition().x << go.getPosition().y;
		_manager.updateUdp(s, UdpPrctl::Type::POSITION);
	}
	GameManager &_manager;
	float _delta;
	float _previous;
};
}