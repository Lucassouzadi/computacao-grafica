#include "System.h"
constexpr auto PI = 3.14159265;


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

	window = glfwCreateWindow(WIDTH, HEIGHT, "Sabertooth", nullptr, nullptr);

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
		1.0f, 0.0f, 0.0f, 0.0f,  // 1ª coluna 
		0.0f, 1.0f, 0.0f, 0.0f,  // 2ª coluna 
		0.0f, 0.0f, 1.0f, 0.0f,  // 3ª coluna 
		0.0f, 0.0f, 0.0f, 1.0f // 4ª coluna
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
	float directionX = 1.0f;
	float directionY = 1.0f;
	float lastPositionX = 0.0f;
	float lastPositionY = 0.0f;
	float angle = 40.0f; // Angle in degrees

	double seno = sin(angle * PI / 180);
	double cosseno = cos(angle * PI / 180);

	printf("seno %f\n", seno);
	printf("cosseno %f\n", cosseno);

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		for (int vert = 0; vert < 3; vert++) {
			float vertX = lastPositionX+vertices[vert * 3];
			float vertY = lastPositionY+vertices[vert * 3 + 1];

			if (vertX > 1.0f) {			// Right border colision
				directionX = -1.0f; 
			}
			else if (vertX < -1.0f) {	// Left border colision
				directionX = 1.0f;
			}
			if (vertY > 1.0f) {			// Top border colision
				directionY = -1.0f;
			}
			else if (vertY < -1.0f) {	// Bottom border colision
				directionY = 1.0f;
			}
		}

		lastPositionX += elapsedSeconds * speed * cosseno * directionX;
		translate[12] = lastPositionX;
		lastPositionY += elapsedSeconds * speed * seno * directionY;
		translate[13] = lastPositionY;

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
