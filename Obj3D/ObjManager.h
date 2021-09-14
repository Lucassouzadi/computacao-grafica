#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Obj3D.h"

class ObjManager {
	public:
		Mesh* readObj(string fileName);
		Obj3D* getHardcoded2DHouse();
		Obj3D* getHardcodedCube(GLfloat size);
		void objToVAO(Obj3D* obj3D);
};
