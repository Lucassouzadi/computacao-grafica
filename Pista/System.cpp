#include "System.h"
#include <vector>

constexpr auto PI = 3.14159265;

using namespace std;

const GLint WIDTH = 1000, HEIGHT = 1000;

vector<glm::vec3> controlPoints;
vector<glm::vec3> curvePoints;

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cout << "mouse right click" << endl;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT - y;

		y =  y / HEIGHT * 2 - 1;
		x = x / WIDTH * 2 - 1;

		cout << "mouse x: " << x << endl;
		cout << "mouse y: " << y << endl;

		controlPoints.push_back(glm::vec3(x, y, 0.f));
	}
}

void System::Run()
{

	coreShader.Use();
	coreShader.LoadTexture("images/woodTexture.jpg", "texture1", "woodTexture");

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	GLuint positionVBO, colorVBO, triangleVAO;
	glGenVertexArrays(1, &triangleVAO);
	glGenBuffers(1, &positionVBO);
	glGenBuffers(1, &colorVBO);

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glPointSize(10.0f);

		const float inc = 1/100.f;
		const int N = controlPoints.size();

		curvePoints.clear();

		for (int i = 0; i < N; i++) {
			for (float t = 0; t <= 1; t += inc) {
				float x, y, z;

				glm::vec3 p1 = controlPoints[i],
					p2 = controlPoints[(i + 1) % N],
					p3 = controlPoints[(i + 2) % N],
					p4 = controlPoints[(i + 3) % N];

				x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.x +
					(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.x +
					(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.x +
					(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.x) / 6);
				y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.y +
					(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.y +
					(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.y +
					(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.y) / 6);
				z = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.z +
					(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.z +
					(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.z +
					(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.z) / 6);

				curvePoints.push_back(glm::vec3(x, y, z));
			}
		}


		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(triangleVAO);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curvePoints.size(), curvePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Color attribute
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curvePoints.size(), curvePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // Unbind VAO

		glBindVertexArray(triangleVAO);
		//glDrawArrays(GL_POINTS, 0, curvePoints.size());
		glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

}

void System::Finish()
{
	coreShader.Delete();

	glfwTerminate();
}
