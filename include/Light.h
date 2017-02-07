#ifndef DWARF_LIGHT_H_
#define DWARF_LIGHT_H_
#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Color.h"

namespace Dwarf
{
    struct LightUniformBuffer
    {
        glm::vec4 position;
        glm::vec3 color;
    };

    class Light
    {
    public:
        Light();
        Light(double x, double y, double z, double radius = 1.0);
        Light(double x, double y, double z, const Color &color, double radius = 1.0);
        virtual ~Light();

        void move(double x, double y, double z);
        void move(const glm::dvec3 &movement);
        void setPosition(double x, double y, double z);
        void setPosition(const glm::dvec3 &position);
        void setColor(const Color &color);
        void changeRadius(double radius);
        void setRadius(double radius);
        const LightUniformBuffer &getUniformBuffer() const;

    private:
        void updateUniformBuffer();

        glm::dvec3 _position;
        Color _color;
        double _radius;
        LightUniformBuffer _uniformBuffer;
    };
}

#endif // DWARF_LIGHT_H_
