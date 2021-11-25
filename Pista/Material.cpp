#include "Material.h"
#include <SOIL.h>

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

unsigned int Material::getTexture() {
	return texture;
}

void Material::setTexture(GLuint texture) {
	this->texture = texture;
}

void Material::loadTexture(const char* filepath)
{
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image(filepath, &width, &height, 0, SOIL_LOAD_RGBA);

	if (image) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture: %s", filepath);
	}
	SOIL_free_image_data(image);
}