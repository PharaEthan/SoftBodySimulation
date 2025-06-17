#include "Exodia.hpp"
#include "Client/SoftBodyApp.hpp"
#include "Client/OrbitCamera.hpp"
#include "Client/SceneFactory.hpp"

int main()
{
	SoftBodySimulationApp app;

	SceneFactory::CreateSphere(app.GetScene(), app.GetRenderer(), app.GetSolver(), glm::vec3(0.0f, 9.0f, 0.0f), glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

    app.Run();

    return 0;
}
