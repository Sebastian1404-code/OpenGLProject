

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>
#include "Skybox.hpp"

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

bool enableDirLight=true;
GLuint enableDirLightLoc;


//point light
glm::vec3 lightPosPointLight;
GLuint lightPosLoc;
bool enablePosLight = true;
GLuint enableLoc;

gps::Camera myCamera(
				glm::vec3(0.0f, 4.0f, 5.5f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.12f;

bool pressedKeys[1024];
float angleZ = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D scarecrow;
gps::Model3D campfire;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;
gps::Shader pointLight;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBoxDay;
gps::SkyBox mySkyBoxNight;


bool showDepthMap;

//rain
gps::Shader rainShader;

bool enableRain = false;
std::vector<glm::vec3> snowPositions;
const int numSnowdrops = 5000;

//raycasting
float deltaTime = 0.0f; 
float lastFrame = 0.0f;

//fog
GLuint viewerLoc;


//modes
enum VisualizationMode {
	WIREFRAME,
	SOLID,
	POINT
};

VisualizationMode currentMode = SOLID;


//skybox

std::vector<const GLchar*> faces_day;
std::vector<const GLchar*> faces_night;

//animation
bool animation = false;
glm::vec3 initPos = glm::vec3(14.128f, 4.5991f, -15.441f);
glm::vec3 currentPos = initPos;


//automated tour

bool tourActive = false;

//mouse movement
bool firstMouse = true;
float lastX = glWindowWidth / 2;
float lastY = glWindowHeight / 2;
float yaw = -90.0f;
float pitch = 0.0f;

struct CameraPoint {
	float x, y, z; // Camera position
	float tx, ty, tz; // Target position (point of interest)
};
std::vector<CameraPoint> tourPoints = {
	{0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f}, 
	{8.0f, 0.0f, 16.0f, 5.0f, 0.0f, 0.0f}, 
	{-10.0f, 4.0f, 10.0f, -4.0f, 0.0f, -4.0f}, 
	{10.0f, 15.0f, 2.0f, 0.0f, 0.0f, -13.0f},  
	{0.0f, 5.0f, 5.0f, 10.0f, 5.0f, -11.0f},  
};

glm::vec3 interpolate(glm::vec3 start, glm::vec3 end, float t) {
	return start * (1.0f - t) + end * t; // Linear interpolation
}



void updateCamera(float t, int currentPoint) {
	glm::vec3 start = glm::vec3(tourPoints[currentPoint].x, tourPoints[currentPoint].y, tourPoints[currentPoint].z);
	glm::vec3 end = glm::vec3(tourPoints[currentPoint + 1].x, tourPoints[currentPoint + 1].y, tourPoints[currentPoint + 1].z);

	glm::vec3 cameraPos = interpolate(start, end, t); 

	glm::vec3 target = glm::vec3(tourPoints[currentPoint + 1].tx, tourPoints[currentPoint + 1].ty, tourPoints[currentPoint + 1].tz);
	myCamera.setPosition(cameraPos);
	myCamera.focusOnTarget(target);
}

float t = 0.0f;
int currentPoint = 0;

void animateTour(float deltaTime) {
	if (tourActive) {
		t += deltaTime * 0.5f; 
		if (t >= 1.0f) {
			t = 0.0f;
			currentPoint++;
			if (currentPoint >= tourPoints.size() - 1) {
				tourActive = false; 
			}
		}
		if(tourActive)
			updateCamera(t, currentPoint);
	}
}




GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized to width: %d, height: %d\n", width, height);

	glfwGetFramebufferSize(window, &retina_width, &retina_height);

	glViewport(0, 0, retina_width, retina_height);

	glm::mat4 projection = glm::perspective(
		glm::radians(55.0f),
		(float)retina_width / (float)retina_height,
		0.1f,
		1000.0f
	);

	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		enablePosLight = !enablePosLight;

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		enableRain = !enableRain;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		enableDirLight = !enableDirLight;
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		tourActive = !tourActive; 
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		glm::vec3 position = myCamera.getPosition();
		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		animation = !animation;
	}




	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		currentMode = WIREFRAME;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		currentMode = SOLID;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		currentMode = POINT;
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}


