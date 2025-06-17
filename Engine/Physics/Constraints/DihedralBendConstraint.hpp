#pragma once

#include "Constraint.hpp"
#include "Utils/Utils.hpp"

#include <glm/geometric.hpp>
#include <iostream>

namespace Exodia {

    class DihedralBendConstraint : public Constraint {

        public:

            DihedralBendConstraint(std::shared_ptr<Particle> p0, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, float compliance) : Constraint({ p0, p1, p2, p3 }, compliance, EQUALITY)
            {
                glm::vec3 p_0 = _Particles[0]->Position;
                glm::vec3 p_1 = _Particles[1]->Position;
                glm::vec3 p_2 = _Particles[2]->Position;
                glm::vec3 p_3 = _Particles[3]->Position;

                glm::vec3 n1 = glm::normalize(glm::cross(p_1 - p_0, p_2 - p_0));
                glm::vec3 n2 = glm::normalize(glm::cross(p_1 - p_0, p_3 - p_0));

                float dot = glm::dot(n1, n2);

                if (dot > 1.0f)
                    dot = 1.0f;
                if (dot < -1.0f)
                    dot = -1.0f;
                _Phi = acosf(dot);
            };

            float Evaluate() const override
            {
                glm::vec3 p0 = _Particles[0]->PredictedPosition;
                glm::vec3 p1 = _Particles[1]->PredictedPosition;
                glm::vec3 p2 = _Particles[2]->PredictedPosition;
                glm::vec3 p3 = _Particles[3]->PredictedPosition;

                glm::vec3 sharedEdgeDir = glm::normalize(p1 - p0);

                glm::vec3 n1 = glm::normalize(glm::cross(p1 - p0, p2 - p0));
                glm::vec3 n2 = glm::normalize(glm::cross(p1 - p0, p3 - p0));

                float dot = glm::dot(n1, n2);

                if (dot > 1.0f)
                    dot = 1.0f;
                if (dot < -1.0f)
                    dot = -1.0f;
                float phi = acosf(dot);

                if (glm::dot(glm::cross(n1, n2), sharedEdgeDir) < 0.0f)
                    phi = 2.0f * glm::pi<float>() - phi;
                return phi - _Phi;
            }

        private:

            void ComputeGradient() override
            {
                glm::vec3 p0 = _Particles[0]->PredictedPosition;
                glm::vec3 p1 = _Particles[1]->PredictedPosition;
                glm::vec3 p2 = _Particles[2]->PredictedPosition;
                glm::vec3 p3 = _Particles[3]->PredictedPosition;

                glm::vec3 el = p2 - p0;
                glm::vec3 em = p1 - p0;
                glm::vec3 er = p3 - p0;

                glm::vec3 n1 = glm::cross(p1 - p0, p2 - p0);
                glm::vec3 n2 = glm::cross(p1 - p0, p3 - p0);

                float l1 = glm::length(n1);
                float l2 = glm::length(n2);

                if (l1 == 0.0f || l2 == 0.0f)
                    return;
                n1 /= l1;
                n2 /= l2;

                float cosPhi = glm::dot(n1, n2);

                if (cosPhi > 1.0f)
                    cosPhi = 1.0f;
                if (cosPhi < -1.0f)
                    cosPhi = -1.0f;
                float cosPhi2 = cosPhi * cosPhi;

                if (cosPhi2 == 1.0f)
                    return;
                float arcosDerivative = -1.0f / sqrtf(1.0f - cosPhi2);

                if (glm::dot(glm::cross(n1, n2), em) < 0.0f)
                    arcosDerivative = -arcosDerivative;
                glm::vec3 dp1 = (glm::cross(er, n1) + cosPhi * glm::cross(n2, er)) / l2 + (glm::cross(el, n2) + cosPhi * glm::cross(n1, el)) / l1;
                glm::vec3 dp2 = (glm::cross(n2, em) - cosPhi * glm::cross(n1, em)) / l1;
                glm::vec3 dp3 = (glm::cross(n1, em) - cosPhi * glm::cross(n2, em)) / l2;

                glm::vec3 grad1 = arcosDerivative * dp1;
                glm::vec3 grad2 = arcosDerivative * dp2;
                glm::vec3 grad3 = arcosDerivative * dp3;
                glm::vec3 grad0 = -grad1 - grad2 - grad3;

                _Gradient[0] = grad0;
                _Gradient[1] = grad1;
                _Gradient[2] = grad2;
                _Gradient[3] = grad3;
            }

            void RecomputeTargetValue() override
            {
                glm::vec3 p_0 = _Particles[0]->Position;
                glm::vec3 p_1 = _Particles[1]->Position;
                glm::vec3 p_2 = _Particles[2]->Position;
                glm::vec3 p_3 = _Particles[3]->Position;

                glm::vec3 n1 = glm::normalize(glm::cross(p_1 - p_0, p_2 - p_0));
                glm::vec3 n2 = glm::normalize(glm::cross(p_1 - p_0, p_3 - p_0));

                float dot = glm::dot(n1, n2);

                if (dot > 1.0f)
                    dot = 1.0f;
                if (dot < -1.0f)
                    dot = -1.0f;
                _Phi = acosf(dot);
            }

        private:

            float _Phi;
    };
};
