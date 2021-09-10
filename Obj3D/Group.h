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
	public:
		void setName(string VAO);
		void setMaterial(string material);
		void addFace(Face* face);
		void setVAO(unsigned int VAO);
		string getName();
		string getMaterial();
		vector<Face*> getFaces();
		unsigned int getVAO();
		unsigned int getNumVertices();
};