void setRenderingMode() {
	switch (currentMode) {
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		glEnable(GL_POLYGON_OFFSET_LINE);         
		break;

	case SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
		glDisable(GL_POLYGON_OFFSET_LINE);
		break;

	case POINT:
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;

	default:
		break;
	}
}



void initSnow() {
	snowPositions.resize(numSnowdrops);
	glm::vec3 cameraPos = myCamera.getPosition(); 

	for (int i = 0; i < numSnowdrops; ++i) {
		float x = cameraPos.x + static_cast<float>(rand() % 200 - 100) / 10.0f; // Random X around the camera [-10, 10]
		float y = cameraPos.y + static_cast<float>(rand() % 100) / 10.0f + 10.0f; // Random Y above the camera [10, 20]
		float z = cameraPos.z + static_cast<float>(rand() % 200 - 100) / 10.0f; // Random Z around the camera [-10, 10]
		snowPositions[i] = glm::vec3(x, y, z);
	}
}


void updateSnow() {
	glm::vec3 cameraPos = myCamera.getPosition(); 

	for (int i = 0; i < numSnowdrops; ++i) {
		snowPositions[i].y -= 0.2f; 
		if (snowPositions[i].y < cameraPos.y - 5.0f) {
			snowPositions[i].y = cameraPos.y + static_cast<float>(rand() % 100) / 10.0f + 10.0f; 
			snowPositions[i].x = cameraPos.x + static_cast<float>(rand() % 200 - 100) / 10.0f;  
			snowPositions[i].z = cameraPos.z + static_cast<float>(rand() % 200 - 100) / 10.0f;  
		}
	}
}

void renderSnow(gps::Shader shader) {
	shader.useShaderProgram();

	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shader.shaderProgram, "projection");
	GLuint timeLoc = glGetUniformLocation(shader.shaderProgram, "time");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	float currentTime = static_cast<float>(glfwGetTime());
	glUniform1f(timeLoc, currentTime);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::vec3 cameraPos = myCamera.getPosition();
	glm::vec3 cameraFront = glm::normalize(myCamera.getCameraFront());
	glm::vec3 cameraUp = myCamera.getCameraUp();
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

	for (int i = 0; i < numSnowdrops; ++i) {
		glm::vec3 snowdropPos = snowPositions[i];
		float windEffect = static_cast<float>(sin(glfwGetTime() * 0.5)) * 0.05f;
		snowdropPos.x += windEffect;

		glm::vec3 scale = glm::vec3(0.008f, 0.008f, 0.00f);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, snowdropPos);
		model[0] = glm::vec4(cameraRight, 0.0f);
		model[1] = glm::vec4(cameraUp, 0.0f);
		model[2] = glm::vec4(-cameraFront, 0.0f);
		model = glm::scale(model, scale);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		screenQuad.Draw(shader);
	}

	glDisable(GL_BLEND);
}





void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float x_offset = xpos - lastX;
	float y_offset = lastY - ypos; // Inverted y-axis
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	// Pass offsets to the camera's rotate function
	myCamera.rotate(y_offset, x_offset);
}



