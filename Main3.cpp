#include "Exodia.hpp"
#include "Client/SoftBodyApp.hpp"
#include "Client/OrbitCamera.hpp"
#include "Client/SceneFactory.hpp"

int main()
{
	SoftBodySimulationApp app;

    int resolution = 32;

    auto carpet = SceneFactory::CreateCarpet(app.GetScene(), app.GetRenderer(), app.GetSolver(), resolution, { 0.0, 2.0f, 0.0 }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 10.0f });

    carpet->AddFixedConstraint(std::make_shared<FixedConstraint>(carpet->GetParticles()[0]));
    carpet->AddFixedConstraint(std::make_shared<FixedConstraint>(carpet->GetParticles()[resolution - 1]));
    carpet->AddFixedConstraint(std::make_shared<FixedConstraint>(carpet->GetParticles()[resolution * (resolution - 1)]));
    carpet->AddFixedConstraint(std::make_shared<FixedConstraint>(carpet->GetParticles()[resolution * resolution - 1]));

    app.Run();

    return 0;
}
