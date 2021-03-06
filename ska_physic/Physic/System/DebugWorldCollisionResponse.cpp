#include "Data/Events/CollisionEvent.h"
#include "DebugWorldCollisionResponse.h"
#include "ECS/Basics/Graphic/DebugGraphicComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"

ska::DebugWorldCollisionResponse::DebugWorldCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
WorldCollisionObserver(std::bind(&DebugWorldCollisionResponse::onWorldCollision, this, std::placeholders::_1), ged),
	m_entityManager(em){
}

bool ska::DebugWorldCollisionResponse::onWorldCollision(CollisionEvent& e) const{
	DebugGraphicComponent dgc;
	dgc.typeMask = COLLISION;
	m_entityManager.addComponent<DebugGraphicComponent>(e.entity, std::move(dgc));
	return true;
}

ska::DebugWorldCollisionResponse::~DebugWorldCollisionResponse() {
}
