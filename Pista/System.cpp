#include "System.h"
#include <iostream>
#include <fstream>

constexpr auto PI = 3.14159265;

using namespace std;

const GLint WIDTH = 700, HEIGHT = 700;

vector<glm::vec3> controlPoints;
vector<glm::vec3> controlPointsColor;
vector<glm::vec3> mainCurvePoints;
vector<glm::vec3> innerCurvePoints;
vector<glm::vec3> outerCurvePoints;
vector<glm::vec3> curveColor;
float currentZ = 0.0f;

const int segmentQuantity = 100;
const float textureRatio = 0.2f;

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

bool deletePressedOnMouseDown = false;

bool mouseDown = false;

int controlPointSelectedIndex = -1;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mouseDown == true)
	{
		std::cout << "Mouse button released" << std::endl;
		mouseDown = false;

		if (deletePressedOnMouseDown) {
			deletePressedOnMouseDown = false;
			if (controlPointSelectedIndex >= 0) {
				controlPoints.erase(controlPoints.begin() + controlPointSelectedIndex);
				controlPointsColor.erase(controlPointsColor.begin() + controlPointSelectedIndex);
			}
			return;
		}
		
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT - y;

		y = y / HEIGHT * 2 - 1;
		x = x / WIDTH * 2 - 1;

		if (controlPointSelectedIndex >= 0) {
			controlPoints[controlPointSelectedIndex] = glm::vec3(x, y, currentZ);
			controlPointsColor[controlPointSelectedIndex] = glm::vec3((currentZ + 1) / 2);
		}
		else {
			controlPoints.push_back(glm::vec3(x, y, currentZ));
			controlPointsColor.push_back(glm::vec3((currentZ + 1) / 2));
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseDown == false)
	{
		std::cout << "Mouse button pressed" << std::endl;
		mouseDown = true;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT - y;

		y = y / HEIGHT * 2 - 1;
		x = x / WIDTH * 2 - 1;

		cout << "mouse x: " << x << endl;
		cout << "mouse y: " << y << endl;

		controlPointSelectedIndex = -1;

		for (int i = 0; i < controlPoints.size(); i++) {
			//We opt to disconsider the Z coordinate
			float difference = sqrt(pow(controlPoints[i].x - x, 2) + pow(controlPoints[i].y - y, 2));// +abs(controlPoints[i].z - currentZ);

			if (difference < 0.1f) {
				controlPointSelectedIndex = i;
				break;
			}
		}
	}
}

void writeCourseObj() {

	cout << "writing course to obj" << endl;

	ofstream courseObjFile("../Lighting/objs/pista.obj");

	courseObjFile << "mtllib pista.mtl" << endl;
	courseObjFile << "g pista" << endl;
	courseObjFile << "usemtl asfalto" << endl << endl;

	//courseObjFile << "vt 0.0 0.0" << endl;
	//courseObjFile << "vt 1.0 0.0" << endl;
	//courseObjFile << "vt 1.0 3.0" << endl;
	//courseObjFile << "vt 0.0 3.0" << endl;

	courseObjFile << "vn 0.0 1.0 0.0" << endl;

	int courseSize = mainCurvePoints.size();

	courseObjFile << endl << "# INNER CURVE VERTICES" << endl;
	for (int i = 0; i < courseSize; i++) {
		glm::vec3 ip = innerCurvePoints[i];
		courseObjFile << "v " << ip.x << " " << ip.z << " " << -ip.y << endl;
	}
	courseObjFile << endl << "# OUTER CURVE VERTICES" << endl;
	for (int i = 0; i < courseSize; i++) {
		glm::vec3 op = outerCurvePoints[i];
		courseObjFile << "v " << op.x << " " << op.z << " " << -op.y << endl;
	}
	float currentTextureMappingY = 0.0;
	for (int i = 0; i < courseSize; i++) {
		courseObjFile << "vt " << "1.0" << " " << currentTextureMappingY << endl;
		courseObjFile << "vt " << "0.0" << " " << currentTextureMappingY << endl;
		glm::vec3 p0 = mainCurvePoints[i];
		glm::vec3 p1 = mainCurvePoints[(i + 1) % courseSize];
		glm::vec3 segment = p1 - p0;
		float segmentLenght = sqrt(pow(segment.x, 2) + pow(segment.y, 2) + pow(segment.z, 2));
		currentTextureMappingY += segmentLenght / textureRatio;
	}
	courseObjFile << endl << "# FACES" << endl;
	for (int i = 1; i <= courseSize; i++) {
		int currentText = (i - 1) * 2;
		courseObjFile << "f "
			<< i <<									"/" << currentText + 1 <<						"/1 "
			<< i % courseSize + 1 <<				"/" << (currentText + 3) % (courseSize * 2) <<	"/1 "
			<< i % courseSize + 1 + courseSize <<	"/" << (currentText + 3) % (courseSize * 2) + 1 <<	"/1 "
			<< i + courseSize <<					"/" << currentText + 2 <<						"/1 " << endl;
	}

	courseObjFile.close();

	cout << "Finished" << endl;
}

void writeCourseCurve() {
	cout << "writing course to curve" << endl;

	ofstream courseCurveFile("../Lighting/objs/pista.curve");

	for (int i = 0; i < mainCurvePoints.size(); i++) {
		courseCurveFile << mainCurvePoints[i].x << " " << mainCurvePoints[i].z << " " << -mainCurvePoints[i].y << " " << endl;
	}

	courseCurveFile.close();

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

		static double previousSeconds = glfwGetTime();
		double currentSeconds = glfwGetTime();
		double elapsedSeconds = currentSeconds - previousSeconds;
		previousSeconds = currentSeconds;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			writeCourseObj();
			writeCourseCurve();
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			currentZ += elapsedSeconds;
			currentZ = min(0.999f, currentZ);
			cout << "z: " << currentZ << endl;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			currentZ -= elapsedSeconds;
			currentZ = max(-1.0f, currentZ);
			cout << "z: " << currentZ << endl;
		}
		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
			if (mouseDown) {
				deletePressedOnMouseDown = true;
			}
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glPointSize(10.0f);

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
				curveColor.push_back(glm::vec3((z + 1) / 2));
			}
		}

		const float width = 0.06f;
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
