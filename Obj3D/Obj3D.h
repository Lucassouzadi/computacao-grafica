#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"

class Obj3D {
	private:
		bool collision;
		Mesh* mesh;
		glm::mat4 translate;
		glm::vec3 origin;
		glm::vec3 position;
		glm::vec3 eulerAngles;
		glm::vec3 scale;
		glm::vec3 direction;
		glm::vec3* globalPMin;
		glm::vec3* globalPMax;
		string name;
		void Obj3D::computeTranslate();
	public:
		Obj3D();
		void setOrigin(glm::vec3 origin);
		void setPosition(glm::vec3 position);
		void setEulerAngles(glm::vec3 eulerAngles);
		void setScale(glm::vec3 scale);
		void setCollision(bool collision);
		void setDirection(glm::vec3 direction);
		void setMesh(Mesh* mesh);
		void setName(string name);
		void setGlobalPMin(glm::vec3* p);
		void setGlobalPMax(glm::vec3* p);
		bool getCollision();
		glm::mat4 getTranslate();
		glm::vec3 getOrigin();
		glm::vec3 getPosition();
		glm::vec3 getEulerAngles();
		glm::vec3 getScale();
		glm::vec3 getDirection();
		Mesh* getMesh();
		string getName();
		glm::vec3* getGlobalPMin();
		glm::vec3* getGlobalPMax();
};
