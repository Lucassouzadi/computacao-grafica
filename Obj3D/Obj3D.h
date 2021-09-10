#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"

class Obj3D {
	private:
		bool collision;
		Mesh* mesh;
		glm::mat4 translate;
		glm::vec3 direction;
		string name;
	public:
		void setCollision(bool collision);
		void setTranslate(glm::mat4 direction);
		void setDirection(glm::vec3 direction);
		void setMesh(Mesh* mesh);
		void setName(string name);
		bool getCollision();
		glm::mat4 getTranslate();
		glm::vec3 getDirection();
		Mesh* getMesh();
		string getName();
};
