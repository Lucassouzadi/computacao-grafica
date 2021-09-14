#include "System.h"
#include "ObjManager.h"
constexpr auto PI = 3.14159265;

using namespace std;

glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 0.1;

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

glm::vec2 reflexao(glm::vec3 direcao, glm::vec3 normal) {
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

void System::ProcessInput(GLFWwindow* window,  double currentseconds)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraDisplacement = cameraSpeed * currentseconds;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

void System::Run()
{
	ObjManager* objManager = new ObjManager();

	Obj3D* table = new Obj3D();
	table->setCollision(false);
	table->setMesh(objManager->readObj("obj/mesa01.obj"));
	objManager->objToVAO(table);

	Obj3D* paintballField = new Obj3D();
	paintballField->setCollision(false);
	paintballField->setMesh(objManager->readObj("obj/cenaPaintball.obj"));
	objManager->objToVAO(paintballField);

	Obj3D* libertyStatue = new Obj3D();
	libertyStatue->setCollision(false);
	libertyStatue->setMesh(objManager->readObj("obj/LibertStatue.obj"));
	objManager->objToVAO(libertyStatue);


	vector<Obj3D*> objs = vector<Obj3D*>();
	objs.push_back(libertyStatue);
	objs.push_back(paintballField);
	objs.push_back(table);

	int modelLocation = glGetUniformLocation(coreShader.program, "model");
	int projectionLocation = glGetUniformLocation(coreShader.program, "projection");
	int viewLocation = glGetUniformLocation(coreShader.program, "view");

	float speed = 0.0f;
	float startingAngle = 32.0f; // Angle in degrees
	float rads = startingAngle * PI / 180; // Angle in radians


	glm::vec3 NR = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 NT = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 NL = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 NB = glm::vec3(0.0f, 1.0f, 0.0f);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		ProcessInput(window, currentSeconds);

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


		static glm::vec3 direction = glm::vec3(cos(rads), sin(rads), 0.0f);
		static glm::vec3 currentPosition = glm::vec3(0.0f);


		glm::vec3 delta = glm::vec3(elapsedSeconds * speed) * direction;

		for (Obj3D* obj : objs) {
			if (obj->getCollision()) {
				for (int vertIndex = 0; vertIndex < obj->getMesh()->getVertex().size(); vertIndex++) {
					glm::vec3 vert = *(obj->getMesh()->getVertex()[vertIndex]);
					glm::vec3 vertPos = currentPosition + vert;
					glm::vec3 vertFuturePos = vertPos + delta;

					bool collidedWithBorder = false;
					glm::vec3 distanceUntillCollidedBorder;
					glm::vec3 collisionNormal;
					if (collidedWithBorder = vertFuturePos.x > 10.0f) {		// Right border colision
						printf("Collision with Right border\n");
						float tan = direction.y / direction.x;
						float xUntilRightBorder = 1.0f - vertPos.x;
						float yUntilRightBorder = tan * xUntilRightBorder;
						distanceUntillCollidedBorder = glm::vec3(xUntilRightBorder, yUntilRightBorder, 0.0f);
						collisionNormal = NR;
					}
					else if (collidedWithBorder = vertFuturePos.x < -10.0f) {	// Left border colision
						printf("Collision with Left border\n");
						float tan = direction.y / direction.x;
						float xUntilLeftBorder = -1.0f - vertPos.x;
						float yUntilLeftBorder = tan * xUntilLeftBorder;
						distanceUntillCollidedBorder = glm::vec3(xUntilLeftBorder, yUntilLeftBorder, 0.0f);
						collisionNormal = NL;
					}
					else if (collidedWithBorder = vertFuturePos.y > 10.0f) {	// Top border colision
						printf("Collision with Top border\n");
						float tan = direction.x / direction.y;
						float yUntilTopBorder = 1.0f - vertPos.y;
						float xUntilTopBorder = tan * yUntilTopBorder;
						distanceUntillCollidedBorder = glm::vec3(xUntilTopBorder, yUntilTopBorder, 0.0f);
						collisionNormal = NT;
					}
					else if (collidedWithBorder = vertFuturePos.y < -10.0f) {	// Bottom border colision
						printf("Collision with Bottom border\n");
						float tan = direction.x / direction.y;
						float yUntilBottomBorder = -1.0f - vertPos.y;
						float xUntilBottomBorder = tan * yUntilBottomBorder;
						distanceUntillCollidedBorder = glm::vec3(xUntilBottomBorder, yUntilBottomBorder, 0.0f);
						collisionNormal = NB;
					}

					if (collidedWithBorder) {
						currentPosition += distanceUntillCollidedBorder;
						delta -= distanceUntillCollidedBorder;
						delta = glm::vec3(reflexao(delta, collisionNormal), 0.0f);
						direction = glm::vec3(reflexao(direction, collisionNormal), 0.0f);
						vertIndex = 0;
					}
				}
			}

			currentPosition += delta;
			obj->setTranslate(glm::translate(glm::mat4(1.0f), glm::vec3(currentPosition)));
			obj->setDirection(direction);


			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obj->getTranslate()));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));


			coreShader.Use();

			for (Group* group : obj->getMesh()->getGroups()) {
				glBindVertexArray(group->getVAO());
				glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
				//glDrawArrays(GL_LINES, 0, group->getNumVertices());
				glBindVertexArray(0);
			}

		}

		glfwSwapBuffers(window);
	}

}

void System::Finish() {
	coreShader.Delete();

	glfwTerminate();
}
