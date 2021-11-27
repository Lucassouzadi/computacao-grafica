#include "System.h"

constexpr auto PI = 3.14159265;

using namespace std;

const GLint WIDTH = 700, HEIGHT = 700;

vector<glm::vec3> controlPoints;
vector<glm::vec3> controlPointsColor;
vector<glm::vec3> mainCurvePoints;
vector<glm::vec3> innerCurvePoints;
vector<glm::vec3> outerCurvePoints;
vector<glm::vec3> curveColor;

float dotXY(glm::vec3 v1, glm::vec3 v2) {
	return v1.x * v2.x + v1.y * v2.y;
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
		controlPointsColor.push_back(glm::vec3(x, y, 0.f));
	}
}

void writeCourseObj() {
	cout << "writing course to obj" << endl;

	cout << "g pista" << endl;
	cout << "vt 0.0 0.0" << endl;
	cout << "vn 0.0 1.0" << endl;

	int courseSize = mainCurvePoints.size();

	cout << endl << "# INNER CURVE VERTICES" << endl;
	for (int i = 0; i < courseSize; i++) {
		glm::vec3 ip = innerCurvePoints[i];
		cout << "v " << ip.x << " " << ip.y << " " << ip.z << endl;
	}
	cout << endl << "# OUTER CURVE VERTICES" << endl;
	for (int i = 0; i < courseSize; i++) {
		glm::vec3 op = outerCurvePoints[i];
		cout << "v " << op.x << " " << op.y << " " << op.z << endl;
	}
	cout << endl << "# FACES" << endl;
	for (int i = 1; i <= courseSize; i++) {
		cout << "f "
			<< i << "/1/1 "
			<< i % courseSize + 1 << "/1/1 "
			<< i % courseSize + 1 + courseSize << "/1/1 "
			<< i + courseSize << "/1/1 " << endl;
	}

	cout << "Finished" << endl;
}

void System::Run()
{

	coreShader.Use();
	coreShader.LoadTexture("images/woodTexture.jpg", "texture1", "woodTexture");

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	GLuint innerCurveVBO, mainCurveVBO, outerCurveVBO, controlPointsVBO, curveColorVBO, controlPointsColorVBO, innerCurveVAO, mainCurveVAO, outerCurveVAO, controlPointsVAO;
	glGenVertexArrays(1, &controlPointsVAO);
	glGenVertexArrays(1, &innerCurveVAO);
	glGenVertexArrays(1, &mainCurveVAO);
	glGenVertexArrays(1, &outerCurveVAO);
	glGenBuffers(1, &controlPointsVBO);
	glGenBuffers(1, &innerCurveVBO);
	glGenBuffers(1, &mainCurveVBO);
	glGenBuffers(1, &outerCurveVBO);
	glGenBuffers(1, &curveColorVBO);
	glGenBuffers(1, &controlPointsColorVBO);

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			writeCourseObj();
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glPointSize(10.0f);

		const int segmentQuantity = 100;
		const int N = controlPoints.size();

		mainCurvePoints.clear();
		innerCurvePoints.clear();
		outerCurvePoints.clear();
		curveColor.clear();

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < segmentQuantity; j++) {
				float x, y, z;
				float t = (float) j / segmentQuantity;

				glm::vec3 
					p1 = controlPoints[i],
					p2 = controlPoints[(i + 1) % N],
					p3 = controlPoints[(i + 2) % N],
					p4 = controlPoints[(i + 3) % N];

				x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.x +
					  ( 3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.x +
					  (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.x +
					  ( 1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.x) / 6);
				y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.y +
					  ( 3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.y +
					  (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.y +
					  ( 1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.y) / 6);
				z = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * p1.z +
					  ( 3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * p2.z +
					  (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * p3.z +
					  ( 1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * p4.z) / 6);

				glm::vec3 point = glm::vec3(x, y, z);
				mainCurvePoints.push_back(point);
				curveColor.push_back(glm::vec3(1.0f));
			}
		}

		const float width = 0.1f;
		if (mainCurvePoints.size() > 0) {
			glm::vec3 previousPoint = mainCurvePoints[mainCurvePoints.size() - 1];
			for (int i = 0; i < mainCurvePoints.size(); i++) {
				glm::vec3 currentPoint = mainCurvePoints[i];

				glm::vec3 AB = currentPoint - previousPoint;
				glm::vec3 offset = glm::normalize(glm::vec3(AB.y, -AB.x, 0.0f)) * width / 2.0f;
				glm::vec3 AC = currentPoint + offset;

				innerCurvePoints.push_back(currentPoint + offset);
				outerCurvePoints.push_back(currentPoint - offset);

				previousPoint = currentPoint;
			}
		}



		glBindVertexArray(controlPointsVAO);

		glBindBuffer(GL_ARRAY_BUFFER, controlPointsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * controlPoints.size(), controlPoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, controlPointsColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * controlPointsColor.size(), controlPointsColor.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);


		glBindVertexArray(mainCurveVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mainCurveVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mainCurvePoints.size(), mainCurvePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, curveColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curveColor.size(), curveColor.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);


		glBindVertexArray(outerCurveVAO);

		glBindBuffer(GL_ARRAY_BUFFER, outerCurveVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * outerCurvePoints.size(), outerCurvePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, curveColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curveColor.size(), curveColor.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);


		glBindVertexArray(innerCurveVAO);

		glBindBuffer(GL_ARRAY_BUFFER, innerCurveVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * innerCurvePoints.size(), innerCurvePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, curveColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curveColor.size(), curveColor.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);


		glBindVertexArray(controlPointsVAO);
		glDrawArrays(GL_POINTS, 0, controlPoints.size());

		glBindVertexArray(mainCurveVAO);
		glDrawArrays(GL_LINE_LOOP, 0, mainCurvePoints.size());

		glBindVertexArray(outerCurveVAO);
		glDrawArrays(GL_LINE_LOOP, 0, outerCurvePoints.size());

		glBindVertexArray(innerCurveVAO);
		glDrawArrays(GL_LINE_LOOP, 0, innerCurvePoints.size());


		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

}

void System::Finish()
{
	coreShader.Delete();

	glfwTerminate();
}
