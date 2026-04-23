#pragma once

#include <string>

namespace AICombat
{
    class ICombatant
    {
    public:
        virtual bool IsAlive() const = 0;
        virtual void TakeDamage(int damage) = 0;

        virtual const std::string& GetCurrentStateName() const = 0;
        virtual float GetStateTime() const = 0;

        virtual ~ICombatant() = default;
    };
}