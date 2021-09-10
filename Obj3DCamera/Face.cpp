#include "Face.h"


/*void Face::addVertex(int vertPos, int vertText, int vertNorm) {
	verts.push_back(vertPos);
	texts.push_back(vertText);
	norms.push_back(vertNorm);
	numVertices++;
}*/

void Face::addVertex(int vertPos, int vertColor) {
	verts.push_back(vertPos);
	colors.push_back(vertColor);
	numVertices++;
}

unsigned int Face::getNumOfVertices() {
	return numVertices;
}

vector<int> Face::getVerts() {
	return verts;
}

vector<int> Face::getColors() {
	return colors;
}

vector<int> Face::getNorms() {
	return norms;
}

vector<int> Face::getTexts() {
	return texts;
}
