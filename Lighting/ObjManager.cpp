#include "ObjManager.h"
#include "System.h"
#include <limits>

Obj3D* ObjManager::readObj(string filename) {
	Obj3D* obj = new Obj3D();
	Mesh* mesh = new Mesh();
	Group* group = new Group();
	bool firstGroup = true;
	ifstream arq(filename);
	while (!arq.eof()) {
		string line;
		getline(arq, line);
		stringstream sline;
		sline << line;
		string lineType;
		sline >> lineType;
		if (lineType == "" || lineType[0] == '#') continue;
		if (lineType == "v") {
			float x, y, z;
			sline >> x >> y >> z;
			glm::vec3* vector = new glm::vec3(x, y, z);

			if (vector->x > obj->getGlobalPMax()->x) obj->getGlobalPMax()->x = vector->x;
			if (vector->y > obj->getGlobalPMax()->y) obj->getGlobalPMax()->y = vector->y;
			if (vector->z > obj->getGlobalPMax()->z) obj->getGlobalPMax()->z = vector->z;
			if (vector->x < obj->getGlobalPMin()->x) obj->getGlobalPMin()->x = vector->x;
			if (vector->y < obj->getGlobalPMin()->y) obj->getGlobalPMin()->y = vector->y;
			if (vector->z < obj->getGlobalPMin()->z) obj->getGlobalPMin()->z = vector->z;

			mesh->addVertex(vector);
		}
		else if (lineType == "vn") {
			float nx, ny, nz;
			sline >> nx >> ny >> nz;
			mesh->addNormal(new glm::vec3(nx, ny, nz));
		} 
		else if (lineType == "vt") {
			float tx, ty;
			sline >> tx >> ty;
			mesh->addTexture(new glm::vec2(tx, ty));
		} 
		else if(lineType == "f") {
			Face* face = new Face();    
			string token;
			while (sline >> token) {
				stringstream stoken;
				stoken << token;
				string aux;

				int i = 0;
				int v = 0;
				int t = 0;
				int n = 0;

				while (getline(stoken, aux, '/')) {
					if (aux == "") { i++; continue; }
					int x = stoi(aux) - 1;
					switch (i++) {
						case 0: v = x; break;
						case 1: t = x; break;
						case 2: n = x; break;
					}
				}
				face->addVertex(v, t, n);
			}
			group->addFace(face);
		} 
		else if (lineType == "g") {
			string groupName;
			sline >> groupName;
			if (firstGroup) {
				firstGroup = false;
			} else {
				mesh->addGroup(group);
				group = new Group();
			}   
			group->setName(groupName);
		}
		else if (lineType == "usemtl") {
			string materialId;
			sline >> materialId;
			group->setMaterial(new Material(materialId));
		}
		else if (lineType == "mtllib") {
			string materialFileName;
			sline >> materialFileName;
			obj->setMaterialFileName(materialFileName);
		}
		else {   
			cout << "[" << filename << "] tipo não reconhecido: " << lineType << endl;
		}
	}
	if (mesh->getTexts().size() == 0) {
		mesh->addTexture(new glm::vec2(0.0f));
	}
	if (mesh->getNorms().size() == 0) {
		mesh->addNormal(new glm::vec3(0.0f));
	}
	mesh->addGroup(group);
	obj->setMesh(mesh);
	this->objToVAO(obj);
	return obj;
}

