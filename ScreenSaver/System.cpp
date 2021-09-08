#include "System.h"
constexpr auto PI = 3.14159265;

float dot(glm::vec2 v1, glm::vec2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

glm::vec2 reflexao(glm::vec2 direcao, glm::vec2 normal) {
	glm::vec2 direcaoContraria = -direcao;
	float a = dot(normal, direcaoContraria);
	glm::vec2 novaDirecao = glm::vec2(2 * normal.x * a - direcaoContraria.x, 2 * normal.y * a - direcaoContraria.y);
	return novaDirecao;
}

System::System()
{
}


System::~System()
{
}

int System::GLFWInit()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(WIDTH, HEIGHT, "ScreenSaver", nullptr, nullptr);

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
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

	coreShader.Use();

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(coreShader.program, "projection"), 1, GL_FALSE, &projection[0][0]);

	GLfloat translate[] = {
		1.0f, 0.0f, 0.0f, 0.0f,  // 1� coluna 
		0.0f, 1.0f, 0.0f, 0.0f,  // 2� coluna 
		0.0f, 0.0f, 1.0f, 0.0f,  // 3� coluna 
		0.0f, 0.0f, 0.0f, 1.0f // 4� coluna
	};

	GLfloat vertices[] =
	{
		0.0f, 0.3f, 0.0f,	// Top
		0.3f,  -0.3f, 0.0f,	// Bottom Right
		-0.3f, -0.3f, 0.0f	// Bottom Left
	};

	GLfloat colors[] =
	{
		1.0f, 0.0f, 0.0f, // Top
		0.0f, 1.0f, 0.0f, // Bottom Right
		0.0f, 0.0f, 1.0f  // Bottom Left
	};

	GLuint positionVBO, colorVBO, triangleVAO;
	glGenVertexArrays(1, &triangleVAO);
	glGenBuffers(1, &positionVBO);
	glGenBuffers(1, &colorVBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(triangleVAO);

	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	int translateLocation = glGetUniformLocation(coreShader.program, "translate");

	glm::vec2 startingPosition = glm::vec2(0.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		static glm::vec2 currentPosition = startingPosition;

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

		glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

		glm::mat4 view;

		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		//view = glm::lookAt(eye, center, up);

		/*const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

		translate[12] = currentPosition.x;
		translate[13] = currentPosition.y;*/

		coreShader.Use();

		glUniformMatrix4fv(glGetUniformLocation(coreShader.program, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(translateLocation, 1, GL_FALSE, translate);

		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}

}

void System::Finish()
{
	coreShader.Delete();

	glfwTerminate();
}
