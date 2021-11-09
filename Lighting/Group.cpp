#include "Group.h"


Group::Group() {
	this->pMin = new glm::vec3(numeric_limits<float>::max());
	this->pMax = new glm::vec3(-numeric_limits<float>::max());
}

void Group::setNumVertices(int numVertices) {
	this->numVertices = numVertices;
}

unsigned int Group::getVAO() {
	return VAO;
}

void Group::setVAO(unsigned int VAO) {
	this->VAO = VAO;
}

void Group::setPMin(glm::vec3* p) {
	this->pMin = p;
}

void Group::setPMax(glm::vec3* p) {
	this->pMax = p;
}

unsigned int Group::getNumVertices() {
	return numVertices;
}

void Group::addFace(Face* face) {
	faces.push_back(face);
}

vector<Face*> Group::getFaces() {
	return faces;
}

void Group::setName(string name) {
	this->name = name;
}

void Group::setMaterial(Material* material) {
	this->material = material;
}

Material* Group::getMaterial() {
	return material;
}

glm::vec3* Group::getPMin() {
	return pMin;
}

glm::vec3* Group::getPMax() {
	return pMax;
}
