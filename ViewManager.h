///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"

// GLFW library
#include "GLFW/glfw3.h" 

enum class ProjectionMode { //enum declaration for perspective toggling
	Perspective,
	Orthographic
};

class ViewManager
{
public:
	// constructor
	ViewManager(
		ShaderManager* pShaderManager);
	// destructor
	~ViewManager();
	void SetupSceneLights();
	// mouse position callback for mouse interaction with the 3D scene
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

	//Chris K edit here
	static void Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset); //Callback function to increase/decrease movement of camera movement speed based on mouse scrolling
	void SetupSceneLights(const glm::vec3& camPosition);

private:


	//Chris K Code here
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool firstMouse = true;

	bool b0rthographicProjection = false;
	float movementSpeed = 0.05f;
	ProjectionMode m_projectionMode = ProjectionMode::Perspective; //private var for handling projection mode
	// pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// active OpenGL display window
	GLFWwindow* m_pWindow;

	// process keyboard events for interaction with the 3D scene
	void ProcessKeyboardEvents();



public:
	// create the initial OpenGL display window
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);
	
	// prepare the conversion from 3D object display to 2D scene display
	void PrepareSceneView();
};