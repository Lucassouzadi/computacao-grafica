#include "System.h"
#include "ObjManager.h"

using namespace std;

Obj3D* projectile;

glm::vec3 cameraPosition = glm::vec3(0.0f, 20.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float objSpeed = 20.0f;
float cameraSpeed = 22.0f;

//mouse configs
bool firstMouse = true;
float cameraXZAngle = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float cameraXYAngle = 0.0f;
float lastX;
float lastY;
float fov = 45.0f;

float mouseSensitivity = 0.8f;

float dot(glm::vec3 v1, glm::vec3 v2) {
	return min(max(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z, -1.0f), 1.0f);
}

glm::vec3 reflexao(glm::vec3 direcao, glm::vec3 normal) {
	glm::vec3 direcaoContraria = -direcao;
	float a = dot(normal, direcaoContraria);
	glm::vec3 novaDirecao = glm::vec3(2 * normal.x * a - direcaoContraria.x, 2 * normal.y * a - direcaoContraria.y, 2 * normal.z * a - direcaoContraria.z);
	return novaDirecao;
}

void resetProjectile() {
	projectile->setActive(false);
	projectile->setPosition(glm::vec3(0.0f, -10.0f, 0.0f));
	projectile->setDirection(glm::vec3(0.0f));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cout << "mouse right click" << endl;
		projectile->setActive(true);
		projectile->setPosition(cameraPosition + cameraFront * 2.0f);
		projectile->setDirection(cameraFront);
		projectile->setEulerAngles(glm::vec3(0.0f, -cameraXZAngle, cameraXYAngle));
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	cameraXZAngle += xoffset;
	cameraXYAngle += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (cameraXYAngle > 89.9f)
		cameraXYAngle = 89.9f;
	if (cameraXYAngle < -89.9f)
		cameraXYAngle = -89.9f;

	glm::vec3 front;
	front.x = cos(glm::radians(cameraXZAngle)) * cos(glm::radians(cameraXYAngle));
	front.y = sin(glm::radians(cameraXYAngle));
	front.z = sin(glm::radians(cameraXZAngle)) * cos(glm::radians(cameraXYAngle));

	glm::vec3 cameraCenter = cameraPosition + glm::normalize(front);
	cameraFront = glm::normalize(front);
}

System::System() {
	//mouse configs start in screen center
	float lastX = screenWidth / 2.0;
	float lastY = screenHeight / 2.0;
}

System::~System() {
}

int System::GLFWInit() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", nullptr, nullptr);

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		cout << "Failed to create GLFW Window" << endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		cout << "Failed no init GLEW." << endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	return EXIT_SUCCESS;

}

int System::OpenGLSetup()
{
	glEnable(GL_BLEND);	// Enables blending ( glBlendFunc )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	return EXIT_SUCCESS;
}

int System::SystemSetup()
{
	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader.Use();

	alphaLocation = glGetUniformLocation(coreShader.program, "alpha");
	modelLocation = glGetUniformLocation(coreShader.program, "model");
	projectionLocation = glGetUniformLocation(coreShader.program, "projection");
	viewLocation = glGetUniformLocation(coreShader.program, "view");
	textureLocation = glGetUniformLocation(coreShader.program, "texture1");
	hasTextureLocation = glGetUniformLocation(coreShader.program, "hasTexture");
	objColorLocation = glGetUniformLocation(coreShader.program, "objColor");
	ambientColorLocation = glGetUniformLocation(coreShader.program, "aColor");
	kaLocation = glGetUniformLocation(coreShader.program, "ka");
	kdLocation = glGetUniformLocation(coreShader.program, "kd");
	ksLocation = glGetUniformLocation(coreShader.program, "ks");
	shininessLocation = glGetUniformLocation(coreShader.program, "shininess");
	lightColorLocation = glGetUniformLocation(coreShader.program, "lightColor");
	lightPositionLocation = glGetUniformLocation(coreShader.program, "lightPosition");
	eyePositionLocation = glGetUniformLocation(coreShader.program, "eyePosition");

	return EXIT_SUCCESS;
}

