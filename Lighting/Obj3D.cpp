#include "Obj3D.h"
#include <glm\gtc\matrix_transform.hpp>
#include <SOIL.h>

Obj3D::Obj3D() {
	this->active = true;
	this->globalPMin = new glm::vec3(numeric_limits<float>::max());
	this->globalPMax = new glm::vec3(-numeric_limits<float>::max());
	this->position = glm::vec3(0.0f);
	this->origin = glm::vec3(0.0f);
	this->eulerAngles = glm::vec3(0.0f);
	this->scale = glm::vec3(1.0f);
	this->direction = glm::vec3(0.0f);
	this->computeTranslate();
}

Obj3D* Obj3D::copy() {
	Obj3D* newObj = new Obj3D();
	newObj->active = this->active;
	newObj->mesh = this->getMesh();
	newObj->globalPMin = this->globalPMin;
	newObj->globalPMax = this->globalPMax;
	newObj->position = this->position;
	newObj->origin = this->origin;
	newObj->eulerAngles = this->eulerAngles;
	newObj->scale = this->scale;
	newObj->direction = this->direction;
	newObj->texture = this->texture;
	newObj->computeTranslate();
	return newObj;
}

void Obj3D::computeTranslate()
{
	glm::mat4 newTranslate, translateMatrix, yaw, pitch, roll, rotationMatrix, scaleMatrix;
	
	translateMatrix = glm::translate(glm::mat4(1.0f), this->position + this->origin);

	yaw = glm::rotate(glm::mat4(1.0f), glm::radians(this->eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	pitch = glm::rotate(glm::mat4(1.0f), glm::radians(this->eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	roll = glm::rotate(glm::mat4(1.0f), glm::radians(this->eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMatrix = yaw * pitch * roll * glm::translate(glm::mat4(1.0f), -this->origin);

	scaleMatrix = glm::scale(glm::mat4(1.0f), this->scale);

	this->translate = translateMatrix * rotationMatrix * scaleMatrix;
}

void Obj3D::setOrigin(glm::vec3 origin)
{
	this->origin = origin;
	this->computeTranslate();
}

void Obj3D::setPosition(glm::vec3 position)
{
	this->position = position;
	this->computeTranslate();
}

void Obj3D::setEulerAngles(glm::vec3 eulerAngles)
{
	this->eulerAngles = eulerAngles;
	this->computeTranslate();
}

void Obj3D::setScale(glm::vec3 scale)
{
	this->scale = scale;
	this->computeTranslate();
}

void Obj3D::setCollision(bool collision)
{
	this->collision = collision;
}

void Obj3D::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}

void Obj3D::setName(string name)
{
	this->name = name;
}

void Obj3D::loadTexture(char* filepath)
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

void Obj3D::setMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

void Obj3D::setGlobalPMin(glm::vec3* p) {
	this->globalPMin = p;
}

void Obj3D::setGlobalPMax(glm::vec3* p) {
	this->globalPMax = p;
}

void Obj3D::setActive(bool active) {
	this->active = active;
}

void Obj3D::setColor(glm::vec3 color) {
	this->color = color;
}

bool Obj3D::getCollision()
{
	return collision;
}

glm::vec3 Obj3D::getDirection()
{
	return direction;
}

Mesh* Obj3D::getMesh() {
	return mesh;
}

glm::mat4 Obj3D::getTranslate()
{
	return translate;
}

glm::vec3 Obj3D::getEulerAngles()
{
	return eulerAngles;
}

string Obj3D::getName()
{
	return name;
}

glm::vec3 Obj3D::getPosition()
{
	return position;
}

glm::vec3 Obj3D::getOrigin()
{
	return origin;
}

glm::vec3 Obj3D::getScale()
{
	return scale;
}

glm::vec3* Obj3D::getGlobalPMin() {
	return globalPMin;
}

glm::vec3* Obj3D::getGlobalPMax() {
	return globalPMax;
}

GLuint Obj3D::getTexture() {
	return texture;
}

glm::vec3 Obj3D::getColor()
{
	return color;
}

bool Obj3D::isActive() {
	return active;
}
