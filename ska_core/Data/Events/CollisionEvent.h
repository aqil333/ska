#pragma once

#include "../../ECS/ECSDefines.h"

namespace ska {
	class WorldCollisionComponent;
	class CollisionComponent;
	class CollidableComponent;

	/**
	 * \brief Declares an event struct that is sent to observers when a collision happen
	 */
	struct CollisionEvent {
		CollisionEvent(const EntityId e, WorldCollisionComponent* wcolComp, CollisionComponent* colComp, const CollidableComponent& colliComp) :
			entity(e),
			collisionComponent(colComp),
			wcollisionComponent(wcolComp),
			collidableComponent(colliComp)
			{ }

		void operator=(const CollisionEvent&) = delete;

		virtual ~CollisionEvent() = default;

		const EntityId entity;
		CollisionComponent* collisionComponent;
		WorldCollisionComponent* wcollisionComponent;
		const CollidableComponent& collidableComponent;

	};
}
