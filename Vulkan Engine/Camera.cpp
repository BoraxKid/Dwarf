#include "Camera.h"
#include "easylogging++.h"

namespace Dwarf
{
	Camera::Camera()
		: _rotationSpeed(1.0f), _movementSpeed(1.0f), _fov(60), _zNear(0.1f), _zFar(100000.0f), _left(false), _right(false), _up(false), _down(false)
	{
		this->_position = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Camera::~Camera()
	{
	}

	void Camera::update(float deltaTime)
	{
		float rotX = glm::radians(this->_rotation.x);
		float rotY = glm::radians(this->_rotation.y);
		float moveSpeed = deltaTime * this->_movementSpeed;
		glm::vec3 camFront(-cos(rotX) * sin(rotY), sin(rotX), cos(rotX) * cos(rotY));
		camFront = glm::normalize(camFront);
		if (this->_up)
			this->_position += camFront * moveSpeed;
		if (this->_down)
			this->_position -= camFront * moveSpeed;
		if (this->_left)
			this->_position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
		if (this->_right)
			this->_position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
		this->updateViewMatrix();
	}

	void Camera::updateAspectRation(float aspect)
	{
		this->_perspective = glm::perspective(glm::radians(this->_fov), aspect, this->_zNear, this->_zFar);
	}

	void Camera::translate(glm::vec3 delta)
	{
		this->_position += delta;
		this->updateViewMatrix();
	}

	void Camera::rotate(glm::vec3 delta)
	{
		this->_rotation += delta;
		//if (this->_rotation.x <= 10.0f)
		//	this->_rotation.x = 10.0f;
		//else if (this->_rotation.x >= 260.0f)
		//	this->_rotation.x = 260.0f;
		if (this->_rotation.x >= 360.0f)
			this->_rotation.x -= 360.0f;
		else if (this->_rotation.x <= 0.0f)
			this->_rotation.x += 360.0f;
		if (this->_rotation.y >= 360.0f)
			this->_rotation.y -= 360.0f;
		else if (this->_rotation.y <= 0.0f)
			this->_rotation.y += 360.0f;
		this->updateViewMatrix();
	}

	void Camera::setRotation(glm::vec3 rotation)
	{
		this->_rotation = rotation;
		this->updateViewMatrix();
	}

	glm::mat4 Camera::getMVP() const
	{
		return (this->_mvp);
	}

	void Camera::updateViewMatrix()
	{
		glm::mat4 rotM = glm::mat4();
		glm::mat4 transM;

		rotM = glm::rotate(rotM, glm::radians(this->_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(this->_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(this->_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		transM = glm::translate(glm::mat4(), this->_position);

		this->_view = rotM * transM;
		this->_mvp = this->_perspective * this->_view;
	}
}
