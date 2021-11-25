#include "Mesh.h"


void Mesh::addVertex(glm::vec3* vert) {
	this->vertex.push_back(vert);
}

vector<glm::vec3*> Mesh::getVertex() {
	return vertex;
}

void Mesh::addNormal(glm::vec3* norm) {
	this->norms.push_back(norm);
}

vector<glm::vec3*> Mesh::getNorms() {
	return norms;
}

void Mesh::addTexture(glm::vec2* text) {
	this->texts.push_back(text);
}

vector<glm::vec2*> Mesh::getTexts() {
	return texts;
}

void Mesh::addGroup(Group* group) {
	this->groups.push_back(group);
}

vector<Group*> Mesh::getGroups() {
	return groups;
}

Material* Mesh::getMaterialById(string materialId)
{
	Material* currentMaterial = nullptr;

	for (int index = 0; index < groups.size(); index++) {
		Group* group = groups[index];
		if (group->getMaterial() == nullptr) {
			continue;
		}

		if (group->getMaterial()->getId() == materialId) {
			currentMaterial = group->getMaterial();
			break;
		}
	}

	return currentMaterial;
}
