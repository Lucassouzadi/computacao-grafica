#ifndef SYSTEM_H
#define SYSTEM_H

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

// External Libs
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

// GLM Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// Headers
#include "Shader.h"
#include "AssetManager.h"
#include "Time.h"

#include "Obj3D.h"

class System
{
private:
	// Screen
	const GLint WIDTH = 1000, HEIGHT = 800;
	int screenWidth, screenHeight;

public:
	GLFWwindow* window;
	Shader coreShader;
	Obj3D *auxCircle, *auxBox;
	int alphaLocation, modelLocation, projectionLocation, viewLocation;
public:
	System();
	~System();

	int GLFWInit();
	int OpenGLSetup();
	int SystemSetup();

	void Run();

	void ProcessInput(GLFWwindow* window, float elapsedSeconds);

	bool testCollisionSphereVSCube(Obj3D* sphere, Obj3D* box, bool visilizeCollisionTesting);

	void Finish();

};

#endif