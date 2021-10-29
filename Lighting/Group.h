#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <string>
#include <GL/glew.h>

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
		GLuint texture;

	public:
		Group();
		void setName(string VAO);
		void setMaterial(string material);
		void addFace(Face* face);
		void setVAO(unsigned int VAO);
		void setNumVertices(int numVertices);
		void setPMin(glm::vec3* p);
		void setPMax(glm::vec3* p);
		void setTexture(GLuint texture);
		string getName();
		string getMaterial();
		vector<Face*> getFaces();
		unsigned int getVAO();
		unsigned int getNumVertices();
		glm::vec3* getPMin();
		glm::vec3* getPMax();
		GLuint getTexture();
};
