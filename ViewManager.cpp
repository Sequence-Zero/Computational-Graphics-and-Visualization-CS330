///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>





// declaration of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";




	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f; 
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager *pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();
	// default camera view parameters
	g_pCamera->Position = glm::vec3(-4.0f, 3.0f, 6.0f); //-x to position to the left, +y to position slightly above scene, and pulled back with +z
	g_pCamera->Front = glm::vec3(2.5f, -1.0f, -5.0f); //Looking toward center right with +x, downward with -y, and deeper into scene with -z
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f); //Unchanged, standard camera direction
	g_pCamera->Zoom = 60.0f; //Tightened down from 80
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture all mouse events
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// this callback is used to receive mouse moving events
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

	m_pWindow = window; //assigning the window

	glfwSetScrollCallback(m_pWindow, &ViewManager::Mouse_Scroll_Callback);

	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{


	//ChrisK Code here

	static bool firstMouse = true; //initializing a mouse tracking state on first movement
	static float lastX = 400.0f;
	static float lastY = 300.0f;


	if (firstMouse)
	{
		lastX = (float)xMousePos;
		lastY = (float)yMousePos;
		firstMouse = false;
	}

	float xoffset = (float)(xMousePos - lastX); //Calculate offset since last mouse position
	float yoffset = (float)(lastY - yMousePos); // reversed since y-coordinates go bottom to top

	lastX = (float)xMousePos; //Storing current position of x and y for next frame
	lastY = (float)yMousePos;

	float sensitivity = 0.1f; //Apply sensitivity and update camera pitch & yaw
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	g_pCamera->Yaw += xoffset;
	g_pCamera->Pitch += yoffset;

	//Setting constraints for pitch to avoid flipping effect
	if (g_pCamera->Pitch > 89.0f)
		g_pCamera->Pitch = 89.0f;
	if (g_pCamera->Pitch < -89.0f)
		g_pCamera->Pitch = -89.0f;

	// Convert updated pitch and yaw to new direction vector
	glm::vec3 front;
	front.x = cos(glm::radians(g_pCamera->Yaw)) * cos(glm::radians(g_pCamera->Pitch));
	front.y = sin(glm::radians(g_pCamera->Pitch));
	front.z = sin(glm::radians(g_pCamera->Yaw)) * cos(glm::radians(g_pCamera->Pitch));
	g_pCamera->Front = glm::normalize(front);
}

void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{

	static float movementSpeed = 0.05f;
	// Adjust movement speed for camera navigation using scroll input
	movementSpeed += static_cast<float>(yoffset) * 0.01f;

	//Limiting the speed to avoid going too slow or too fast
	if (movementSpeed < 0.01f)
		movementSpeed = 0.01f;
	if (movementSpeed > 1.0f)
		movementSpeed = 1.0f;
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// close the window if the escape key has been pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}

	//Chris K Code here

	float velocity = 0.05f;  //for consistent movement

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
		g_pCamera->Position += velocity * g_pCamera->Front;
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
		g_pCamera->Position -= velocity * g_pCamera->Front;
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
		g_pCamera->Position -= glm::normalize(glm::cross(g_pCamera->Front, g_pCamera->Up)) * velocity;
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
		g_pCamera->Position += glm::normalize(glm::cross(g_pCamera->Front, g_pCamera->Up)) * velocity;
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
		g_pCamera->Position.y += velocity;
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
		g_pCamera->Position.y -= velocity;

	// Projection toggle
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS) //if key pressed is p, perspective view
		bOrthographicProjection = false;
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS) //if key pressed is o, orthographic view
		bOrthographicProjection = true;

}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	if (bOrthographicProjection)
	{
		float scale = 2.0f; //Defining an orthographic projection box
		projection = glm::ortho(-scale, scale, -scale, scale, 0.1f, 100.0f); //used for a 2D style flat projection
	}
	else
	{
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), 800.0f / 600.0f, 0.1f, 100.0f); //Defines a perspective projection matrix
		//Used for a realistic 3D perspective
	}

	//view = glm::lookAt(g_pCamera->Position, g_pCamera->Position + g_pCamera->Front, g_pCamera->Up);


	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the 
	// event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
		SetupSceneLights(g_pCamera->Position);
	}
}
void ViewManager::SetupSceneLights(const glm::vec3& camPosition) //Chris K Extraneous Light setup Code here
{
	if (!m_pShaderManager)
		return;

	
	m_pShaderManager->setVec3Value("light.position", g_pCamera->Position); //Spotlight properties (coming from camera)
	m_pShaderManager->setVec3Value("light.direction", g_pCamera->Front);
	m_pShaderManager->setFloatValue("light.cutOff", glm::cos(glm::radians(12.5f)));
	m_pShaderManager->setFloatValue("light.outerCutOff", glm::cos(glm::radians(15.0f)));

	
	m_pShaderManager->setVec3Value("light.ambient", glm::vec3(0.1f)); //Spotlight color
	m_pShaderManager->setVec3Value("light.diffuse", glm::vec3(0.8f));
	m_pShaderManager->setVec3Value("light.specular", glm::vec3(1.0f));

	
	m_pShaderManager->setFloatValue("light.constant", 1.0f); //Attenuation
	m_pShaderManager->setFloatValue("light.linear", 0.09f);
	m_pShaderManager->setFloatValue("light.quadratic", 0.032f);

	m_pShaderManager->setVec3Value("pointLights[0].position", glm::vec3(2.0f, 2.0f, 2.0f));
	m_pShaderManager->setVec3Value("pointLights[0].ambient", glm::vec3(0.2f, 0.0f, 0.2f)); // Dim purple
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", glm::vec3(0.5f, 0.0f, 0.5f)); // Stronger purple
	m_pShaderManager->setVec3Value("pointLights[0].specular", glm::vec3(0.8f, 0.0f, 0.8f));
	m_pShaderManager->setFloatValue("pointLights[0].constant", 1.0f);
	m_pShaderManager->setFloatValue("pointLights[0].linear", 0.09f);
	m_pShaderManager->setFloatValue("pointLights[0].quadratic", 0.032f);
}