#pragma once
#include "..\GameEngine\Component.h"

namespace GameEngine
{
    struct RigidBody : Component
    {
        /* Override component functions */
        void initialize() override;
        void onTick() override;

        /* Variable Setters */
        void addForce(const glm::vec3& _force) { m_forces += _force; }
        void addImpulse(const glm::vec3& impulse) { m_velocity += impulse / m_mass; }

        void setVelocity(const glm::vec3& _velocity) { m_velocity = _velocity; }
        void setMass(float _mass) { m_mass = (_mass > 0.1f) ? _mass : 0.1f; }
        void setUseGravity(bool _useGravity) { m_useGravity = _useGravity; }
        void setGravity(glm::vec3 _gravity) { m_gravity = _gravity; }

        /* Variable Getters */
        glm::vec3 getVelocity() { return m_velocity; }

    private:
        /* Physics based variables */
        glm::vec3 m_velocity;
        glm::vec3 m_acceleration;
        glm::vec3 m_forces;
        float m_mass;
        bool m_useGravity;
        glm::vec3 m_gravity;

        float m_dampening = 1.0f;
    };
}