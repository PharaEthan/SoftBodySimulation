#include "Exodia.hpp"
#include "Client/SoftBodyApp.hpp"
#include "Client/OrbitCamera.hpp"
#include "Client/SceneFactory.hpp"

int main()
{
	SoftBodySimulationApp app;

	SceneFactory::CreateCarpet(app.GetScene(), app.GetRenderer(), app.GetSolver(), 32, { 0.0, 0.2, 0.0 }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 10.0f });

    app.Run();

    return 0;
}
