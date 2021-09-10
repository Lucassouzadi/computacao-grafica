#include "ObjManager.h"
#include "System.h"

Mesh* ObjManager::readObj(string filename) {
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
		if (lineType.size() == 0 || lineType == "#") {
			continue;
		}
		if (lineType == "v") {
			float x, y, z;
			sline >> x >> y >> z;
			mesh->addVertex(new glm::vec3(x, y, z));
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
		else {   
			cout << "tipo não reconhecido: "<< lineType << endl;
		}
	}
	mesh->addGroup(group);
	return mesh;
}

void ObjManager::objToVAO(Obj3D* obj) {
	Mesh* mesh = obj->getMesh();
	for (Group* group : mesh->getGroups()) {
		vector <GLfloat> vPosition, vTexture, vNormal;
		vector <glm::vec3*> meshVerts = mesh->getVertex();
		vector <glm::vec2*> meshTexts = mesh->getTexts();
		vector <glm::vec3*> meshNorms = mesh->getNorms();
		for (Face* face : group->getFaces()) {
			vector<int> faceVerts = face->getVerts();
			vector<int> faceTexts = face->getTexts();
			vector<int> faceNorms = face->getNorms();
			for (int i = 0; i < face->getNumOfVertices(); i++) {	// TODO: tratar faces com 4 vértices
				glm::vec3* pos = meshVerts[faceVerts[i]];
				vPosition.push_back(pos->x);
				vPosition.push_back(pos->y);
				vPosition.push_back(pos->z);
				glm::vec2* text = meshTexts[faceTexts[i]];
				vTexture.push_back(text->x);
				vTexture.push_back(text->y);
				glm::vec3* norm = meshNorms[faceNorms[i]];
				vNormal.push_back(norm->x);
				vNormal.push_back(norm->y);
				vNormal.push_back(norm->z);
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

		// TODO: ver com o Tonietto se tem como colocar índices para mais de um atributo
		// GLuint EBO;
		// glGenBuffers(1, &EBO);
		// Index Buffer TODO: ver com o Tonietto se tem como usar índices pra mais de um atributo
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO
	}
}

Obj3D* ObjManager::getHardcoded2DHouse() {

	Face* wallFace1 = new Face();
	wallFace1->addVertex(0, 0, 0);
	wallFace1->addVertex(1, 0, 0);
	wallFace1->addVertex(3, 0, 0);
	Face* wallFace2 = new Face();
	wallFace2->addVertex(1, 0, 0);
	wallFace2->addVertex(2, 0, 0);
	wallFace2->addVertex(3, 0, 0);

	Face* doorFace1 = new Face();
	doorFace1->addVertex(4, 1, 0);
	doorFace1->addVertex(5, 1, 0);
	doorFace1->addVertex(7, 1, 0);
	Face* doorFace2 = new Face();
	doorFace2->addVertex(5, 1, 0);
	doorFace2->addVertex(6, 1, 0);
	doorFace2->addVertex(7, 1, 0);

	Face* roofFace = new Face();
	roofFace->addVertex(8, 0, 0);
	roofFace->addVertex(0, 1, 0);
	roofFace->addVertex(3, 1, 0);

	Group* wallGroup = new Group();
	wallGroup->addFace(wallFace1);
	wallGroup->addFace(wallFace2);
	Group* doorGroup = new Group();
	doorGroup->addFace(doorFace1);
	doorGroup->addFace(doorFace2);
	Group* roofGroup = new Group();
	roofGroup->addFace(roofFace);

	Mesh* mesh = new Mesh();
	mesh->addVertex(new glm::vec3(0.4f, 0.4f, 0.0f));	// top right
	mesh->addVertex(new glm::vec3(0.4f, -0.4f, 0.0f));	// bottom right
	mesh->addVertex(new glm::vec3(-0.4f, -0.4f, 0.0f));	// bottom left
	mesh->addVertex(new glm::vec3(-0.4f, 0.4f, 0.0f));	// top left

	mesh->addVertex(new glm::vec3(0.05f, -0.24f, -0.01f));	// door top right
	mesh->addVertex(new glm::vec3(0.05f, -0.4f, -0.01f));	// door bottom right
	mesh->addVertex(new glm::vec3(-0.05f, -0.4f, -0.01f));	// door bottom left
	mesh->addVertex(new glm::vec3(-0.05f, -0.24f, -0.01f));	// door top left

	mesh->addVertex(new glm::vec3(0.0f, 0.5f, 0.0f));	// roof

	mesh->addNormal(new glm::vec3(0.0f, 0.0f, 1.0f));	// normal global

	mesh->addTexture(new glm::vec2(1.0f, 0.0f));		// red
	mesh->addTexture(new glm::vec2(0.0f, 1.0f));		// green
	mesh->addGroup(wallGroup);
	mesh->addGroup(doorGroup);
	mesh->addGroup(roofGroup);

	Obj3D* obj = new Obj3D();
	obj->setName("Casinha da massa");
	obj->setMesh(mesh);
	obj->setTranslate(glm::mat4(1.0f));
	return obj;
}