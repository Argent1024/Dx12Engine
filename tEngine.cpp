// tEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "WinApp.h"
//#include "src/Samples/RayMarching.h"
//#include "src/Samples/TextureTest.h"
#include "src/Samples/ParticlesTest.h"

#include <iostream>


_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	//Graphic::GraphicCore *graphic_core = new Samples::RayMarching(1280, 720);
	Graphic::GraphicCore *graphic_core = new Samples::ParticleTest(1280, 720);
	return WinApp::RunApp(graphic_core, hInstance, nCmdShow);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