void processMovement()
{
	if (animation)
	{
		angleZ += 0.8f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 3.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed,deltaTime, ground);		
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed,deltaTime, ground);		
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed,deltaTime, ground);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed,deltaTime,ground);		
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); ///DISABLED/NORMAL

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	ground.LoadModel("objects/objects_scene/final.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	scarecrow.LoadModel("objects/objects_scene/scarecrow.obj");
	campfire.LoadModel("objects/objects_scene/campfire.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/shadow.vert", "shaders/shadow.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	pointLight.loadShader("shaders/lightPoint.vert","shaders/lightPoint.frag");
	pointLight.useShaderProgram();
	rainShader.loadShader("shaders/rain.vert", "shaders/rain.frag");
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 6.0f, 1.0f); //changed should be 1
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set pointLight position
	lightPosPointLight = glm::vec3(11.658f, 1.022f, -7.915f); //unde vreti sa fie lumina punctiforma
	glm::vec3 lightPosEye = glm::vec3(view * glm::vec4(lightPosPointLight, 1.0f));
	lightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosEye));

	enableLoc= glGetUniformLocation(myCustomShader.shaderProgram, "enablePosLight");
	glUniform1i(enableLoc, enablePosLight ? 1 : 0);



	//camera changing
	viewerLoc= glGetUniformLocation(myCustomShader.shaderProgram, "viewerLoc");
	glm::vec3 viewerEye= glm::vec3(view * glm::vec4(myCamera.getPosition(), 1.0f));
	glUniform3fv(viewerLoc, 1, glm::value_ptr(viewerEye));

	//enable directional light
	enableDirLightLoc= glGetUniformLocation(myCustomShader.shaderProgram, "dirLight");
	glUniform1i(enableDirLightLoc, enableDirLight ? 1 : 0);


	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	pointLight.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(pointLight.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	


}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,SHADOW_WIDTH,SHADOW_HEIGHT,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);


	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightRotation * glm::vec4(lightDir,0.0f)), glm::vec3(0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat nearPlane = 0.1f, farPlane=30.0f;
	glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f,nearPlane,farPlane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}
void  initSkybox() {
	faces_day.push_back("skybox/right.tga");
	faces_day.push_back("skybox/left.tga");
	faces_day.push_back("skybox/top.tga");
	faces_day.push_back("skybox/bottom.tga");
	faces_day.push_back("skybox/back.tga");
	faces_day.push_back("skybox/front.tga");
	mySkyBoxDay.Load(faces_day);

	faces_night.push_back("skybox/skybox_night/posx.jpg");
	faces_night.push_back("skybox/skybox_night/negx.jpg");
	faces_night.push_back("skybox/skybox_night/posy.jpg");
	faces_night.push_back("skybox/skybox_night/negy.jpg");
	faces_night.push_back("skybox/skybox_night/posz.jpg");
	faces_night.push_back("skybox/skybox_night/negz.jpg");
	mySkyBoxNight.Load(faces_night);
}
void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	ground.Draw(shader);
	campfire.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(6.817f, 1.859f, -7.965f));
	model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f)); 
	model = glm::translate(model, glm::vec3(-13.635f, -5.7184f, 15.921f));



	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	scarecrow.Draw(shader);

	if (!depthPass)
	{
		if (enablePosLight)
		{
			mySkyBoxNight.Draw(skyboxShader, view, projection);
		}
		else
		mySkyBoxDay.Draw(skyboxShader, view, projection);
	}
}

void renderScene() {

	
	glCullFace(GL_FRONT);
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);




	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);
		
		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)
		
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();


		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		enableLoc= glGetUniformLocation(myCustomShader.shaderProgram, "enablePosLight");
		glUniform1i(enableLoc, enablePosLight ? 1 : 0);

		//directional light enable
		glUniform1i(enableDirLightLoc, enableDirLight ? 1 : 0);


		//fog
		glm::vec3 viewerEye = glm::vec3(view * glm::vec4(myCamera.getPosition(), 1.0f));
		glUniform3fv(viewerLoc, 1, glm::value_ptr(viewerEye));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//lightCube.Draw(lightShader); //decomment to see a white cube around the light

		if (enablePosLight) {
			myCustomShader.useShaderProgram();


			glm::vec3 lightPosEye = glm::vec3(view * glm::vec4(lightPosPointLight, 1.0f));
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosEye));
			

			pointLight.useShaderProgram();

		
			glUniformMatrix4fv(glGetUniformLocation(pointLight.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

			model = glm::mat4(1.0f); 
			model = glm::translate(model, lightPosPointLight); 
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			glUniformMatrix4fv(glGetUniformLocation(pointLight.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			//lightCube.Draw(pointLight); //decomment to see the location of point light as a white cube
		}
		if (enableRain) {
			updateSnow();
			renderSnow(rainShader);
		}
	}
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initSkybox();
	initUniforms();
	initFBO();
	initSnow();


	glCheckError();


	
	while (!glfwWindowShouldClose(glWindow)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processMovement();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (tourActive) {
			animateTour(deltaTime);
		}


		setRenderingMode();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
