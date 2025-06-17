#pragma once

#include <glm/glm.hpp>

namespace Exodia {

	struct PickResult {

		bool HasHit = false;

		glm::vec3 HitPoint;

		int FaceIndex = -1;
	};
};
