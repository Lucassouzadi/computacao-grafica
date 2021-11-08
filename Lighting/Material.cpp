#include "Material.h"

Material::Material(string id)
{
	this->id = id;
}

void Material::setKa(glm::vec3 ka) {
	this->ka = ka;
}

void Material::setKd(glm::vec3 kd) {
	this->kd = kd;
}

void Material::setKs(glm::vec3 ks) {
	this->ks = ks;
}

void Material::setNs(float ns) {
	this->ns = ns;
}

void Material::setMapKd(string mapKd) {
	this->mapKd = mapKd;
}

string Material::getId() {
	return id;
}

glm::vec3 Material::getKa() {
	return ka;
}

glm::vec3 Material::getKd() {
	return kd;
}

glm::vec3 Material::getKs() {
	return ks;
}

float Material::getNs() {
	return ns;
}

string Material::getMapKd() {
	return mapKd;
}
