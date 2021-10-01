#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Obj3D.h"

class ObjManager {
	public:
		Obj3D* readObj(string fileName);
		Obj3D* getHardcodedCube(GLfloat size);
		Obj3D* get2DCircle(GLfloat radius, int vertices);
		void objToVAO(Obj3D* obj3D);
};
