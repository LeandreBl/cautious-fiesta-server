#pragma once

#include "GameObject.hpp"
#include "GoEnnemy.hpp"
#include "GoBoss.hpp"
#include "GameManager.hpp"

namespace cf
{
    class SpawnManager : public sfs::GameObject
    {
        public:
            SpawnManager(GameManager &manager) noexcept;
            void start(sfs::Scene &scene) noexcept;
            void update(sfs::Scene &scene) noexcept;
            void ennemisStatsUp() noexcept;
        protected:
            int _difficulty = 0;
            float _difficultytime;
            float _time;
            int _ennemiIdName = 0;
            struct Stats _ennemiStats;
            GameManager &_gameManager;
    };
}