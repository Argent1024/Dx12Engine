#pragma once

#include "stdafx.h"
#include "WinApp.h"

#include "World/Engine.h"


#include <iostream>


_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	Engine::GameEngine* engine = new Engine::GameEngine();
	return WinApp::RunApp(engine, hInstance, nCmdShow);
}