void System::processInput(GLFWwindow* window, float elapsedSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 cameraFrontOnThefloor = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * elapsedSeconds * cameraFrontOnThefloor;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * elapsedSeconds * cameraFrontOnThefloor;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * elapsedSeconds * glm::normalize(glm::cross(cameraFrontOnThefloor, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += cameraSpeed * elapsedSeconds * glm::normalize(glm::cross(cameraFrontOnThefloor, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPosition.y += cameraSpeed * elapsedSeconds;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPosition.y -= cameraSpeed * elapsedSeconds;
}

float lenght(glm::vec3 vector) {
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

void System::drawObj(Obj3D* obj, GLenum mode, GLenum frontFace = GL_CCW) {
	glFrontFace(frontFace);
	glUniform3fv(objColorLocation, 1, glm::value_ptr((obj->getColor())));
	for (Group* group : obj->getMesh()->getGroups()) {
		if (group->getMaterial() == nullptr) {
			glUniform3fv(kaLocation, 1, glm::value_ptr(glm::vec3(0.16f)));
			glUniform3fv(kdLocation, 1, glm::value_ptr(glm::vec3(0.5f)));
			glUniform3fv(ksLocation, 1, glm::value_ptr(glm::vec3(0.5f)));
			glUniform1f(shininessLocation, 50.f);
		}
		else {
			glUniform3fv(kaLocation, 1, glm::value_ptr(group->getMaterial()->getKa()));
			glUniform3fv(kdLocation, 1, glm::value_ptr(group->getMaterial()->getKd()));
			glUniform3fv(ksLocation, 1, glm::value_ptr(group->getMaterial()->getKs()));
			glUniform1f(shininessLocation, group->getMaterial()->getNs());
		}

		glUniform1i(hasTextureLocation, group->getMaterial() != nullptr && group->getMaterial()->getTexture() != 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, group->getMaterial() != nullptr ? group->getMaterial()->getTexture() : 0);
		glBindVertexArray(group->getVAO());
		glDrawArrays(mode, 0, group->getNumVertices());
	}
	glBindVertexArray(0);
}

glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 centerOfRotationCoords, glm::vec3 eulerAngles, bool reverse = false) {
	glm::mat4 newTranslate, translateMatrix, yaw, pitch, roll, rotationMatrix;

	glm::vec3 centerOfRotation = centerOfRotationCoords - point;
	translateMatrix = glm::translate(glm::mat4(1.0f), point + centerOfRotation);

	yaw = glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	pitch = glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	roll = glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	if (!reverse)
		rotationMatrix = yaw * pitch * roll * glm::translate(glm::mat4(1.0f), -centerOfRotation);
	else
		rotationMatrix = glm::transpose(yaw * pitch * roll) * glm::translate(glm::mat4(1.0f), -centerOfRotation);

	glm::vec3 rotatedPos = (translateMatrix * rotationMatrix)[3];
	return rotatedPos;
}

float getBoundingSphereRadius(Obj3D* obj) {
	return length((*obj->getGlobalPMax() - *obj->getGlobalPMin()) * obj->getScale()) / 2.0f;
}

bool System::testCollisionSphereVSCube(Obj3D* projectile, Obj3D* obj, bool visilizeCollisionTesting, glm::vec3* reflectionNormal) {

	float projectileRadius = getBoundingSphereRadius(projectile);

	glm::vec3 objPosition = obj->getPosition();
	glm::vec3 objScale = obj->getScale();
	glm::vec3 unrotatedP0Coords = *obj->getGlobalPMin() * objScale + objPosition;
	glm::vec3 unrotatedP1Coords = *obj->getGlobalPMax() * objScale + objPosition;
	glm::vec3 objCenter = ((*obj->getGlobalPMin() + *obj->getGlobalPMax()) * objScale) / 2.0f;

	glm::vec3 projectileRotatedPos = rotatePoint(projectile->getPosition(), objPosition + obj->getOrigin(), obj->getEulerAngles(), true);

	glm::vec3 collisionTestCoord = projectileRotatedPos;

	if (collisionTestCoord.x < unrotatedP0Coords.x) {			// left   
		collisionTestCoord.x = unrotatedP0Coords.x;
	}
	else if (collisionTestCoord.x > unrotatedP1Coords.x) {	// right
		collisionTestCoord.x = unrotatedP1Coords.x;
	}

	if (collisionTestCoord.y < unrotatedP0Coords.y) {			// bottom 
		collisionTestCoord.y = unrotatedP0Coords.y;
	}
	else if (collisionTestCoord.y > unrotatedP1Coords.y) {	// top
		collisionTestCoord.y = unrotatedP1Coords.y;
	}

	if (collisionTestCoord.z < unrotatedP0Coords.z) {			// back
		collisionTestCoord.z = unrotatedP0Coords.z;
	}
	else if (collisionTestCoord.z > unrotatedP1Coords.z) {	// front
		collisionTestCoord.z = unrotatedP1Coords.z;
	}

	glm::vec3 collisionDistanceVector = projectileRotatedPos - collisionTestCoord;
	float distance = lenght(collisionDistanceVector);

	bool collidedWithGlobalBoundingBox = distance <= projectileRadius;

	if (visilizeCollisionTesting) {
		if (collidedWithGlobalBoundingBox) glUniform1f(alphaLocation, 0.2f);

		/* Desenha centro do Objeto (rotacionado) */
		glm::vec3 objCenterCoords = rotatePoint(objPosition + objCenter, objPosition + obj->getOrigin(), obj->getEulerAngles());
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), objCenterCoords)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha ponto de teste de colisão corrigido */
		glm::vec3 rotatedCollisionTestCoord = rotatePoint(collisionTestCoord, obj->getOrigin() + objPosition, obj->getEulerAngles());
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), rotatedCollisionTestCoord)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha centro do projetil */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), projectile->getPosition())));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha centro do projetil rotacionado em relação ao obj */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), projectileRotatedPos)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Raio */
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(projectile->getPosition() + glm::vec3(projectileRadius, 0.0f, 0.0f)))));
		//drawObj(auxCircle, GL_LINE_STRIP);

		/* Normal de reflexão (com rotação) */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), objPosition + objCenter + *reflectionNormal * getBoundingSphereRadius(obj))));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha bounding box global do objeto (rotacionada) */
		glm::vec3 boundingBoxDimensions = unrotatedP0Coords - unrotatedP1Coords;
		auxBox->setOrigin(obj->getOrigin() - objCenter);		// Origem de rota��o da BB tem que ser o centro do objeto
		auxBox->setPosition(obj->getPosition() + objCenter);	// Como (0, 0, 0) est� no centro da BB, precisa somar o centro do objeto
		auxBox->setEulerAngles(obj->getEulerAngles());
		auxBox->setScale(boundingBoxDimensions);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
		drawObj(auxBox, GL_LINE_STRIP);

		/* Desenha bounding box global do objeto (sem rotação) */
		//auxBox->setEulerAngles(glm::vec3(0.0f));
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
		//drawObj(auxBox, GL_LINE_STRIP);

		glUniform1f(alphaLocation, 1.0f);
	}

	/* Se houve colisão com BB global, testa colisão com cada um dos grupos */
	bool coollidedWithAnyGroup = false;
	if (collidedWithGlobalBoundingBox) {
		for (int groupIndex = 0; !coollidedWithAnyGroup && groupIndex < obj->getMesh()->getGroups().size(); groupIndex++) {
			Group* group = obj->getMesh()->getGroups()[groupIndex];

			glm::vec3 groupP0UnrotatedCoords = *group->getPMin() * objScale + objPosition;
			glm::vec3 groupP1UnrotatedCoords = *group->getPMax() * objScale + objPosition;

			glm::vec3 groupCollisionTestCoord = projectileRotatedPos;

			*reflectionNormal = glm::vec3(0.0f);

			if (groupCollisionTestCoord.x < groupP0UnrotatedCoords.x) {			// left   
				groupCollisionTestCoord.x = groupP0UnrotatedCoords.x;
				reflectionNormal->x -= 1.0f;
			}
			else if (groupCollisionTestCoord.x > groupP1UnrotatedCoords.x) {	// right
				groupCollisionTestCoord.x = groupP1UnrotatedCoords.x;
				reflectionNormal->x += 1.0f;
			}

			if (groupCollisionTestCoord.y < groupP0UnrotatedCoords.y) {			// bottom 
				groupCollisionTestCoord.y = groupP0UnrotatedCoords.y;
				reflectionNormal->y -= 1.0f;
			}
			else if (groupCollisionTestCoord.y > groupP1UnrotatedCoords.y) {	// top
				groupCollisionTestCoord.y = groupP1UnrotatedCoords.y;
				reflectionNormal->y += 1.0f;
			}

			if (groupCollisionTestCoord.z < groupP0UnrotatedCoords.z) {			// back
				groupCollisionTestCoord.z = groupP0UnrotatedCoords.z;
				reflectionNormal->z -= 1.0f;
			}
			else if (groupCollisionTestCoord.z > groupP1UnrotatedCoords.z) {	// front
				groupCollisionTestCoord.z = groupP1UnrotatedCoords.z;
				reflectionNormal->z += 1.0f;
			}

			glm::vec3 collisionDistanceVector = projectileRotatedPos - groupCollisionTestCoord;
			float distance = lenght(collisionDistanceVector);

			coollidedWithAnyGroup = distance <= projectileRadius;

			if (visilizeCollisionTesting) {
				/* Desenha bounding boxes do grupo */
				glm::vec3 boundingBoxDimensions = (groupP0UnrotatedCoords - groupP1UnrotatedCoords);
				glm::vec3 groupCenter = ((*group->getPMin() + *group->getPMax()) * objScale) / 2.0f;
				glm::vec3 groupPosition = obj->getPosition() + groupCenter;
				glm::vec3 groupRotationOrigin = obj->getOrigin() - groupCenter;	// Origem de rota��o da BB do grupo tem que estar ancorada na origem do objeto
				auxBox->setOrigin(groupRotationOrigin);
				auxBox->setPosition(groupPosition);
				auxBox->setEulerAngles(obj->getEulerAngles());
				auxBox->setScale(boundingBoxDimensions);
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
				drawObj(auxBox, GL_LINE_STRIP);

				/* Desenha centro do grupo (rotacionado) */
				glm::vec3 groupCenterCoords = rotatePoint(groupCenter + objPosition, obj->getOrigin() + objPosition, obj->getEulerAngles());
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), groupCenterCoords)));
				drawObj(auxCircle, GL_LINE_STRIP);

				/* Desenha P0 do grupo (rotacionado) */
				glm::vec3 rotatedGroupP0 = rotatePoint(groupP0UnrotatedCoords, obj->getOrigin() + objPosition, obj->getEulerAngles());
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), rotatedGroupP0)));
				drawObj(auxCircle, GL_LINE_STRIP);

				/* Desenha P1 do grupo (rotacionado) */
				glm::vec3 rotatedGroupP1 = rotatePoint(groupP1UnrotatedCoords, obj->getOrigin() + objPosition, obj->getEulerAngles());
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), rotatedGroupP1)));
				drawObj(auxCircle, GL_LINE_STRIP);

				/* Desenha ponto de teste de colisão corrigido */
				glm::vec3 rotatedCollisionTestCoord = rotatePoint(groupCollisionTestCoord, obj->getOrigin() + objPosition, obj->getEulerAngles());
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), rotatedCollisionTestCoord)));
				drawObj(auxCircle, GL_LINE_STRIP);
			}
		}
	}

	*reflectionNormal = glm::normalize(*reflectionNormal);
	*reflectionNormal = rotatePoint(*reflectionNormal, glm::vec3(0.0f), obj->getEulerAngles());

	return coollidedWithAnyGroup;
}

