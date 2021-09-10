#include "System.h"
#include "ObjManager.h"
constexpr auto PI = 3.14159265;

using namespace std;

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
	glFrontFace(GL_CW);

	return EXIT_SUCCESS;
}

int System::SystemSetup()
{

	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader.Use();

	return EXIT_SUCCESS;
}

void System::Run()
{

	ObjManager* objManager = new ObjManager();
	Obj3D* obj = objManager->getHardcoded2DHouse();
	objManager->objToVAO(obj);

	int modelLocation = glGetUniformLocation(coreShader.program, "model");

	float speed = 0.5f;
	float startingAngle = 32.0f; // Angle in degrees
	float rads = startingAngle * PI / 180; // Angle in radians


	glm::vec3 NR = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 NT = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 NL = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 NB = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(coreShader.program, "projection"), 1, GL_FALSE, &projection[0][0]);


	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		static glm::vec3 direction = glm::vec3(cos(rads), sin(rads), 0.0f);
		static glm::vec3 currentPosition = glm::vec3(0.0f);

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		glm::vec3 delta = glm::vec3(elapsedSeconds * speed) * direction;

		for (int vertIndex = 0; vertIndex < obj->getMesh()->getVertex().size(); vertIndex++) {
			glm::vec3 vert = *(obj->getMesh()->getVertex()[vertIndex]);
			glm::vec3 vertPos = currentPosition + vert;
			glm::vec3 vertFuturePos = vertPos + delta;

			bool collided = false;
			glm::vec3 distanceUntillCollidedBorder;
			glm::vec3 collisionNormal;
			if (collided = vertFuturePos.x > 1.0f) {		// Right border colision
				printf("Collision with Right border\n");
				float tan = direction.y / direction.x;
				float xUntilRightBorder = 1.0f - vertPos.x;
				float yUntilRightBorder = tan * xUntilRightBorder;
				distanceUntillCollidedBorder = glm::vec3(xUntilRightBorder, yUntilRightBorder, 0.0f);
				collisionNormal = NR;
			}
			else if (collided = vertFuturePos.x < -1.0f) {	// Left border colision
				printf("Collision with Left border\n");
				float tan = direction.y / direction.x;
				float xUntilLeftBorder = -1.0f - vertPos.x;
				float yUntilLeftBorder = tan * xUntilLeftBorder;
				distanceUntillCollidedBorder = glm::vec3(xUntilLeftBorder, yUntilLeftBorder, 0.0f);
				collisionNormal = NL;
			}
			else if (collided = vertFuturePos.y > 1.0f) {	// Top border colision
				printf("Collision with Top border\n");
				float tan = direction.x / direction.y;
				float yUntilTopBorder = 1.0f - vertPos.y;
				float xUntilTopBorder = tan * yUntilTopBorder;
				distanceUntillCollidedBorder = glm::vec3(xUntilTopBorder, yUntilTopBorder, 0.0f);
				collisionNormal = NT;
			}
			else if (collided = vertFuturePos.y < -1.0f) {	// Bottom border colision
				printf("Collision with Bottom border\n");
				float tan = direction.x / direction.y;
				float yUntilBottomBorder = -1.0f - vertPos.y;
				float xUntilBottomBorder = tan * yUntilBottomBorder;
				distanceUntillCollidedBorder = glm::vec3(xUntilBottomBorder, yUntilBottomBorder, 0.0f);
				collisionNormal = NB;
			}

			if (collided) {
				currentPosition += distanceUntillCollidedBorder;
				delta -= distanceUntillCollidedBorder;
				delta = glm::vec3(reflexao(delta, collisionNormal), 0.0f);
				direction = glm::vec3(reflexao(direction, collisionNormal), 0.0f);
				vertIndex = 0;
			}
		}

		currentPosition += delta;
		obj->setTranslate(glm::translate(glm::mat4(1.0f), glm::vec3(currentPosition)));
		obj->setDirection(direction);

		/*glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

		glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

		glm::mat4 view;

		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));*/

		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(coreShader.program, "view"), 1, GL_FALSE, &view[0][0]);

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obj->getTranslate()));

		for (Group* group : obj->getMesh()->getGroups()) {
			glBindVertexArray(group->getVAO());
			glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
	}

}

void System::Finish() {
	coreShader.Delete();

	glfwTerminate();
}
