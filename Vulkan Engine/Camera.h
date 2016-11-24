#ifndef CAMERA_H_
#define CAMERA_H_
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace Dwarf
{
	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		void update(float deltaTime);
		void updateAspectRation(float aspect);
		void translate(glm::vec3 delta);
		void rotate(glm::vec3 delta);
		void setRotation(glm::vec3 rotation);
        void setCameraSpeed(float movementSpeed);
		const glm::mat4 &getMVP() const;

		bool _left;
		bool _right;
		bool _up;
		bool _down;

	private:
		void updateViewMatrix();

		float _rotationSpeed;
		float _movementSpeed;
		float _fov;
		float _zNear;
		float _zFar;
		glm::mat4 _mvp;
		glm::mat4 _perspective;
		glm::mat4 _view;
		glm::vec3 _position;
		glm::vec3 _rotation;
	};
}

#endif // CAMERA_H_