void ObjManager::loadMaterials(Obj3D* obj) {
	string fileName = obj->getMaterialFileName();
	if (fileName == "") {
		return;
	}

	ifstream arq("objs/" + fileName);
	Material* currentMaterial = nullptr;
	while (!arq.eof()) {
		string line;
		getline(arq, line);
		stringstream sline;
		sline << line;
		string lineType;
		sline >> lineType;
		if (lineType == "" || lineType[0] == '#') continue;
		if (lineType == "newmtl") {
			string materialId;
			sline >> materialId;
			currentMaterial = obj->getMaterialById(materialId);
		}
		if (currentMaterial == nullptr) continue;
		if (lineType == "ka") {
			float x, y, z;
			sline >> x >> y >> z;
			currentMaterial->setKa(glm::vec3(x, y, z));
		}
		else if (lineType == "kd") {
			float x, y, z;
			sline >> x >> y >> z;
			currentMaterial->setKd(glm::vec3(x, y, z));
		}
		else if (lineType == "ks") {
			float x, y, z;
			sline >> x >> y >> z;
			currentMaterial->setKs(glm::vec3(x, y, z));
		}
		else if (lineType == "Ns") {
			float ns;
			sline >> ns;
			currentMaterial->setNs(ns);
		}
		else if (lineType == "map_Kd") {
			string mapKd;
			sline >> mapKd;
			currentMaterial->setMapKd(mapKd);
		}
		else {
			cout << "[" << fileName << "] tipo não reconhecido: " << lineType << endl;
		}
	}
}

void addTriangle(Group* group, Mesh* mesh, Face* face, vector <GLfloat>* vPosition, vector <GLfloat>* vTexture, vector <GLfloat>* vNormal, int v1, int v2, int v3) {
	vector <glm::vec3*> meshVerts = mesh->getVertex();
	vector <glm::vec2*> meshTexts = mesh->getTexts();
	vector <glm::vec3*> meshNorms = mesh->getNorms();
	int vertIndexes[3] = { v1, v2, v3 };
	for (int currentVertIndex : vertIndexes) {
		vector<int> faceVerts = face->getVerts();
		vector<int> faceTexts = face->getTexts();
		vector<int> faceNorms = face->getNorms();
		glm::vec3* pos = meshVerts[faceVerts[currentVertIndex]];
		vPosition->push_back(pos->x);
		vPosition->push_back(pos->y);
		vPosition->push_back(pos->z);
		glm::vec2* text = meshTexts[faceTexts[currentVertIndex]];
		vTexture->push_back(text->x);
		vTexture->push_back(text->y);
		glm::vec3* norm = meshNorms[faceNorms[currentVertIndex]];
		vNormal->push_back(norm->x);
		vNormal->push_back(norm->y);
		vNormal->push_back(norm->z);

		if (pos->x > group->getPMax()->x) group->getPMax()->x = pos->x;
		if (pos->y > group->getPMax()->y) group->getPMax()->y = pos->y;
		if (pos->z > group->getPMax()->z) group->getPMax()->z = pos->z;
		if (pos->x < group->getPMin()->x) group->getPMin()->x = pos->x;
		if (pos->y < group->getPMin()->y) group->getPMin()->y = pos->y;
		if (pos->z < group->getPMin()->z) group->getPMin()->z = pos->z;
	}
	group->setNumVertices(group->getNumVertices() + 3);
}

void ObjManager::objToVAO(Obj3D* obj) {
	Mesh* mesh = obj->getMesh();
	for (Group* group : mesh->getGroups()) {
		group->setNumVertices(0);
		vector <GLfloat> vPosition, vTexture, vNormal;
		vector <glm::vec3*> meshVerts = mesh->getVertex();
		vector <glm::vec2*> meshTexts = mesh->getTexts();
		vector <glm::vec3*> meshNorms = mesh->getNorms();
		for (Face* face : group->getFaces()) {
			if (face->getNumOfVertices() == 3) {
				addTriangle(group, mesh, face, &vPosition, &vTexture, &vNormal, 0, 1, 2);
			}
			else if (face->getNumOfVertices() == 4) {
				addTriangle(group, mesh, face, &vPosition, &vTexture, &vNormal, 0, 1, 2);
				addTriangle(group, mesh, face, &vPosition, &vTexture, &vNormal, 2, 3, 0);
			}
			else {
				printf("Faces com %i vértices não suportada", face->getNumOfVertices());
			}
		}

		// 1) criar VAO para o grupo
		GLuint groupVAO;
		glGenVertexArrays(1, &groupVAO);
		glBindVertexArray(groupVAO);

		// 2) associar VAO criado com grupo
		group->setVAO(groupVAO);

		// 3) criar um VBO para vs
		GLuint positionVBO;
		glGenBuffers(1, &positionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vPosition.size(), vPosition.data(), GL_STATIC_DRAW);

		// 4) criar um VBO para vns
		GLuint normalVBO;
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vNormal.size(), vNormal.data(), GL_STATIC_DRAW);

		// 5) criar um VBO para vts
		GLuint textureVBO;
		glGenBuffers(1, &textureVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vTexture.size(), vTexture.data(), GL_STATIC_DRAW);

		// 6) definir layout e atributos do VAO     
		//    para leitura dos VBOs
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // Unbind VAO
	}
}

