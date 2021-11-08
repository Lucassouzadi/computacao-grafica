#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Obj3D.h"

class ObjManager {
	public:
		Obj3D* readObj(string fileName);
		void loadMaterials(Obj3D* obj);
		Obj3D* getHardcodedCube(GLfloat size);
		Obj3D* get2DCircle(GLfloat radius, int vertices);
		Obj3D* getCross(GLfloat width, GLfloat length);
		void objToVAO(Obj3D* obj3D);
};
