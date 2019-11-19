#pragma once

#include <Scene.hpp>
#include <GameObject.hpp>
#include <IComponent.hpp>

namespace cf {
class CpnPrevPosition : public sfs::IComponent {
      public:
	void start(sfs::Scene &, sfs::GameObject &go) noexcept
	{
		_position = go.getPosition();
		_prevPosition = go.getPosition();
	}
	void update(sfs::Scene &, sfs::GameObject &go) noexcept
	{
		_prevPosition = _position;
		_position = go.getPosition();
	}
	const sf::Vector2f &getPrevPosition() const noexcept
	{
		return _prevPosition;
	}

      protected:
	sf::Vector2f _prevPosition;
	sf::Vector2f _position;
};
} // namespace cf