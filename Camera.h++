#pragma once

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

extern const int WINDOW_DEFAULT_HEIGHT;
extern const int WINDOW_DEFAULT_WIDTH;

class Camera
{
private:
	const float YAW;		//  -90.0f;
	const float PITCH;		//  0.0f;
	const float ZOOM;		//  45.0f;
	const float SPEED;		//  2.5f;
	const float SENSITIVITY;//  0.1f;


	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_WorldUp;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	float m_Yaw;
	float m_Pitch;
	float m_DeltaTime;		// Time between current frame and last frame
	float m_LastFrame;		// Time of last frame

	void updateCameraVectors();

public:
	float m_Zoom;
	enum class Direction { FORWARD, BACKWARD, LEFT,	RIGHT };

	Camera(glm::vec3, glm::vec3);
	glm::mat4 viewMatrix();
	void deltaUpdate();
	void keyboardInput(Direction);
	void mouseMovement(float, float, GLboolean);
	void mouseScroll(float);
};

