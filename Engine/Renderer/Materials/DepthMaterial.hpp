#pragma once

#include "Material.hpp"

namespace Exodia {

    class DepthMaterial : public Material {

        public:

            DepthMaterial() : Material("../../Assets/Shaders/Depth")
            {
                SetBackFaceCullingEnabled(false);
            }
    };
};
