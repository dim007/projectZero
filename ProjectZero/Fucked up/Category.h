#pragma once
#include "stdafx.h"

namespace Category {
	enum Type {
		None = 0,
		Platform = 1 << 0,
		PlayerRobot = 1 << 1,
		AlliedRobot = 1 << 2,
		EnemyRobot = 1 << 3,
		Pickup = 1 << 4,
		EnemyProjectile = 1 << 5,
		AlliedProjectile = 1 << 6,
		ActionLayer = 1 << 7,
		EnemyAttack = 1 << 8,
		AlliedAttack = 1 << 9,

		Robot = PlayerRobot | AlliedRobot | EnemyRobot,
		Projectile = AlliedProjectile | EnemyProjectile
	};
}