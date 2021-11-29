//
//  Geometry.h
//
//  (c) CJG 2016, cjgribel@gmail.com
//

#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "ShaderBuffers.h"
#include "drawcall.h"
#include "mesh.h"

using namespace linalg;

class Geometry_t
{
protected:

	// pointers to device vertex & index arrays
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;
	ID3D11SamplerState* sampler_state = nullptr;
	ID3D11SamplerState* sampler_state2 = nullptr;


public:

	Geometry_t()
	{

	}

	Geometry_t(ID3D11Device* device)
	{
		HRESULT hr;
		D3D11_SAMPLER_DESC sd =
		{
			D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP ,
			D3D11_TEXTURE_ADDRESS_WRAP,
			0.0f,
			4,
			D3D11_COMPARISON_NEVER,
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			-FLT_MAX,
			FLT_MAX,
		};
		ASSERT(hr = device->CreateSamplerState(&sd, &sampler_state));
	
	}

	//
	// Map and update the matrix buffer
	//
	virtual void MapMatrixBuffers(
		ID3D11DeviceContext* device_context,
		ID3D11Buffer* matrix_buffer,
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);	

	virtual void MapMaterialBuffers(
		ID3D11DeviceContext* device_context,
		ID3D11Buffer* material_buffer,
		vec4f Ka, vec4f Kd, vec4f Ks);
	
	void CalculateTangents(vertex_t &v0, vertex_t &v1, vertex_t &v2);

	virtual void render(ID3D11DeviceContext* device_context) const = 0;

	virtual ~Geometry_t()
	{ 
		// release the Krak-..device buffers
		SAFE_RELEASE(vertex_buffer);
		SAFE_RELEASE(index_buffer);
		SAFE_RELEASE(sampler_state);
		SAFE_RELEASE(sampler_state2);
	}
};

class Quad_t : public Geometry_t
{
	// our local vertex and index arrays
	std::vector<vertex_t> vertices;
	std::vector<unsigned> indices;
	unsigned nbr_indices = 0;

public:

	Quad_t(ID3D11Device* device);

	void render(ID3D11DeviceContext* device_context) const;

	~Quad_t() { }
};

class Cube_t : public Geometry_t
{
	// our local vertex and index arrays
	std::vector<vertex_t> vertices;
	std::vector<unsigned> indices;
	unsigned nbr_indices = 0;

public:

	Cube_t(ID3D11Device* device);

	void render(ID3D11DeviceContext* device_context) const;

	~Cube_t() { }
};

class Cube2_t : public Geometry_t
{
	// our local vertex and index arrays
	std::vector<vertex_t> vertices;
	std::vector<unsigned> indices;
	unsigned nbr_indices = 0;

public:

	Cube2_t(ID3D11Device* device);

	void render(ID3D11DeviceContext* device_context) const;

	~Cube2_t() { }
};

class Triangle_t : public Geometry_t
{
	// our local vertex and index arrays
	std::vector<vertex_t> vertices;
	std::vector<unsigned> indices;
	unsigned nbr_indices = 0;

public:

	Triangle_t(ID3D11Device* device);

	void render(ID3D11DeviceContext* device_context) const;

	~Triangle_t() { }
};


class OBJModel_t : public Geometry_t
{
	// index ranges, representing drawcalls, within an index array
	struct index_range_t
	{
		size_t start;
		size_t size;
		unsigned ofs;
		int mtl_index;
	};

	std::vector<index_range_t> index_ranges;
	std::vector<material_t> materials;

	void append_materials(const std::vector<material_t>& mtl_vec)
	{
		materials.insert(materials.end(), mtl_vec.begin(), mtl_vec.end());
	}

public:

	OBJModel_t(
		const std::string& objfile,
		ID3D11Device* device);

	void render(ID3D11DeviceContext* device_context) const;

	void renderObj(ID3D11DeviceContext* device_context, ID3D11Buffer* material_buffer);

	~OBJModel_t()
	{ 
		for (auto& mtl : materials)
		{
			SAFE_RELEASE(mtl.map_Kd_Tex);
			SAFE_RELEASE(mtl.map_Kd_TexSRV);
			SAFE_RELEASE(mtl.map_Ks_Tex);
			SAFE_RELEASE(mtl.map_Ks_TexSRV);
			SAFE_RELEASE(mtl.map_d_Tex);
			SAFE_RELEASE(mtl.map_d_TexSRV);
			SAFE_RELEASE(mtl.map_bump_Tex);
			SAFE_RELEASE(mtl.map_bump_TexSRV);
		}
		SAFE_RELEASE(sampler_state);
	}
};

#endif