#include "MeshCollider.h"
namespace GameEngine
{
    MeshCollider::~MeshCollider() {}

    void MeshCollider::initialize()
    {
        m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
    }

    /*
    * onTick() runs every frame
    */
    void MeshCollider::onTick()
    {
        
    }

    finalIntersection MeshCollider::rayIntersect(Ray _ray)
    {
        finalIntersection info;

        return info;
    }


}
