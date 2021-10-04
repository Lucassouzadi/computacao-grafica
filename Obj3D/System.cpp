#include "System.h"
#include "ObjManager.h"

using namespace std;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 20.0f;

//mouse configs
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX;
float lastY;
float fov = 45.0f;

float mouseSensitivity = 0.8f;

float dot(glm::vec3 v1, glm::vec3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

glm::vec3 reflexao(glm::vec3 direcao, glm::vec3 normal) {
	glm::vec3 direcaoContraria = -direcao;
	float a = dot(normal, direcaoContraria);
	glm::vec3 novaDirecao = glm::vec3(2 * normal.x * a - direcaoContraria.x, 2 * normal.y * a - direcaoContraria.y, 2 * normal.z * a - direcaoContraria.z);
	return novaDirecao;
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

	window = glfwCreateWindow(WIDTH, HEIGHT, "Obj3D", nullptr, nullptr);

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		cout << "Failed to create GLFW Window" << endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

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

	return EXIT_SUCCESS;
}

void System::ProcessInput(GLFWwindow* window, float elapsedSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * elapsedSeconds * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * elapsedSeconds * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * elapsedSeconds;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * elapsedSeconds;
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

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 cameraCenter = cameraPosition + glm::normalize(front);
	cameraFront = glm::normalize(front);
}

float lenght(glm::vec3 vector) {
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

glm::vec3 normalize(glm::vec3 vector) {
	float vectorLength = length(vector);
	return vector / vectorLength;
}

void drawObj(Obj3D* obj, GLenum mode) {
	for (Group* group : obj->getMesh()->getGroups()) {
		glBindVertexArray(group->getVAO());
		glDrawArrays(mode, 0, group->getNumVertices());
	}
	glBindVertexArray(0);
}

glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 centerOfRotationCoords, glm::vec3 eulerAngles, bool reverse) {
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

bool System::testCollisionSphereVSCube(Obj3D* projectile, Obj3D* obj, bool visilizeCollisionTesting) {

	glm::vec3 objPosition = obj->getPosition();
	glm::vec3 objScale = obj->getScale();
	glm::vec3 objPmax = *obj->getGlobalPMax();
	glm::vec3 objPmin = *obj->getGlobalPMin();
	glm::vec3 unrotatedP0 = objPmin * objScale + objPosition;
	glm::vec3 unrotatedP1 = objPmax * objScale + objPosition;

	glm::vec3 projectileRotatedPos = rotatePoint(projectile->getPosition(), objPosition + obj->getOrigin(), obj->getEulerAngles(), true);

	glm::vec3 collisionTestCoord = projectileRotatedPos;

	if (collisionTestCoord.x < unrotatedP0.x)		// left   
		collisionTestCoord.x = unrotatedP0.x;
	else if (collisionTestCoord.x > unrotatedP1.x)	// right
		collisionTestCoord.x = unrotatedP1.x;

	if (collisionTestCoord.y < unrotatedP0.y)        // top
		collisionTestCoord.y = unrotatedP0.y;
	else if (collisionTestCoord.y > unrotatedP1.y)	// bottom
		collisionTestCoord.y = unrotatedP1.y;

	if (collisionTestCoord.z < unrotatedP0.z)		// back
		collisionTestCoord.z = unrotatedP0.z;
	else if (collisionTestCoord.z > unrotatedP1.z)	// front
		collisionTestCoord.z = unrotatedP1.z;

	
	//glm::vec3 collisionDistanceVector = projectile->getPosition() - rotatedCollisionTestCoord;
	glm::vec3 collisionDistanceVector = projectileRotatedPos - collisionTestCoord;
	float distance = lenght(collisionDistanceVector);

	bool collidedWithCurrentObject = distance <= getBoundingSphereRadius(projectile);

	if (visilizeCollisionTesting){	
		if (collidedWithCurrentObject) glUniform1f(alphaLocation, 0.2f);

		/* Desenha centro do Objeto (rotacionado) */
		glm::vec3 objCenter = ((objPmax + objPmin) * objScale) / 2.0f;
		glm::vec3 objCenterCoords = rotatePoint(objPosition + objCenter, objPosition + obj->getOrigin(), obj->getEulerAngles(), false);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), objCenterCoords)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha ponto de teste de colisão rotacionado */
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), collisionTestCoord)));
		//drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha ponto de teste de colisão corrigido */
		glm::vec3 rotatedCollisionTestCoord = rotatePoint(collisionTestCoord, obj->getOrigin() + objPosition, obj->getEulerAngles(), false);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), rotatedCollisionTestCoord)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha centro do projetil */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), projectile->getPosition())));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha centro do projetil rotacionado em relação ao obj */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), projectileRotatedPos)));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Raio */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(projectile->getPosition().x + getBoundingSphereRadius(projectile), projectile->getPosition().y, projectile->getPosition().z))));
		drawObj(auxCircle, GL_LINE_STRIP);

		/* Desenha bounding box global do objeto (rotacionada) */
		glm::vec3 boundingBoxDimensions = (objPmax - objPmin) * obj->getScale();
		auxBox->setOrigin(obj->getOrigin() - objCenter);		// Origem de rota��o da BB tem que ser o centro do objeto
		auxBox->setPosition(obj->getPosition() + objCenter);	// Como (0, 0, 0) est� no centro da BB, precisa somar o centro do objeto
		auxBox->setEulerAngles(obj->getEulerAngles());
		auxBox->setScale(boundingBoxDimensions);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
		drawObj(auxBox, GL_LINE_STRIP);

		/* Desenha bounding box global do objeto (sem rotação) */
		auxBox->setEulerAngles(glm::vec3(0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
		drawObj(auxBox, GL_LINE_STRIP);

		/* Desenha bounding boxes de cada um dos grupos do objeto */
		for (Group* group : obj->getMesh()->getGroups()) {
			glm::vec3 groupPmax = *group->getPMax();
			glm::vec3 groupPmin = *group->getPMin();
			glm::vec3 boundingBoxDimensions = (groupPmax - groupPmin) * objScale;
			glm::vec3 groupCenter = ((groupPmax + groupPmin) * objScale) / 2.0f;

			glm::vec3 groupRotationOrigin = obj->getOrigin() - groupCenter;	// Origem de rota��o da BB do grupo tem que estar ancorada na origem do objeto
			glm::vec3 groupPosition = obj->getPosition() + groupCenter;

			auxBox->setOrigin(groupRotationOrigin);
			auxBox->setPosition(groupPosition);
			auxBox->setEulerAngles(obj->getEulerAngles());
			auxBox->setScale(boundingBoxDimensions);
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxBox->getTranslate()));
			drawObj(auxBox, GL_LINE_STRIP);
		}

		glUniform1f(alphaLocation, 1.0f);
	}

	return collidedWithCurrentObject;
}


