#include "Light.h"

namespace Dwarf
{
    Light::Light()
        : _position(0.0, 0.0, 0.0), _radius(0.0)
    {
        this->updateUniformBuffer();
    }

    Light::Light(double x, double y, double z, double radius)
        : _position(x, y, z), _radius(radius)
    {
        this->updateUniformBuffer();
    }

    Light::Light(double x, double y, double z, const Color &color, double radius)
        : _position(x, y, z), _color(color), _radius(radius)
    {
        this->updateUniformBuffer();
    }

    Light::~Light()
    {
    }

    void Light::move(double x, double y, double z)
    {
        this->move(glm::dvec3(x, y, z));
    }

    void Light::move(const glm::dvec3 &movement)
    {
        this->setPosition(this->_position + movement);
    }

    void Light::setPosition(double x, double y, double z)
    {
        this->setPosition(glm::dvec3(x, y, z));
    }

    void Light::setPosition(const glm::dvec3 &position)
    {
        this->_position = position;
        this->_uniformBuffer.position = glm::vec4(this->_position, this->_radius);
    }

    void Light::setColor(const Color &color)
    {
        this->_color = color;
        this->_uniformBuffer.color = this->_color.getColor();
    }

    void Light::changeRadius(double radius)
    {
        this->setRadius(this->_radius + radius);
    }

    void Light::setRadius(double radius)
    {
        this->_radius = radius;
        this->_uniformBuffer.position.w = static_cast<float>(this->_radius);
    }

    const LightUniformBuffer &Light::getUniformBuffer() const
    {
        return (this->_uniformBuffer);
    }

    void Light::updateUniformBuffer()
    {
        this->_uniformBuffer.position = glm::vec4(this->_position, this->_radius);
        this->_uniformBuffer.color = this->_color.getColor();
    }
}
