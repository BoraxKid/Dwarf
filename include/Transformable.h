#ifndef DWARF_TRANSFORMABLE_H_
#define DWARF_TRANSFORMABLE_H_
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Dwarf
{
    class Transformable
    {
    public:
        Transformable();
        virtual ~Transformable();

        void move(double x, double y, double z);
        void move(const glm::dvec3 &movement);
        void setPosition(double x, double y, double z);
        void setPosition(const glm::dvec3 &position);
        void scale(double x, double y, double z);
        void scale(const glm::dvec3 &scale);
        void setScale(double x, double y, double z);
        void setScale(const glm::dvec3 &scale);
        void rotate(double x, double y, double z);
        void rotate(const glm::dvec3 &rotation);
        void setRotation(double x, double y, double z);
        void setRotation(const glm::dvec3 &rotation);

    protected:
        glm::dvec3 _position;
        glm::dvec3 _rotation;
        glm::dvec3 _scale;
        glm::dmat4 _positionMatrix;
        glm::dmat4 _rotationMatrix;
        glm::dmat4 _scaleMatrix;
        glm::dmat4 _preciseTransformationMatrix;
        glm::mat4 _transformationMatrix;

    private:
        void updateTransformationMatrix();
    };
}

#endif // DWARF_TRANSFORMABLE_H_
