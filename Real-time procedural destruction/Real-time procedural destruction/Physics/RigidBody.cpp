#include "RigidBody.h"
#include "..\GameEngine\Transform.h"

namespace GameEngine
{
    void RigidBody::initialize()
    {
        m_velocity = glm::vec3(0.0f);
        m_acceleration = glm::vec3(0.0f);
        m_mass = 1.0f;
        m_useGravity = true;
        m_gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        m_forces = glm::vec3(0.0f);
        if (m_useGravity)
        {
            addForce(m_gravity * m_mass);
        }
    }

    void RigidBody::onTick()
    {
        /* Apply gravity if active */
        if (m_useGravity)
        {
            addForce(m_gravity * m_mass);
        }

        /* Calculate acceleration from accumulated forces */
        m_acceleration = m_forces / m_mass;

        /* Calculate velocity from acceleration */
        m_velocity += m_acceleration * (float)getDT();

        m_velocity *= m_dampening;

        /* Calculate new position from velocity */
        auto transform = m_entity.lock()->m_transform.lock();
        glm::vec3 pos = transform->getPos();
        pos += m_velocity * (float)getDT();
        transform->setPos(pos);

        /* Reset Forces for next frame */
        m_forces = glm::vec3(0.0f);
    }
}