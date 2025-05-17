#include "DestructedObject.h"   

DestructedObject::~DestructedObject() {}
DestructedObject::DestructedObject() {}

void DestructedObject::initialize() 
{
    m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();

    pos = m_transform.lock()->getPos();
}

void DestructedObject::onTick()
{
    m_transform.lock()->setPos(glm::vec3(pos.x, pos.y, pos.z - 1.5f));
}
