#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <string>

#include "Face.h"

class Group {
	private:
		string name;
		string material;
		vector<Face*> faces;
		unsigned int VAO;
		unsigned int numVertices;
		glm::vec3* pMin;
		glm::vec3* pMax;
	public:
		Group();
		void setName(string VAO);
		void setMaterial(string material);
		void addFace(Face* face);
		void setVAO(unsigned int VAO);
		void setNumVertices(int numVertices);
		void setPMin(glm::vec3* p);
		void setPMax(glm::vec3* p);
		string getName();
		string getMaterial();
		vector<Face*> getFaces();
		unsigned int getVAO();
		unsigned int getNumVertices();
		glm::vec3* getPMin();
		glm::vec3* getPMax();
};
