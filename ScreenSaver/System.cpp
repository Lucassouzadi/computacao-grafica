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
	coreShader.LoadTexture("images/woodTexture.jpg", "texture1", "woodTexture");

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

	float speed = 1.0f;
	float startingAngle = 40.0f; // Angle in degrees
	float rads = startingAngle * PI / 180; // Angle in radians

	glm::vec2 startingPosition = glm::vec2(0.0f, 0.0f);
	glm::vec2 direction = glm::vec2(cos(rads), sin(rads));

	glm::vec2 NR = glm::vec2(-1.0f, 0.0f);
	glm::vec2 NT = glm::vec2(0.0f, -1.0f);
	glm::vec2 NL = glm::vec2(1.0f, 0.0f);
	glm::vec2 NB = glm::vec2(0.0f, 1.0f);

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

		glm::vec2 delta = glm::vec2(elapsedSeconds * speed) * direction;

		for (int vert = 0; vert < 3; vert++) {
			glm::vec2 vertPos = glm::vec2(currentPosition.x + vertices[vert * 3], currentPosition.y + vertices[vert * 3 + 1]);
			glm::vec2 vertFuturePos = vertPos + delta;

			bool collided = false;
			glm::vec2 distanceUntillCollidedBorder;
			glm::vec2 collisionNormal;
			if (collided = vertFuturePos.x > 1.0f) {		// Right border colision
				float tan = direction.y / direction.x;
				float xUntilRightBorder = 1.0f - vertPos.x;
				float yUntilRightBorder = tan * xUntilRightBorder;
				distanceUntillCollidedBorder = glm::vec2(xUntilRightBorder, yUntilRightBorder);
				collisionNormal = NR;
			}
			else if (collided = vertFuturePos.x < -1.0f) {	// Left border colision
				float tan = direction.y / direction.x;
				float xUntilLeftBorder = -1.0f - vertPos.x;
				float yUntilLeftBorder = tan * xUntilLeftBorder;
				distanceUntillCollidedBorder = glm::vec2(xUntilLeftBorder, yUntilLeftBorder);
				collisionNormal = NL;
			}
			else if (collided = vertFuturePos.y > 1.0f) {	// Top border colision
				float tan = direction.x / direction.y;
				float yUntilTopBorder = 1.0f - vertPos.y;
				float xUntilTopBorder = tan * yUntilTopBorder;
				distanceUntillCollidedBorder = glm::vec2(xUntilTopBorder, yUntilTopBorder);
				collisionNormal = NT;
			}
			else if (collided = vertFuturePos.y < -1.0f) {	// Bottom border colision
				float tan = direction.x / direction.y;
				float yUntilBottomBorder = -1.0f - vertPos.y;
				float xUntilBottomBorder = tan * yUntilBottomBorder;
				distanceUntillCollidedBorder = glm::vec2(xUntilBottomBorder, yUntilBottomBorder);
				collisionNormal = NB;
			}

			if (collided) {
				currentPosition += distanceUntillCollidedBorder;
				delta -= distanceUntillCollidedBorder;
				delta = reflexao(delta, collisionNormal);
				direction = reflexao(direction, collisionNormal);
				vert = 0;
			}
		}

		currentPosition += delta;
		translate[12] = currentPosition.x;
		translate[13] = currentPosition.y;



		coreShader.Use();
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
