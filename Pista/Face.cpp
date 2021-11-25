#include "Face.h"


void Face::addVertex(int vertPos, int vertText, int vertNorm) {
	verts.push_back(vertPos);
	texts.push_back(vertText);
	norms.push_back(vertNorm);
	numVertices++;
}

unsigned int Face::getNumOfVertices() {
	return numVertices;
}

vector<int> Face::getVerts() {
	return verts;
}

vector<int> Face::getNorms() {
	return norms;
}

vector<int> Face::getTexts() {
	return texts;
}
