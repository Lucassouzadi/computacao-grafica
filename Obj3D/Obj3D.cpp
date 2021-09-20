#include "Obj3D.h"
#include <glm\gtc\matrix_transform.hpp>

Obj3D::Obj3D() {
	this->position = glm::vec3(0.0f);
	this->eulerAngles = glm::vec3(0.0f);
	this->scale = glm::vec3(1.0f);
	this->computeTranslate();
}

void Obj3D::computeTranslate()
{
	glm::mat4 newTranslate = glm::mat4(1.0f);
	newTranslate = glm::translate(newTranslate, this->position);
	newTranslate = glm::rotate(newTranslate, glm::radians(this->eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	newTranslate = glm::rotate(newTranslate, glm::radians(this->eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	newTranslate = glm::rotate(newTranslate, glm::radians(this->eulerAngles.z), glm::vec3(1.0f, 0.0f, 1.0f));
	newTranslate = glm::scale(newTranslate, this->scale);
	this->translate = newTranslate;
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

void Obj3D::setMesh(Mesh* mesh)
{
	this->mesh = mesh;
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

glm::vec3 Obj3D::getScale()
{
	return scale;
}