Obj3D* ObjManager::getHardcodedCube(GLfloat size) {

	Face* t1face1 = new Face();
	t1face1->addVertex(0, 0, 0);
	t1face1->addVertex(1, 0, 1);
	t1face1->addVertex(2, 0, 2);
	Face* t2face1 = new Face();
	t2face1->addVertex(2, 0, 2);
	t2face1->addVertex(3, 0, 3);
	t2face1->addVertex(0, 0, 0);
	Face* t1face2 = new Face();
	t1face2->addVertex(0, 0, 0);
	t1face2->addVertex(4, 0, 4);
	t1face2->addVertex(5, 0, 5);
	Face* t2face2 = new Face();
	t2face2->addVertex(5, 0, 5);
	t2face2->addVertex(1, 0, 1);
	t2face2->addVertex(0, 0, 0);
	Face* t1face3 = new Face();
	t1face3->addVertex(4, 0, 4);
	t1face3->addVertex(7, 0, 7);
	t1face3->addVertex(6, 0, 6);
	Face* t2face3 = new Face();
	t2face3->addVertex(6, 0, 6);
	t2face3->addVertex(5, 0, 5);
	t2face3->addVertex(4, 0, 4);
	Face* t1face4 = new Face();
	t1face4->addVertex(7, 0, 7);
	t1face4->addVertex(3, 0, 3);
	t1face4->addVertex(2, 0, 2);
	Face* t2face4 = new Face();
	t2face4->addVertex(2, 0, 2);
	t2face4->addVertex(6, 0, 6);
	t2face4->addVertex(7, 0, 7);
	Face* t1face5 = new Face();
	t1face5->addVertex(4, 0, 4);
	t1face5->addVertex(0, 0, 0);
	t1face5->addVertex(3, 0, 3);
	Face* t2face5 = new Face();
	t2face5->addVertex(3, 0, 3);
	t2face5->addVertex(7, 0, 7);
	t2face5->addVertex(4, 0, 4);
	Face* t1face6 = new Face();
	t1face6->addVertex(1, 0, 1);
	t1face6->addVertex(5, 0, 5);
	t1face6->addVertex(6, 0, 6);
	Face* t2face6 = new Face();
	t2face6->addVertex(6, 0, 6);
	t2face6->addVertex(2, 0, 2);
	t2face6->addVertex(1, 0, 1);

	Group* face1 = new Group();
	face1->addFace(t1face1);
	face1->addFace(t2face1);
	Group* face2 = new Group();
	face2->addFace(t1face2);
	face2->addFace(t2face2);
	Group* face3 = new Group();
	face3->addFace(t1face3);
	face3->addFace(t2face3);
	Group* face4 = new Group();
	face4->addFace(t1face4);
	face4->addFace(t2face4);
	Group* face5 = new Group();
	face5->addFace(t1face5);
	face5->addFace(t2face5);
	Group* face6 = new Group();
	face6->addFace(t1face6);
	face6->addFace(t2face6);

	Mesh* mesh = new Mesh();
	mesh->addVertex(new glm::vec3(size, size, size));		// front top right
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
	mesh->addVertex(new glm::vec3(size, -size, size));		// front bottom right
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f))));
	mesh->addVertex(new glm::vec3(-size, -size, size));		// front bottom left
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f))));
	mesh->addVertex(new glm::vec3(-size, size, size));		// front top left
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f))));
	mesh->addVertex(new glm::vec3(size, size, -size));		// back top right
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f))));
	mesh->addVertex(new glm::vec3(size, -size, -size));		// back bottom right
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f))));
	mesh->addVertex(new glm::vec3(-size, -size, -size));	// back bottom left
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f))));
	mesh->addVertex(new glm::vec3(-size, size, -size));		// back top left
	mesh->addNormal(new glm::vec3(glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f))));

	mesh->addTexture(new glm::vec2(0.0f, 0.0f));

	mesh->addGroup(face1);
	mesh->addGroup(face2);
	mesh->addGroup(face3);
	mesh->addGroup(face4);
	mesh->addGroup(face5);
	mesh->addGroup(face6);

	Obj3D* obj = new Obj3D();
	obj->setGlobalPMax(new glm::vec3(size));
	obj->setGlobalPMin(new glm::vec3(-size));
	obj->setName("Cubo da massa");
	obj->setMesh(mesh);
	this->objToVAO(obj);
	return obj;
}


