#pragma once

#include "Transform.hpp"

namespace Exodia {

	class Transformable {

		public:
			virtual Transform *Transform() = 0;
	};
};