void System::Run()
{
	/* Setup da cena */
	float objSpeed = 2.5f;
	float worldSize = 60.0f;
	float ang1 = glm::radians(48.0f);
	float ang2 = glm::radians(15.0f);

	vector<Obj3D*> objs = vector<Obj3D*>();

	ObjManager* objManager = new ObjManager();

	//Obj3D* table1 = objManager->readObj("objs/mesa01.obj");
	//table1->setName("table1");
	//table1->setScale(glm::vec3(0.6f));
	//table1->setPosition(glm::vec3(10.0f));
	//table1->setCollision(false);
	//table1->setDirection(glm::normalize(glm::vec3(cos(radsXY), sin(radsXY), sin(radsXZ))));

	//Obj3D* table2 = objManager->readObj("objs/mesa01.obj");
	//table2->setName("table2");
	//table1->setScale(glm::vec3(0.3f));
	//table2->setCollision(false);
	//table2->setDirection(glm::normalize(glm::vec3(cos(radsXZ), sin(radsXZ), sin(radsXY))));

	//Obj3D* paintballField = objManager->readObj("objs/cenaPaintball.obj");
	//paintballField->setName("paintballField");
	//paintballField->setTranslate(glm::scale(glm::mat4(1.0f), glm::vec3(0.6f)));
	//paintballField->setCollision(true);
	//paintballField->setDirection(glm::normalize(glm::vec3(cos(radsXY), sin(radsXY), sin(radsXY))));

	Obj3D* toonLink1 = objManager->readObj("../objs/DolToonlinkR1_fixed.obj");
	toonLink1->setName("ToonLink1");
	toonLink1->setScale(glm::vec3(0.6f, 1.5f, 0.6f));
	objs.push_back(toonLink1);

	Obj3D* toonLink2 = toonLink1->copy();
	toonLink2->setName("ToonLink2");
	//objs.push_back(toonLink2);

	//Obj3D* toonLink3 = toonLink1->copy();
	//toonLink3->setName("ToonLink3");
	//objs.push_back(toonLink3);

	Obj3D* libertyStatue = objManager->readObj("../objs/LibertStatue.obj");
	libertyStatue->setName("LibertStatue");
	libertyStatue->setEulerAngles(glm::vec3(16.0f, 0.0f, -16.0f));
	libertyStatue->setScale(glm::vec3(18.0f));
	libertyStatue->setCollision(false);
	libertyStatue->setDirection(glm::normalize(glm::vec3(cos(ang1), sin(ang2), sin(ang2))));
	//objs.push_back(libertyStatue);

	auxBox = objManager->getHardcodedCube(0.5f);
	auxCircle = objManager->get2DCircle(0.5f, 32);

	Obj3D* worldBox = objManager->getHardcodedCube(worldSize);

	Obj3D* projectile = auxBox->copy();
	projectile->setPosition(glm::vec3(30.0f, 0.0f, 0.0f));
	projectile->setScale(glm::vec3(10.0f, 5.0f, 5.0f));


	glm::vec3 NR = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 NT = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 NL = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 NB = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 NFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 NBack = glm::vec3(0.0f, 0.0f, 1.0f);

	bool collidedWithAnyObjectThisFrame = false;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		ProcessInput(window, elapsedSeconds);

		glfwSetCursorPosCallback(window, mouseCallback);

		/* Projection e view */
		glm::mat4 projection, view;
		projection = glm::perspective(glm::radians(90.0f), (float)screenWidth / (float)screenHeight, 0.01f, 1000.0f);
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

		/* Desenha caixa do mundo */
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(worldBox->getTranslate()));
		drawObj(worldBox, GL_LINE_STRIP);

		/* Move objetos da cena */
		float angle = currentSeconds / 4;
		glm::vec3 toonlinkCenter = toonLink1->getScale() * (*toonLink1->getGlobalPMax() + *toonLink1->getGlobalPMin()) / 2.0f;
		toonlinkCenter += cos(currentSeconds) * 10.0f;
		toonLink1->setPosition(glm::vec3(35.0f * cos(angle), -toonlinkCenter.y, 30.0f * sin(angle)));
		toonLink2->setPosition(glm::vec3(40.0f * cos(90 + angle), 40 * sin(90 + angle), -toonlinkCenter.z));
		libertyStatue->setPosition(glm::vec3(-toonlinkCenter.x, 25.0f * cos(90 + angle), 25.0f * sin(90 + angle)));
		toonLink1->setEulerAngles(glm::vec3(currentSeconds * 20.0f));
		toonLink2->setEulerAngles(glm::vec3(currentSeconds * 20.0f));
		libertyStatue->setEulerAngles(glm::vec3(currentSeconds * 20.0f));

		projectile->setPosition(glm::vec3(30.0f, 0.0f, 0.0f));


		glUniform1f(alphaLocation, 1.0f);
		collidedWithAnyObjectThisFrame = false;

		for (Obj3D* obj : objs) {

			//glm::vec3 objCurrentDirection = obj->getDirection();
			//glm::vec3 delta = glm::vec3(elapsedSeconds * objSpeed) * objCurrentDirection;

			bool collidedWithCurrentObject = testCollisionSphereVSCube(projectile, obj, true);
			collidedWithAnyObjectThisFrame = collidedWithAnyObjectThisFrame || collidedWithCurrentObject;

			if (collidedWithCurrentObject) {
				cout << "collided with " << obj->getName() << endl;
			} else {
				cout << "didn't collide with " << obj->getName() << endl;
			}

			/* Desenha objeto */
			if (collidedWithCurrentObject) glUniform1f(alphaLocation, 0.2f);
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obj->getTranslate()));
			drawObj(obj, GL_TRIANGLES);
			glUniform1f(alphaLocation, 1.0f);
		}
		cout << endl;

		/* Desenha projétil */
		if (collidedWithAnyObjectThisFrame) glUniform1f(alphaLocation, 0.2f);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(projectile->getTranslate()));
		drawObj(projectile, GL_LINE_STRIP);

		/* Desenha bounding sphere do projétil */
		glUniform1f(alphaLocation, 0.4f);
		int numberOfCircles = 10;
		for (int i = 0; i < numberOfCircles; i++) {
			auxCircle->setScale(glm::vec3(getBoundingSphereRadius(projectile) * 2.0f));
			auxCircle->setPosition(projectile->getPosition());
			auxCircle->setEulerAngles(glm::vec3(0.0f, (i / (float)numberOfCircles) * 180, 0.0f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(auxCircle->getTranslate()));
			drawObj(auxCircle, GL_LINE_STRIP);
		}
		glUniform1f(alphaLocation, 1.0f);

		glfwSwapBuffers(window);
	}

}

void System::Finish() {
	coreShader.Delete();

	glfwTerminate();
}
