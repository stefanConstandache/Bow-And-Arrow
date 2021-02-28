#include <ctime>
#include <iostream>

using namespace std;

#include <Core/Engine.h>
#include <Teme/Tema1/Tema1.h>
//#include <Teme/Tema2/Tema2.h>
#include <Teme/Tema3/Tema3.h>
#include <Laboratoare/LabList.h>

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	// Create a window property structure
	WindowProperties wp;
	wp.resolution = glm::ivec2(1280, 720);
	wp.name = "EGC";

	// Init the Engine and create a new window with the defined properties
	WindowObject* window = Engine::Init(wp);

	// Create a new 3D world and start running it
	World *world = new Tema3();
	
	world->Init();
	world->Run();

	// Signals to the Engine to release the OpenGL context
	Engine::Exit();
	
	return 0;
}