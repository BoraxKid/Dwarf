#include "Transformable.h"

namespace Dwarf
{
    Transformable::Transformable()
    {
        this->setPosition(0, 0, 0);
        this->setRotation(0, 0, 0);
        this->setScale(1, 1, 1);
    }

    Transformable::~Transformable()
    {
    }

    void Transformable::move(double x, double y, double z)
    {
        this->move(glm::dvec3(x, y, z));
    }

    void Transformable::move(const glm::dvec3 &movement)
    {
        this->setPosition(this->_position + movement);
    }

    void Transformable::setPosition(double x, double y, double z)
    {
        this->setPosition(glm::dvec3(x, y, z));
    }

    void Transformable::setPosition(const glm::dvec3 &position)
    {
        this->_position = position;
        this->_positionMatrix = glm::translate(glm::dmat4(), this->_position);
        this->updateTransformationMatrix();
    }

    void Transformable::scale(double x, double y, double z)
    {
        this->scale(glm::dvec3(x, y, z));
    }

    void Transformable::scale(const glm::dvec3 &scale)
    {
        this->setScale(this->_scale + scale);
    }

    void Transformable::setScale(double x, double y, double z)
    {
        this->setScale(glm::dvec3(x, y, z));
    }

    void Transformable::setScale(const glm::dvec3 &scale)
    {
        this->_scale = scale;
        this->_scaleMatrix = glm::scale(glm::dmat4(), this->_scale);
        this->updateTransformationMatrix();
    }

    void Transformable::rotate(double x, double y, double z)
    {
        this->rotate(glm::dvec3(x, y, z));
    }

    void Transformable::rotate(const glm::dvec3 &rotation)
    {
        this->setRotation(this->_rotation + rotation);
    }

    void Transformable::setRotation(double x, double y, double z)
    {
        this->setRotation(glm::dvec3(x, y, z));
    }

    void Transformable::setRotation(const glm::dvec3 &rotation)
    {
        this->_rotation = rotation;
        this->_rotationMatrix = glm::rotate(glm::dmat4(), glm::radians(this->_rotation.x), glm::dvec3(1.0, 0.0, 0.0));
        this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(this->_rotation.y), glm::dvec3(0.0, 1.0, 0.0));
        this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(this->_rotation.z), glm::dvec3(0.0, 0.0, 1.0));
        this->updateTransformationMatrix();
    }

    void Transformable::updateTransformationMatrix()
    {
        this->_preciseTransformationMatrix = this->_scaleMatrix * this->_rotationMatrix * this->_positionMatrix;
        this->_transformationMatrix = this->_preciseTransformationMatrix;
    }
}