Obj3D* ObjManager::get2DCircle(GLfloat radius, int vertices) {

	GLfloat twicePi = 2 * 3.14159265;

	vector <GLfloat> vPosition, vNormal;

	for (int i = 0; i <= vertices; i++) {
		float angle = (i * twicePi / vertices);
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		vPosition.push_back(x);
		vPosition.push_back(y);
		vPosition.push_back(0.0f);
		vNormal.push_back(1.0f);
		vNormal.push_back(1.0f);
		vNormal.push_back(1.0f);
	}

	GLuint circleVAO;
	glGenVertexArrays(1, &circleVAO);
	glBindVertexArray(circleVAO);

	GLuint positionVBO;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vPosition.size(), vPosition.data(), GL_STATIC_DRAW);

	GLuint normalVBO;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vNormal.size(), vNormal.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	Group* circle = new Group();
	circle->setVAO(circleVAO);
	circle->setNumVertices(vertices + 1);

	Mesh* mesh = new Mesh();
	mesh->addGroup(circle);

	Obj3D* obj = new Obj3D();
	obj->setGlobalPMax(new glm::vec3(abs(radius)));
	obj->setGlobalPMin(new glm::vec3(-abs(radius)));
	obj->setName("circulo");
	obj->setMesh(mesh);
	return obj;
}


Obj3D* ObjManager::getCross(GLfloat width, GLfloat length) {
	width /= 2;
	length /= 2;

	Face* face1 = new Face();
	face1->addVertex(0, 0, 0);
	face1->addVertex(1, 0, 0);
	face1->addVertex(2, 0, 0);
	face1->addVertex(3, 0, 0);
	Face* face2 = new Face();
	face2->addVertex(4, 0, 0);
	face2->addVertex(5, 0, 0);
	face2->addVertex(6, 0, 0);
	face2->addVertex(7, 0, 0);

	Group* group = new Group();
	group->addFace(face1);
	group->addFace(face2);

	Mesh* mesh = new Mesh();
	mesh->addVertex(new glm::vec3(0.0f, -width, length));		// 0
	mesh->addVertex(new glm::vec3(0.0f, width, length));		// 1
	mesh->addVertex(new glm::vec3(0.0f, width, -length));		// 2
	mesh->addVertex(new glm::vec3(0.0f, -width, -length));		// 3
	mesh->addVertex(new glm::vec3(0.0f, -length, width));		// 4
	mesh->addVertex(new glm::vec3(0.0f, length, width));		// 5
	mesh->addVertex(new glm::vec3(0.0f, length, -width));		// 6
	mesh->addVertex(new glm::vec3(0.0f, -length, -width));		// 7

	mesh->addTexture(new glm::vec2(0.0f));
	mesh->addNormal(new glm::vec3(1.0f));

	mesh->addGroup(group);

	Obj3D* obj = new Obj3D();
	obj->setGlobalPMax(new glm::vec3(length));
	obj->setGlobalPMin(new glm::vec3(-length));
	obj->setName("cruz");
	obj->setMesh(mesh);
	this->objToVAO(obj);
	return obj;
}