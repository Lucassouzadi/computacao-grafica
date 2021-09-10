#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <vector>

using namespace std;

class Face {
	private:
		vector<int> verts, norms, texts;
		unsigned int numVertices;
	public:
		void addVertex(int vertPos, int vertText, int vertNorm);
		unsigned int getNumOfVertices();
		vector<int> getVerts();
		vector<int> getNorms();
		vector<int> getTexts();
};