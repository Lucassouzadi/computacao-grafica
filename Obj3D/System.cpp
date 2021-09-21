#include "System.h"
#include "ObjManager.h"
constexpr auto PI = 3.14159265;

using namespace std;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 70.0f;

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
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 cameraCenter = cameraPosition + glm::normalize(front);
	cameraFront = glm::normalize(front);
}

float rads(float degrees) {
	return degrees * PI / 180.0f;
}

void System::Run()
{

	float objSpeed = 0.0f;
	float worldSize = 50.0f;
	float radsXY = rads(48.0f);
	float radsXZ = rads(15.0f);

	ObjManager* objManager = new ObjManager();

	Obj3D* table1 = objManager->readObj("obj/mesa01.obj");
	table1->setName("table1");
	table1->setScale(glm::vec3(0.8f));
	table1->setCollision(true);
	table1->setDirection(glm::normalize(glm::vec3(cos(radsXY), sin(radsXY), sin(radsXZ))));
	objManager->objToVAO(table1);

	Obj3D* table2 = objManager->readObj("obj/mesa01.obj");
	table2->setName("table2");
	table1->setScale(glm::vec3(0.3f));
	table2->setCollision(true);
	table2->setDirection(glm::normalize(glm::vec3(cos(radsXZ), sin(radsXZ), sin(radsXY))));
	objManager->objToVAO(table2);

	//Obj3D* paintballField = objManager->readObj("obj/cenaPaintball.obj");
	//paintballField->setName("paintballField");
	//paintballField->setTranslate(glm::scale(glm::mat4(1.0f), glm::vec3(0.6f)));
	//paintballField->setCollision(true);
	//paintballField->setDirection(glm::normalize(glm::vec3(cos(radsXY), sin(radsXY), sin(radsXY))));
	//objManager->objToVAO(paintballField);

	Obj3D* libertyStatue = objManager->readObj("obj/LibertStatue.obj");
	libertyStatue->setName("libertyStatue");

	libertyStatue->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//libertyStatue->setRotation(90.0f);
	libertyStatue->setScale(glm::vec3(20.0f));

	libertyStatue->setCollision(false);
	libertyStatue->setDirection(glm::normalize(glm::vec3(cos(radsXY), sin(radsXY), sin(radsXZ))));
	objManager->objToVAO(libertyStatue);

	Obj3D* worldLimits = objManager->getHardcodedCube(worldSize);
	objManager->objToVAO(worldLimits);


	vector<Obj3D*> objs = vector<Obj3D*>();
	objs.push_back(libertyStatue);
	//objs.push_back(paintballField);
	objs.push_back(table1);
	objs.push_back(table2);

	int modelLocation = glGetUniformLocation(coreShader.program, "model");
	int projectionLocation = glGetUniformLocation(coreShader.program, "projection");
	int viewLocation = glGetUniformLocation(coreShader.program, "view");


	glm::vec3 NR = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 NT = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 NL = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 NB = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 NFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 NBack = glm::vec3(0.0f, 0.0f, 1.0f);


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

		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		projection = glm::perspective(glm::radians(90.0f), (float)screenWidth / (float)screenHeight, 0.01f, 1000.0f);

		/*float radius = 100.0f;
		float camX = sin(currentSeconds);
		float camY = (camX + 1) / 2;
		float camZ = cos(currentSeconds);
		glm::vec3 cameraPosition = glm::vec3(camX, camY, camZ);
		glm::vec3 normalizedCamPosition = glm::normalize(camPosition);*/

		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

		for (Obj3D* obj : objs) {
			glm::vec3 objCurrentDirection = obj->getDirection();
			glm::vec3 delta = glm::vec3(elapsedSeconds * objSpeed) * objCurrentDirection;
			if (obj->getCollision()) {
				for (int vertIndex = 0; vertIndex < obj->getMesh()->getVertex().size(); vertIndex++) {
					glm::vec3 vert = *(obj->getMesh()->getVertex()[vertIndex]);
					glm::vec3 vertPos = glm::translate(obj->getTranslate(), vert)[3];
					glm::vec3 vertFuturePos = vertPos + delta;

					bool collidedWithBorder = false;
					glm::vec3 distanceUntillCollidedBorder;
					glm::vec3 collisionNormal;
					if (collidedWithBorder = vertFuturePos.x > worldSize) {		// Right border colision
						cout << "[" << obj->getName() << "]- Collision with Right border" << endl;
						float xUntilRightBorder = worldSize - vertPos.x;
						distanceUntillCollidedBorder = delta * (xUntilRightBorder / delta.x);
						collisionNormal = NR;
					}
					else if (collidedWithBorder = vertFuturePos.x < -worldSize) {	// Left border colision
						cout << "[" << obj->getName() << "]- Collision with Left border" << endl;
						float xUntilLeftBorder = -worldSize - vertPos.x;
						distanceUntillCollidedBorder = delta * (xUntilLeftBorder / delta.x);
						collisionNormal = NL;
					}
					else if (collidedWithBorder = vertFuturePos.y > worldSize) {	// Top border colision
						cout << "[" << obj->getName() << "]- Collision with Top border" << endl;
						float yUntilTopBorder = worldSize - vertPos.y;
						distanceUntillCollidedBorder = delta * (yUntilTopBorder / delta.y);
						collisionNormal = NT;
					}
					else if (collidedWithBorder = vertFuturePos.y < -worldSize) {	// Bottom border colision
						cout << "[" << obj->getName() << "]- Collision with Bottom border" << endl;
						float yUntilBottomBorder = -worldSize - vertPos.y;
						distanceUntillCollidedBorder = delta * (yUntilBottomBorder / delta.y);
						collisionNormal = NB;
					}
					else if (collidedWithBorder = vertFuturePos.z > worldSize) {	// Front border colision
						cout << "[" << obj->getName() << "]- Collision with Front border" << endl;
						float zUntilRightBorder = worldSize - vertPos.z;
						distanceUntillCollidedBorder = delta * (zUntilRightBorder / delta.z);
						collisionNormal = NFront;
					}
					else if (collidedWithBorder = vertFuturePos.z < -worldSize) {	// Back border colision
						cout << "[" << obj->getName() << "]- Collision with Back border" << endl;
						float zUntilRightBorder = -worldSize - vertPos.z;
						distanceUntillCollidedBorder = delta * (zUntilRightBorder / delta.z);
						collisionNormal = NBack;
					}

					if (collidedWithBorder) {
						delta -= distanceUntillCollidedBorder;
						delta = glm::vec3(reflexao(delta, collisionNormal));
						obj->setPosition(obj->getPosition()+ glm::vec3(distanceUntillCollidedBorder));
						obj->setDirection(glm::vec3(reflexao(objCurrentDirection, collisionNormal)));
						vertIndex = 0;
					}
				}
			}

			obj->setPosition(obj->getPosition() + glm::vec3(delta));
			obj->setEulerAngles(glm::vec3(0.0f, currentSeconds * 10, 0.0f));


			cout << cameraFront.x << " " << cameraFront.z << " " << obj->getEulerAngles().y << endl;

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obj->getTranslate()));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));


			coreShader.Use();

			for (Group* group : obj->getMesh()->getGroups()) {
				glBindVertexArray(group->getVAO());
				glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
				glDrawArrays(GL_POINTS, 0, group->getNumVertices());
				//glDrawArrays(GL_LINE_STRIP, 0, group->getNumVertices());
				glBindVertexArray(0);
			}

		}


		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(worldLimits->getTranslate()));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		for (Group* group : worldLimits->getMesh()->getGroups()) {
			glBindVertexArray(group->getVAO());
			//glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
			//glDrawArrays(GL_POINTS, 0, group->getNumVertices());
			glDrawArrays(GL_LINE_STRIP, 0, group->getNumVertices());
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
	}

}

void System::Finish() {
	coreShader.Delete();

	glfwTerminate();
}
