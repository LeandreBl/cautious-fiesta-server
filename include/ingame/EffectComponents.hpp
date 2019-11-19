#pragma once

#include <IComponent.hpp>
#include <IGoEntity.hpp>

namespace cf {
class CpnFlame : public sfs::IComponent {
      public:
	CpnFlame(float damagePerSec, float duration)
		: _damage(damagePerSec)
		, _elapsed(0)
		, _duration(duration)
	{
	}

	void update(sfs::Scene &scene, sfs::GameObject &object) noexcept
	{
		auto delta = scene.deltaTime();

		_elapsed += delta;
		if (_elapsed > _duration)
			destroy();
		else
			static_cast<IGoEntity &>(object).inflictDamage(_damage * delta);
	}

      protected:
	float _damage;
	float _elapsed;
	float _duration;
};

class CpnFreeze : public sfs::IComponent {
      public:
	CpnFreeze(float percentage, float duration)
		: _freeze(percentage)
		, _elapsed(0)
		, _duration(duration)
	{
	}

	void start(sfs::Scene &, sfs::GameObject &object) noexcept
	{
		auto &entity = static_cast<IGoEntity &>(object);
		auto speed = entity.getSpeed();
		_removed = speed - (_freeze * speed / 100);
		entity.setSpeed(speed - _removed);
	}

	void update(sfs::Scene &scene, sfs::GameObject &object) noexcept
	{
		auto delta = scene.deltaTime();

		_elapsed += delta;
		if (_elapsed > _duration) {
			auto &entity = static_cast<IGoEntity &>(object);
			entity.setSpeed(entity.getSpeed() + _removed);
			destroy();
		}
	}

      protected:
	float _removed;
	float _freeze;
	float _elapsed;
	float _duration;
};
} // namespace cf