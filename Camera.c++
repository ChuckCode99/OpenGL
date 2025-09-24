#include "Camera.h++"

Camera::Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f,  3.0f), glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f))
	: YAW(-90.0f), PITCH(0.0f), ZOOM(45.0f), SPEED(2.5f), SENSITIVITY(0.1f),
	  m_DeltaTime(0.0f), m_LastFrame(0.0f), m_Yaw(YAW), m_Pitch(PITCH), m_Zoom(ZOOM),
	  m_Position(Position), m_WorldUp(WorldUp), m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	updateCameraVectors();
}

glm::mat4 Camera::viewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::deltaUpdate()
{
	float currentFrame{ static_cast<float>(glfwGetTime()) };
	m_DeltaTime = currentFrame - m_LastFrame;
	m_LastFrame = currentFrame;
}

void Camera::keyboardInput(Direction Dir)
{
	float velocity{ SPEED * m_DeltaTime };

	if (Dir == Direction::FORWARD)
		m_Position += m_Front * velocity;
	if (Dir == Direction::BACKWARD)
		m_Position -= m_Front * velocity;
	if (Dir == Direction::RIGHT)
		m_Position += m_Right * velocity;
	if (Dir == Direction::LEFT)
		m_Position -= m_Right * velocity;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::mouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
	xoffset *= SENSITIVITY;
	yoffset *= SENSITIVITY;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::mouseScroll(float yoffset)
{
	m_Zoom -= (float)yoffset;
	if (m_Zoom < 1.0f)
		m_Zoom = 1.0f;
	if (m_Zoom > 45.0f)
		m_Zoom = 45.0f;
}
