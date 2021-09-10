#include "ObjManager.h"
#include "System.h"


void ObjManager::objToVAO(Obj3D* obj) {
	Mesh* mesh = obj->getMesh();
	for (Group* group : mesh->getGroups()) {
		vector < GLfloat > vPosition;
		vector < GLfloat > vColor;
		for (Face* face : group->getFaces()) {
			vector<int> verts = face->getVerts();
			vector<int> colors = face->getColors();
			for (int i = 0; i < face->getNumOfVertices(); i++) {
				glm::vec3* pos = mesh->getVertex()[verts[i]];
				vPosition.push_back(pos->x);
				vPosition.push_back(pos->y);
				vPosition.push_back(pos->z);
				glm::vec3* col = mesh->getColors()[colors[i]];
				vColor.push_back(col->x);
				vColor.push_back(col->y);
				vColor.push_back(col->z);
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

		// 4) criar um VBO para vns (por enquanto é a cor)
		GLuint colorVBO;
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vColor.size(), vColor.data(), GL_STATIC_DRAW);

		// TODO: 5) criar um VBO para vts

		// 6) definir layout e atributos do VAO     
		//    para leitura dos VBOs
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

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
	wallFace1->addVertex(0, 0);
	wallFace1->addVertex(1, 0);
	wallFace1->addVertex(3, 0);
	Face* wallFace2 = new Face();
	wallFace2->addVertex(1, 1);
	wallFace2->addVertex(2, 1);
	wallFace2->addVertex(3, 1);

	Face* doorFace1 = new Face();
	doorFace1->addVertex(4, 3);
	doorFace1->addVertex(5, 3);
	doorFace1->addVertex(7, 3);
	Face* doorFace2 = new Face();
	doorFace2->addVertex(5, 3);
	doorFace2->addVertex(6, 3);
	doorFace2->addVertex(7, 3);

	Face* roofFace = new Face();
	roofFace->addVertex(8, 2);
	roofFace->addVertex(0, 2);
	roofFace->addVertex(3, 2);

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

	mesh->addVertex(new glm::vec3(0.05f, -0.24f, 0.01f));	// door top right
	mesh->addVertex(new glm::vec3(0.05f, -0.4f, 0.01f));	// door bottom right
	mesh->addVertex(new glm::vec3(-0.05f, -0.4f, 0.01f));	// door bottom left
	mesh->addVertex(new glm::vec3(-0.05f, -0.24f, 0.01f));	// door top left

	mesh->addVertex(new glm::vec3(0.0f, 0.5f, 0.0f));	// roof

	mesh->addColor(new glm::vec3(1.0f, 0.0f, 0.0f));	// red
	mesh->addColor(new glm::vec3(0.0f, 1.0f, 0.0f));	// blue
	mesh->addColor(new glm::vec3(0.0f, 0.0f, 1.0f));	// green
	mesh->addColor(new glm::vec3(0.0f, 0.0f, 0.0f));	// black
	mesh->addGroup(wallGroup);
	mesh->addGroup(doorGroup);
	mesh->addGroup(roofGroup);

	Obj3D* obj = new Obj3D();
	obj->setName("Casinha da massa");
	obj->setMesh(mesh);
	obj->setTranslate(glm::mat4(1.0f));
	return obj;
}