#include "DestructedObject.h"   

DestructedObject::~DestructedObject() {}
DestructedObject::DestructedObject() {}

void DestructedObject::initialize() 
{
    m_transform = m_entity.lock()->m_transform;
}
