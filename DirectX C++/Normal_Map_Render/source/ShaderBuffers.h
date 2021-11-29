
#pragma once
#ifndef MATRIXBUFFERS_H
#define MATRIXBUFFERS_H

#include "vec\vec.h"
#include "vec\mat.h"

using namespace linalg;

struct MatrixBuffer_t
{
	mat4f ModelToWorldMatrix;
	mat4f WorldToViewMatrix;
	mat4f ProjectionMatrix;
};

struct material_own
{
	//Ka = ambient. Kd = Diffuse. Ks = Specular. 
	vec4f Ka, Kd, Ks;
};


struct camera_light
{
	vec4f light_pos;
	vec4f camera_pos;
	//vec2f padding;
};

#endif