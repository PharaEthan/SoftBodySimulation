#pragma once

#include <random>
#include <sstream>

namespace Exodia {

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    class UUID {

        public:

            static std::string GenerateUUIDV4()
            {
                std::stringstream ss;
                int i;

                ss << std::hex;

                for (i = 0; i < 8; i++)
                    ss << dis(gen);
                ss << "-";

                for (i = 0; i < 4; i++)
                    ss << dis(gen);
                ss << "-4";

                for (i = 0; i < 3; i++)
                    ss << dis(gen);
                ss << "-";
                ss << dis2(gen);

                for (i = 0; i < 3; i++)
                    ss << dis(gen);
                ss << "-";
                for (i = 0; i < 12; i++)
                    ss << dis(gen);
                return ss.str();
            }
    };
};
