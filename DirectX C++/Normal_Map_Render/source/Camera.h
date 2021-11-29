//
//  Camera.h
//
//	Basic camera class
//

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "vec\vec.h"
#include "vec\mat.h"

using namespace linalg;

class camera_t
{
	float vfov, aspect;	// aperture attributes
	float zNear, zFar;	// clip planes
	vec4f position;
	vec3f direction;
	float angle_X = 0.0f;
	float angle_Y = 0.0f;

public:

	camera_t(
		float vfov,
		float aspect,
		float zNear,
		float zFar
        ) :
		vfov(vfov), aspect(aspect), zNear(zNear), zFar(zFar)
	{

	}

	vec3f GetPos()
	{
		return position.xyz();
	}

	void moveTo(const vec3f& p)
	{
		position = p.xyz0();
	}

	void move(const vec3f& v)
	{
		position += get_ViewToWorldMatrix() * v.xyz0();
	}

	void SetAngleY(float degrees_to_add)
	{
		angle_Y += degrees_to_add;
	}

	void SetAngleX(float degrees_to_add)
	{
		angle_X += degrees_to_add;
	}

	mat4f get_WorldToViewMatrix() const
	{
		mat4f rotation = mat4f::rotation(0, angle_X, angle_Y);
		rotation.transpose();

		mat4f translation = mat4f::translation(-position.xyz());
		return rotation * translation;
	}

	mat4f get_ViewToWorldMatrix() const
	{
		return mat4f::translation(position.xyz()) * mat4f::rotation(0, angle_X, angle_Y);
	}

	mat4f get_ProjectionMatrix() const
	{
		return mat4f::projection(vfov, aspect, zNear, zFar);
	}
};
#endif