vector<glm::vec3*> readCurve() {
	vector<glm::vec3*> vertices;
	ifstream arq("objs/pista.curve");
	while (!arq.eof()) {
		string line;
		getline(arq, line);
		stringstream sline;
		sline << line;
		if (line == "" || line[0] == '#') continue;
		float x, y, z;
		sline >> x >> y >> z;
		glm::vec3* vector = new glm::vec3(x, y, z);

		vertices.push_back(vector);
	}

	return vertices;
}

void System::Run() {
	/* Setup da cena */
	vector<Obj3D*> objs = vector<Obj3D*>();

	ObjManager* objManager = new ObjManager();

	vector<glm::vec3*> curveVertices = readCurve();

	//Obj3D* toonLink2 = toonLink1->copy();
	//toonLink2->setName("ToonLink2");
	//objs.push_back(toonLink2);

	Obj3D* table = objManager->readObj("objs/mesa01.obj");
	objManager->loadMaterials(table);
	table->setName("table");
	table->setScale(glm::vec3(1.5f));
	table->setPosition(glm::vec3(-35.0f, 0.0f, -10.0f));
	objs.push_back(table);

	Obj3D* cenaPaintball = objManager->readObj("objs/cenaPaintball.obj");
	objManager->loadMaterials(cenaPaintball);
	cenaPaintball->setName("CenaPaintball");
	cenaPaintball->setScale(glm::vec3(5.0f));
	cenaPaintball->setColor(glm::vec3(0.6f, 0.0f, 0.1f));
	objs.push_back(cenaPaintball);

	Obj3D* libertyStatue = objManager->readObj("objs/LibertStatue.obj");
	objManager->loadMaterials(libertyStatue);
	libertyStatue->setColor(glm::vec3(0.80f, 0.64f, 0.25f));
	libertyStatue->setName("LibertStatue");
	libertyStatue->setPosition(glm::vec3(30.0f, 0.0f, 0.0f));
	libertyStatue->setScale(glm::vec3(35.0f, 35.0f, 35.0f));
	objs.push_back(libertyStatue);

	Obj3D* pista = objManager->readObj("objs/pista.obj");
	objManager->loadMaterials(pista);
	pista->setColor(glm::vec3(0.80f, 0.64f, 0.25f));
	pista->setName("pista");
	pista->setScale(glm::vec3(70.0f));
	float courseLowestPoint = pista->getGlobalPMin()->y * pista->getScale().y;
	pista->setPosition(glm::vec3(-70.0f, 1.0f-(courseLowestPoint), 0.0f));
	objs.push_back(pista);

	int carPoint = 0;
	Obj3D* carrito = objManager->readObj("objs/free_car_001.obj");
	objManager->loadMaterials(carrito);
	carrito->setCollision(true);
	carrito->setName("free_car_001");
	carrito->setScale(glm::vec3(.8f));
	carrito->setPosition(pista->getPosition() + * curveVertices[carPoint] * pista->getScale());
	objs.push_back(carrito);

	auxBox = objManager->getHardcodedCube(0.5f);
	auxCircle = objManager->get2DCircle(0.5f, 32);
	auxSphere = objManager->readObj("objs/sphere.obj");
	objManager->loadMaterials(auxSphere);
	auxSphere->setScale(glm::vec3(0.7f));
	Obj3D* aimObj = objManager->getCross(0.003f, 0.06f);

	projectile = auxBox->copy();
	projectile->setScale(glm::vec3(3.0f));
	projectile->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
	resetProjectile();

	glUniform1i(textureLocation, 0);

	/* Ambient Lighting setup */
	glm::vec3 ambientColor = glm::vec3(1.0f);
	glUniform3fv(ambientColorLocation, 1, glm::value_ptr(ambientColor));

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		processInput(window, elapsedSeconds);


		/* Diffuse Lighting setup */
		glm::vec3 lightPosition = glm::vec3(sin(currentSeconds) * 40, 100.0f, cos(currentSeconds) * 20);
		//glm::vec3 lightPosition = glm::vec3(10.0f, 35.0f, 0.0f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));

		/* Specular Lighting setup */
		glUniform3fv(eyePositionLocation, 1, glm::value_ptr(cameraPosition));

		/* Projection e view */
		glm::mat4 projection, view;
		projection = glm::perspective(glm::radians(90.0f), (float)screenWidth / (float)screenHeight, 0.01f, 1000.0f);
		glm::vec3 eye = cameraPosition;
		glm::vec3 front = cameraPosition + cameraFront;
		glm::vec3 up = cameraUp;
		view = glm::lookAt(eye, front, up);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

		/* Desenha mira */
		//aimObj->setPosition(front);
		//aimObj->setEulerAngles(glm::vec3(0.0f, -cameraXZAngle, cameraXYAngle));
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(aimObj->getTranslate()));
		//drawObj(aimObj, GL_TRIANGLES);

		/* Desloca projétil */
		if (projectile->isActive()) {
			glm::vec3 projectileDelta = glm::vec3(elapsedSeconds * objSpeed) * projectile->getDirection();
			projectile->setPosition(projectile->getPosition() + projectileDelta);
		}

		for (int objectIndex = 0; objectIndex < objs.size(); objectIndex++) {
			Obj3D* obj = objs[objectIndex];
			if (!obj->isActive()) continue;

			/* Rotaciona objeto */
			//if (obj->getName() != "CenaPaintball")
				//obj->setEulerAngles(glm::vec3(currentSeconds * 30));

			/* Desenha objeto */
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obj->getTranslate()));
			drawObj(obj, GL_TRIANGLES);

			/* Teste de colisão */
			if (projectile->isActive() && obj->getCollision()) {

				glm::vec3* reflectionNormal = new glm::vec3;
				bool collidedWithCurrentObject = testCollisionSphereVSCube(projectile, obj, false, reflectionNormal);

				if (collidedWithCurrentObject) {
					obj->setActive(false);
					resetProjectile();
				}

			}
		}

		/* Desenha projétil */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(projectile->getTranslate()));
		drawObj(projectile, GL_TRIANGLES, GL_CW);
		drawObj(projectile, GL_LINE_STRIP);

		/* Animação do carro */ // TODO
		glm::vec3 previousPoint = *curveVertices[carPoint % curveVertices.size()];
		glm::vec3 currentPoint = *curveVertices[++carPoint % curveVertices.size()];

		/* Rotação do carro */ // TODO
		glm::vec3 directionXZ = glm::normalize(glm::vec3(previousPoint.x, 0.0f, previousPoint.z) - glm::vec3(currentPoint.x, 0.0f, currentPoint.z));
		
		float angleXZ = glm::degrees(acos(dot(directionXZ, glm::vec3(1.0f, 0.0f, 0.0f))));

		if (directionXZ.z < 0) {
			angleXZ -= 90.0f;
		} else {
			angleXZ = 270.0f - angleXZ;
		}
		glm::vec3 directionYZ = glm::normalize(previousPoint - currentPoint);

		float angleYZ = glm::degrees(acos(dot(directionYZ, glm::normalize(glm::vec3(directionYZ.x, 0.0f, directionYZ.z)))));
		
		if (directionYZ.y < 0) {
			angleYZ = -angleYZ;
		}

		carrito->setEulerAngles(glm::vec3(angleYZ, angleXZ, 0.0f));
		carrito->setPosition(pista->getPosition() + currentPoint * pista->getScale());

		/* Desenha ponto de luz */
		auxSphere->setColor(lightColor);
		auxSphere->setPosition(lightPosition);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxSphere->getTranslate()));
		glUniform3fv(ambientColorLocation, 1, glm::value_ptr(glm::vec3(1.0f)));
		drawObj(auxSphere, GL_TRIANGLES);
		drawObj(auxSphere, GL_LINE_STRIP);
		glUniform3fv(ambientColorLocation, 1, glm::value_ptr(ambientColor));

		glfwSwapBuffers(window);
	}

}

void System::Finish() {
	coreShader.Delete();

	glfwTerminate();
}