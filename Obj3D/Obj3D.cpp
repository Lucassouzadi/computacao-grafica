#include "Obj3D.h"


void Obj3D::setCollision(bool direction)
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

void Obj3D::setTranslate(glm::mat4 translate)
{
	this->translate = translate;
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

string Obj3D::getName()
{
	return name;
}
