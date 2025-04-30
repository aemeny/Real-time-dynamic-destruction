#include "DestructionObjs.h"
#include <vector>

DestructionObjs::~DestructionObjs() {}

DestructionObjs::DestructionObjs() {}

/*
* initialize function called from component, sets up model
*/
void DestructionObjs::initialize()
{
	std::weak_ptr<GameEngine::ModelRenderer> modelRenderer = m_entity.lock()->findComponent<GameEngine::ModelRenderer>();

	modelRenderer.lock()->setModel("Cube/Cube.obj", true); // Path, model will change later so load individually
	modelRenderer.lock()->setTexture("Floor/CustomUV.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();

	std::weak_ptr<Renderer::Model> model = modelRenderer.lock()->getModel();
	std::vector<bu::Face>* faces = model.lock()->getFaces();
	glm::vec3 scale = m_transform.lock()->getScale() * (1.0f/1.3f);
	for (bu::Face& face : *faces)
	{
		if (face.na.z == 1 || face.na.z == -1)
		{
			face.tca = glm::vec2(face.pa.x * scale.x, face.pa.y * scale.y);
			face.tcb = glm::vec2(face.pb.x * scale.x, face.pb.y * scale.y);
			face.tcc = glm::vec2(face.pc.x * scale.x, face.pc.y * scale.y);
		}
		else if (face.na.x == 1 || face.na.x == -1)
		{
			face.tca = glm::vec2(face.pa.z * scale.z, face.pa.y * scale.y);
			face.tcb = glm::vec2(face.pb.z * scale.z, face.pb.y * scale.y);
			face.tcc = glm::vec2(face.pc.z * scale.z, face.pc.y * scale.y);
		}
		else
		{
			face.tca = glm::vec2(face.pa.x * scale.x, face.pa.z * scale.z);
			face.tcb = glm::vec2(face.pb.x * scale.x, face.pb.z * scale.z);
			face.tcc = glm::vec2(face.pc.x * scale.x, face.pc.z * scale.z);
		}
	}

	m_transform.lock()->setDirty(true);
	model.lock()->updateModel();
	model.lock()->setDestruction(true);
}
