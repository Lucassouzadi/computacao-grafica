#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Group.h"

class Mesh {
	private:
		vector<glm::vec3*> vertex;
		vector<glm::vec2*> texts;
		vector<glm::vec3*> norms;
		vector<Group*> groups;
	public:
		void addVertex(glm::vec3* vertex);
		void addNormal(glm::vec3* normal);
		void addTexture(glm::vec2* texture);
		void addGroup(Group* group);
		vector<glm::vec3*> getVertex();
		vector<glm::vec3*> getNorms();
		vector<glm::vec2*> getTexts();
		vector<Group*> getGroups();
		Material* Mesh::getMaterialById(string materialId);
};