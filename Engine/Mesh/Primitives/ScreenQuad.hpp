#pragma once

#include <vector>

namespace Exodia {

    class ScreenQuad {
    
        public:

            ScreenQuad();

        public:
    
            void Render();

        private:

            unsigned int _VAO   {};
            unsigned int _VBO   {};
            unsigned int _IVO   {};
            unsigned int _UVVBO {};

            std::vector<int> Indices = {
                0, 1, 2,
                1, 3, 2
            };
    };
};
