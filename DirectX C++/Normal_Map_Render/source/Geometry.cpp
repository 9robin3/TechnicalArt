
#include "Geometry.h"


void Geometry_t::MapMatrixBuffers(
	ID3D11DeviceContext* device_context,
	ID3D11Buffer* matrix_buffer,
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// map the resource buffer, obtain a pointer to it and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	device_context->Map(matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MatrixBuffer_t* matrix_buffer_ = (MatrixBuffer_t*)resource.pData;
	matrix_buffer_->ModelToWorldMatrix = ModelToWorldMatrix;
	matrix_buffer_->WorldToViewMatrix = WorldToViewMatrix;
	matrix_buffer_->ProjectionMatrix = ProjectionMatrix;
	device_context->Unmap(matrix_buffer, 0);
}

void Geometry_t::MapMaterialBuffers(
	ID3D11DeviceContext* g_DeviceContext,
	ID3D11Buffer* material_buffer,
	vec4f Ka, vec4f Kd, vec4f Ks)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	g_DeviceContext->Map(material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	material_own* material_buffer_ = (material_own*)resource.pData;
	material_buffer_->Ka = Ka;
	material_buffer_->Kd = Kd;
	material_buffer_->Ks = Ks;
	g_DeviceContext->Unmap(material_buffer, 0);
}

Quad_t::Quad_t(ID3D11Device* device) :Geometry_t(device)
{
	// populate the vertex array with 4 vertices
	//(14 vertices for a cube)
	vertex_t v0, v1, v2, v3;
	v0.Pos = { 1, 0, 0.0f };
	v0.Normal = { 0, 0, 1 };
	v0.TexCoord = { 0, 0 };
	v1.Pos = { 2, 0, 0.0f };
	v1.Normal = { 0, 0, 1 };
	v1.TexCoord = { 0, 1 };
	v2.Pos = { 2.0f, 1.0f, 0.0f };
	v2.Normal = { 0, 0, 1 };
	v2.TexCoord = { 1, 1 };
	v3.Pos = { 1, 1.0f, 0.0f };
	v3.Normal = { 0, 0, 1 };
	v3.TexCoord = { 0, 0 };
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	// populate the index array
	// triangle #1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	// triangle #2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0.0f };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = vertices.size()*sizeof(vertex_t);
	// data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// create vertex buffer on device using descriptor & data
	HRESULT vhr = device->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);

	//  index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0.0f };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = indices.size()*sizeof(unsigned);
	// data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// create index buffer on device using descriptor & data
	HRESULT ihr = device->CreateBuffer(&ibufferDesc, &idata, &index_buffer);

	// local data is now loaded to device so it can be released
	vertices.clear();
	nbr_indices = indices.size();
	indices.clear();
}


void Quad_t::render(ID3D11DeviceContext* device_context) const
{
	//set topology
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind our vertex buffer
	UINT32 stride = sizeof(vertex_t); //  sizeof(float) * 8;
	UINT32 offset = 0;
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// bind our index buffer
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// make the drawcall
	device_context->DrawIndexed(nbr_indices, 0, 0);
}

Cube_t::Cube_t(ID3D11Device* device) :Geometry_t(device)
{
	// populate the vertex array with 4 vertices
	vertex_t v00, v01, v02, v10, v11, v12, v20, v21, v22, v30, v31, v32, v40, v41, v42, v50, v51, v52, v60, v61, v62, v70, v71, v72;
	//index

	v00.Pos = { -0.5f, -0.5f, 0.0f }; //0
	v00.Normal = { 0, 0, 1 };
	v00.TexCoord = { 0, 0 };
	v01.Pos = { -0.5, -0.5f, 0.0f }; //1
	v01.Normal = { 0, -1, 0 };
	v01.TexCoord = { 0, 0 };
	v02.Pos = { -0.5, -0.5f, 0.0f }; //2
	v02.Normal = { -1, 0, 0 };
	v02.TexCoord = { 1, 0};

	v10.Pos = { 0.5, -0.5f, 0.0f };  //3
	v10.Normal = { 0, 0, 1 };
	v10.TexCoord = { 1, 0 };
	v11.Pos = { 0.5, -0.5f, 0.0f };  //4
	v11.Normal = { 0, -1, 0 };
	v11.TexCoord = { 1, 0 };
	v12.Pos = { 0.5, -0.5f, 0.0f };  //5
	v12.Normal = { 1, 0, 0 };
	v12.TexCoord = { 1, 0 };

	v20.Pos = { 0.5, 0.5f, 0.0f };  //6
	v20.Normal = { 0, 0, 1 };
	v20.TexCoord = { 0, 1 };
	v21.Pos = { 0.5, 0.5f, 0.0f };  //7
	v21.Normal = { 0, 1, 0 };
	v21.TexCoord = { 1, 0 };
	v22.Pos = { 0.5, 0.5f, 0.0f };  //8
	v22.Normal = { 1, 0, 0 };
	v22.TexCoord = { 1, 1 };

	v30.Pos = { -0.5, 0.5f, 0.0f };  //9
	v30.Normal = { 0, 0, 1 };
	v30.TexCoord = { 1, 1 };
	v31.Pos = { -0.5, 0.5f, 0.0f };  //10
	v31.Normal = { 0, 1, 0 };
	v31.TexCoord = { 0, 0 };
	v32.Pos = { -0.5, 0.5f, 0.0f };  //11
	v32.Normal = { -1, 0, 0 };
	v32.TexCoord = { 1, 1 };

	v40.Pos = { -0.5, -0.5f, -1.0f }; //12
	v40.Normal = { 0, 0, -1 };
	v40.TexCoord = { 1, 1 };
	v41.Pos = { -0.5, -0.5f, -1.0f }; //13
	v41.Normal = { 0, -1, 0 };
	v41.TexCoord = { 0, 1 };
	v42.Pos = { -0.5, -0.5f, -1.0f }; //14
	v42.Normal = { -1, 0, 0 };
	v42.TexCoord = { 0, 0 };

	v50.Pos = { 0.5, -0.5f, -1.0f }; //15
	v50.Normal = { 0, 0, -1 };
	v50.TexCoord = { 0, 1 };
	v51.Pos = { 0.5, -0.5f, -1.0f }; //16
	v51.Normal = { 0, -1, 0 };
	v51.TexCoord = { 1, 1 };
	v52.Pos = { 0.5, -0.5f, -1.0f }; //17
	v52.Normal = { 1, 0, 0 };
	v52.TexCoord = { 0, 0 };

	v60.Pos = { 0.5, 0.5f, -1.0f };  //18
	v60.Normal = { 0, 0, -1 };
	v60.TexCoord = { 1, 0 };
	v61.Pos = { 0.5, 0.5f, -1.0f };  //19
	v61.Normal = { 0, 1, 0 };
	v61.TexCoord = { 0, 1 };
	v62.Pos = { 0.5, 0.5f, -1.0f };  //20
	v62.Normal = { 1, 0, 0 };
	v62.TexCoord = { 0, 1 };

	v70.Pos = { -0.5, 0.5f, -1.0f }; //21
	v70.Normal = { 0, 0, -1 };
	v70.TexCoord = { 0, 0 };
	v71.Pos = { -0.5, 0.5f, -1.0f }; //22
	v71.Normal = { 0, 1, 0 };
	v71.TexCoord = { 1, 1 };
	v72.Pos = { -0.5, 0.5f, -1.0f }; //23
	v72.Normal = { -1, 0, 0 };
	v72.TexCoord = { 0, 1 };

	vertices.push_back(v00);
	vertices.push_back(v01);
	vertices.push_back(v02);

	vertices.push_back(v10);
	vertices.push_back(v11);
	vertices.push_back(v12);

	vertices.push_back(v20);
	vertices.push_back(v21);
	vertices.push_back(v22);

	vertices.push_back(v30);
	vertices.push_back(v31);
	vertices.push_back(v32);

	vertices.push_back(v40);
	vertices.push_back(v41);
	vertices.push_back(v42);

	vertices.push_back(v50);
	vertices.push_back(v51);
	vertices.push_back(v52);

	vertices.push_back(v60);
	vertices.push_back(v61);
	vertices.push_back(v62);

	vertices.push_back(v70);
	vertices.push_back(v71);
	vertices.push_back(v72);

	// populate the index array
	// triangle #1
	// normal Z
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(9);

	// triangle #2
	//Z
	indices.push_back(3);
	indices.push_back(6);
	indices.push_back(9);

	// triangle #3
	//Z
	indices.push_back(21);
	indices.push_back(18);
	indices.push_back(12);

	// triangle #4
	//Z
	indices.push_back(18);
	indices.push_back(15);
	indices.push_back(12);

	// triangle #5
	//X
	indices.push_back(23);
	indices.push_back(2);
	indices.push_back(11);

	// triangle #6
	//X
	indices.push_back(23);
	indices.push_back(14);
	indices.push_back(2);

	// triangle #7
	//X
	indices.push_back(17);
	indices.push_back(20);
	indices.push_back(8);

	// triangle #8
	//X
	indices.push_back(17);
	indices.push_back(8);
	indices.push_back(5);

	// triangle #9
	//Y
	indices.push_back(19);
	indices.push_back(10);
	indices.push_back(7);

	// triangle #10
	//Y
	indices.push_back(10);
	indices.push_back(19);
	indices.push_back(22);

	// triangle #11
	//Y
	indices.push_back(1);
	indices.push_back(13);
	indices.push_back(16);

	// triangle #12
	//Y
	indices.push_back(4);
	indices.push_back(1);
	indices.push_back(16);

	// vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0.0f };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = vertices.size()*sizeof(vertex_t);
	// data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// create vertex buffer on device using descriptor & data
	HRESULT vhr = device->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);

	//  index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0.0f };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = indices.size()*sizeof(unsigned);
	// data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// create index buffer on device using descriptor & data
	HRESULT ihr = device->CreateBuffer(&ibufferDesc, &idata, &index_buffer);

	// local data is now loaded to device so it can be released
	vertices.clear();
	nbr_indices = indices.size();
	indices.clear();
}


void Cube_t::render(ID3D11DeviceContext* device_context) const
{
	//set topology
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind our vertex buffer
	UINT32 stride = sizeof(vertex_t); //  sizeof(float) * 8;
	UINT32 offset = 0;
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// bind our index buffer
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// make the drawcall
	device_context->DrawIndexed(nbr_indices, 0, 0);
}

Cube2_t::Cube2_t(ID3D11Device* device) :Geometry_t(device)
{

	// populate the vertex array with 4 vertices
	vertex_t v00, v01, v02, v10, v11, v12, v20, v21, v22, v30, v31, v32, v40, v41, v42, v50, v51, v52, v60, v61, v62, v70, v71, v72;
	//index

	v00.Pos = { -0.5f, -0.5f, 0.0f }; //0
	v00.Normal = { 0, 0, 1 };
	v00.TexCoord = { 0, 0 };
	v01.Pos = { -0.5, -0.5f, 0.0f }; //1
	v01.Normal = { 0, -1, 0 };
	v01.TexCoord = { 0, 0 };
	v02.Pos = { -0.5, -0.5f, 0.0f }; //2
	v02.Normal = { -1, 0, 0 };
	v02.TexCoord = { 1, 0 };

	v10.Pos = { 0.5, -0.5f, 0.0f };  //3
	v10.Normal = { 0, 0, 1 };
	v10.TexCoord = { 1, 0 };
	v11.Pos = { 0.5, -0.5f, 0.0f };  //4
	v11.Normal = { 0, -1, 0 };
	v11.TexCoord = { 1, 0 };
	v12.Pos = { 0.5, -0.5f, 0.0f };  //5
	v12.Normal = { 1, 0, 0 };
	v12.TexCoord = { 1, 0 };

	v20.Pos = { 0.5, 0.5f, 0.0f };  //6
	v20.Normal = { 0, 0, 1 };
	v20.TexCoord = { 0, 1 };
	v21.Pos = { 0.5, 0.5f, 0.0f };  //7
	v21.Normal = { 0, 1, 0 };
	v21.TexCoord = { 1, 0 };
	v22.Pos = { 0.5, 0.5f, 0.0f };  //8
	v22.Normal = { 1, 0, 0 };
	v22.TexCoord = { 1, 1 };

	v30.Pos = { -0.5, 0.5f, 0.0f };  //9
	v30.Normal = { 0, 0, 1 };
	v30.TexCoord = { 1, 1 };
	v31.Pos = { -0.5, 0.5f, 0.0f };  //10
	v31.Normal = { 0, 1, 0 };
	v31.TexCoord = { 0, 0 };
	v32.Pos = { -0.5, 0.5f, 0.0f };  //11
	v32.Normal = { -1, 0, 0 };
	v32.TexCoord = { 1, 1 };

	v40.Pos = { -0.5, -0.5f, -1.0f }; //12
	v40.Normal = { 0, 0, -1 };
	v40.TexCoord = { 1, 1 };
	v41.Pos = { -0.5, -0.5f, -1.0f }; //13
	v41.Normal = { 0, -1, 0 };
	v41.TexCoord = { 0, 1 };
	v42.Pos = { -0.5, -0.5f, -1.0f }; //14
	v42.Normal = { -1, 0, 0 };
	v42.TexCoord = { 0, 0 };

	v50.Pos = { 0.5, -0.5f, -1.0f }; //15
	v50.Normal = { 0, 0, -1 };
	v50.TexCoord = { 0, 1 };
	v51.Pos = { 0.5, -0.5f, -1.0f }; //16
	v51.Normal = { 0, -1, 0 };
	v51.TexCoord = { 1, 1 };
	v52.Pos = { 0.5, -0.5f, -1.0f }; //17
	v52.Normal = { 1, 0, 0 };
	v52.TexCoord = { 0, 0 };

	v60.Pos = { 0.5, 0.5f, -1.0f };  //18
	v60.Normal = { 0, 0, -1 };
	v60.TexCoord = { 1, 0 };
	v61.Pos = { 0.5, 0.5f, -1.0f };  //19
	v61.Normal = { 0, 1, 0 };
	v61.TexCoord = { 0, 1 };
	v62.Pos = { 0.5, 0.5f, -1.0f };  //20
	v62.Normal = { 1, 0, 0 };
	v62.TexCoord = { 0, 1 };

	v70.Pos = { -0.5, 0.5f, -1.0f }; //21
	v70.Normal = { 0, 0, -1 };
	v70.TexCoord = { 0, 0 };
	v71.Pos = { -0.5, 0.5f, -1.0f }; //22
	v71.Normal = { 0, 1, 0 };
	v71.TexCoord = { 1, 1 };
	v72.Pos = { -0.5, 0.5f, -1.0f }; //23
	v72.Normal = { -1, 0, 0 };
	v72.TexCoord = { 0, 1 };

	vertices.push_back(v00);
	vertices.push_back(v01);
	vertices.push_back(v02);

	vertices.push_back(v10);
	vertices.push_back(v11);
	vertices.push_back(v12);

	vertices.push_back(v20);
	vertices.push_back(v21);
	vertices.push_back(v22);

	vertices.push_back(v30);
	vertices.push_back(v31);
	vertices.push_back(v32);

	vertices.push_back(v40);
	vertices.push_back(v41);
	vertices.push_back(v42);

	vertices.push_back(v50);
	vertices.push_back(v51);
	vertices.push_back(v52);

	vertices.push_back(v60);
	vertices.push_back(v61);
	vertices.push_back(v62);

	vertices.push_back(v70);
	vertices.push_back(v71);
	vertices.push_back(v72);

	// populate the index array
	// triangle #1
	// normal Z
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(9);

	// triangle #2
	//Z
	indices.push_back(3);
	indices.push_back(6);
	indices.push_back(9);

	// triangle #3
	//Z
	indices.push_back(21);
	indices.push_back(18);
	indices.push_back(12);

	// triangle #4
	//Z
	indices.push_back(18);
	indices.push_back(15);
	indices.push_back(12);

	// triangle #5
	//X
	indices.push_back(23);
	indices.push_back(2);
	indices.push_back(11);

	// triangle #6
	//X
	indices.push_back(23);
	indices.push_back(14);
	indices.push_back(2);

	// triangle #7
	//X
	indices.push_back(17);
	indices.push_back(20);
	indices.push_back(8);

	// triangle #8
	//X
	indices.push_back(17);
	indices.push_back(8);
	indices.push_back(5);

	// triangle #9
	//Y
	indices.push_back(19);
	indices.push_back(10);
	indices.push_back(7);

	// triangle #10
	//Y
	indices.push_back(10);
	indices.push_back(19);
	indices.push_back(22);

	// triangle #11
	//Y
	indices.push_back(1);
	indices.push_back(13);
	indices.push_back(16);

	// triangle #12
	//Y
	indices.push_back(4);
	indices.push_back(1);
	indices.push_back(16);

	// vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0.0f };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = vertices.size()*sizeof(vertex_t);
	// data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// create vertex buffer on device using descriptor & data
	HRESULT vhr = device->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);

	//  index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0.0f };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = indices.size()*sizeof(unsigned);
	// data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// create index buffer on device using descriptor & data
	HRESULT ihr = device->CreateBuffer(&ibufferDesc, &idata, &index_buffer);

	// local data is now loaded to device so it can be released
	vertices.clear();
	nbr_indices = indices.size();
	indices.clear();
}


void Cube2_t::render(ID3D11DeviceContext* device_context) const
{
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
		// bind our vertex buffer
		UINT32 stride = sizeof(vertex_t); //  sizeof(float) * 8;
		UINT32 offset = 0;
		device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	
		// bind our index buffer
		device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	
		// make the drawcall
		device_context->DrawIndexed(nbr_indices, 0, 0);
}

Triangle_t::Triangle_t(ID3D11Device* device) :Geometry_t(device)
{
	vertex_t v0_1, v0_2, v0_3, v1_1, v1_2, v1_3, v2_1, v2_2, v2_3, v3_1, v3_2, v3_3, v4_1, v4_2, v4_3;
	
	//VERTEX #0 POS
	v0_1.Pos = { 5.0, -1.0, 0.0 };
	v0_2.Pos = { 5.0, -1.0, 0.0 };
	v0_3.Pos = { 5.0, -1.0, 0.0 };
	//VERTEX #0 NORMALS
	v0_1.Normal = { 0, 0, 1 };
	v0_2.Normal = { -1, 0, 0 };
	v0_3.Normal = { 0, -1, 0 };
	//VERTEX #0 TEXTURE_COORDINATES
	v0_1.TexCoord = { 0,0 };
	v0_2.TexCoord = { 0, 0 };
	v0_1.TexCoord = { 0, 0 };

	//VERTEX #1 POS
	v1_1.Pos = { 7.0, -1.0, 0.0 };
	v1_2.Pos = { 7.0, -1.0, 0.0 };
	v1_3.Pos = { 7.0, -1.0, 0.0 };

	//VERTEX #1 NORMALS
	v1_1.Normal = { 0, 0, 1 };
	v1_2.Normal = { 1, 0, 0 };
	v1_3.Normal = { 0, -1, 1 };

	//VERTEX #1 TEXCOORD
	v1_1.TexCoord = { 0, 1 }; 
	v1_2.TexCoord = { 0, 1 };
	v1_3.TexCoord = { 0, 1 };

	//VERTEX #2 POS
	v2_1.Pos = { 6.0, 1.0, -1.0 };
	v2_2.Pos = { 6.0, 1.0, -1.0 };
	v2_3.Pos = { 6.0, 1.0, -1.0 };

	//VERTEX #2 NORMALS
	v2_1.Normal = { 0, 0, 1 };
	v2_2.Normal = { 1, 0, 0 };
	v2_3.Normal = { 0, 1, 0 };

	//VERTEX #2 TEXCOORD
	v2_1.TexCoord = { 1, 1 };
	v2_2.TexCoord = { 1, 1 };
	v2_3.TexCoord = { 1, 1 };

	//VERTEX #3 POS
	v3_1.Pos = { 5.0, -1.0, -2.0 };
	v3_2.Pos = { 5.0, -1.0, -2.0 };
	v3_2.Pos = { 5.0, -1.0, -2.0 };

	//VERTEX #3 NORMALS
	v3_1.Normal = { 1, 0, 0 };
	v3_2.Normal = { 0, -1, 0 };
	v3_3.Normal = { 0, 0, -1 };

	//VERTEX #3 TEXCOORD
	v3_1.TexCoord = { 1, 0 };
	v3_2.TexCoord = { 1, 0 };
	v3_3.TexCoord = { 1, 0 };

    //VERTEX #4 POS
	v4_1.Pos = { 7.0, -1.0, -2.0 };
	v4_2.Pos = { 7.0, -1.0, -2.0 };
	v4_3.Pos = { 7.0, -1.0, -2.0 };

	//VERTEX #4 NORMALS
	v4_1.Normal = { 0, 0, 1 };
	v4_2.Normal = { -1, 0, 0 };
	v4_3.Normal = { 0, 0, -1 };
    
	//VERTEX #4 TEXCOORD
	v4_1.TexCoord = { 0, 0 };
	v4_2.TexCoord = { 0, 0 };
	v4_3.TexCoord = { 0, 0 };
	   
	vertices.push_back(v0_1);
	vertices.push_back(v0_2);
	vertices.push_back(v0_3);
	vertices.push_back(v1_1);
	vertices.push_back(v1_2);
	vertices.push_back(v1_3);
	vertices.push_back(v2_1);
	vertices.push_back(v2_2);
	vertices.push_back(v2_3);
	vertices.push_back(v3_1);
	vertices.push_back(v3_2);
	vertices.push_back(v3_3);
	vertices.push_back(v4_1);
	vertices.push_back(v4_2);
	vertices.push_back(v4_3);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(0);

	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(1);

	indices.push_back(1);
	indices.push_back(4);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	indices.push_back(4);
	indices.push_back(3);
	indices.push_back(2);

	// vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0.0f };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = vertices.size()*sizeof(vertex_t);
	// data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// create vertex buffer on device using descriptor & data
	HRESULT vhr = device->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);

	//  index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0.0f };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = indices.size()*sizeof(unsigned);
	// data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// create index buffer on device using descriptor & data
	HRESULT ihr = device->CreateBuffer(&ibufferDesc, &idata, &index_buffer);

	// local data is now loaded to device so it can be released
	vertices.clear();
	nbr_indices = indices.size();
	indices.clear();

	
}

void Triangle_t::render(ID3D11DeviceContext* device_context) const
{
	//set topology
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind our vertex buffer
	UINT32 stride = sizeof(vertex_t); //  sizeof(float) * 8;
	UINT32 offset = 0;
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// bind our index buffer
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// make the drawcall
	device_context->DrawIndexed(nbr_indices, 0, 0);
}

OBJModel_t::OBJModel_t(const std::string &objfile, ID3D11Device* device)
{
	//
	// load the OBJ
	//
	mesh_t* mesh = new mesh_t();
	mesh->load_obj(objfile);

	// load and organize indices in ranges per drawcall (material)
	//
	std::vector<unsigned> indices;
	size_t i_ofs = 0;

	for (auto& dc : mesh->drawcalls)
	{
		// append the drawcall indices
		for (auto& tri : dc.tris)
			indices.insert(indices.end(), tri.vi, tri.vi + 3);

		// create a range
		size_t i_size = dc.tris.size() * 3;
		int mtl_index = dc.mtl_index > -1 ? dc.mtl_index : -1;
		index_ranges.push_back({ i_ofs, i_size, 0, mtl_index });

		i_ofs = indices.size();
	}

	for (int i = 0; i < indices.size(); i+=3)
	{
		CalculateTangents(mesh->vertices[indices[i]], mesh->vertices[indices[i + 1]], mesh->vertices[indices[i+2]]);
	}

	// vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0.0f };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = mesh->vertices.size()*sizeof(vertex_t);
	// data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &(mesh->vertices)[0];
	// create vertex buffer on device using descriptor & data
	HRESULT vhr = device->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);

	// index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0.0f };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = indices.size()*sizeof(unsigned);
	// data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// create index buffer on device using descriptor & data
	HRESULT ihr = device->CreateBuffer(&ibufferDesc, &idata, &index_buffer);

	// copy materials from mesh
	append_materials(mesh->materials);

	// load textures associated with materials to device
	for (auto& mtl : materials)
	{
		HRESULT hr;
		std::wstring wstr; // for conversion from string to wstring

		/* Kd_map*/
		if (mtl.map_Kd.size())
		{
			wstr = std::wstring(mtl.map_Kd.begin(), mtl.map_Kd.end());
			hr = DirectX::CreateWICTextureFromFile(device, wstr.c_str(), &mtl.map_Kd_Tex, &mtl.map_Kd_TexSRV);
			printf("loading texture %s - %s\n", wstr.c_str(), SUCCEEDED(hr) ? "OK" : "FAILED");
		}
		if (mtl.map_bump.size())
		{
			wstr = std::wstring(mtl.map_bump.begin(), mtl.map_bump.end());
			hr = DirectX::CreateWICTextureFromFile(device, wstr.c_str(), &mtl.map_bump_Tex, &mtl.map_bump_TexSRV);
			printf("loading texture %s - %s\n", wstr.c_str(), SUCCEEDED(hr) ? "OK" : "FAILED");
		}

		// other maps here...
	}

	SAFE_DELETE(mesh);
}

void Geometry_t::CalculateTangents(vertex_t &v0, vertex_t &v1, vertex_t &v2)
{
	vec3f tangent, binormal;
	vec3f D, E;
	vec2f F, G;

	//LENGYEL'S METHOD
	
	//Calculate distance between all vertex points (x,y,z). Builds traingle DE.
	D = v1.Pos - v0.Pos;
	E = v2.Pos - v0.Pos;

	//calulate distance between all texture coordinate points (u,v). Builds triangle FG.
	F = v1.TexCoord - v0.TexCoord;
	G = v2.TexCoord - v0.TexCoord;

	//Matris invers - beräkning
	mat2f FG;
	FG.m11 = F.x;
	FG.m12 = F.y;
	FG.m21 = G.x;
	FG.m22 = G.y;
	FG.invert();
	
	tangent.x = FG.m11 * D.x + FG.m12 * E.x;
	tangent.y = FG.m11 * D.y + FG.m12 * E.y;
	tangent.z = FG.m11 * D.y + FG.m12 * E.z;

	binormal.x = FG.m21 * D.x + FG.m22 * E.x;
	binormal.y = FG.m21 * D.y + FG.m22 * E.y;
	binormal.z = FG.m21 * D.z + FG.m22 * E.z;

	tangent.normalize();
	binormal.normalize();

	v0.Tangent = v1.Tangent = v2.Tangent = tangent;
	v0.Binormal = v1.Binormal = v2.Binormal = binormal;
}

void OBJModel_t::render(ID3D11DeviceContext* device_context) const
{
 ////////////////
}

void OBJModel_t::renderObj(ID3D11DeviceContext* device_context, ID3D11Buffer* material_buffer)
{
	//set topology
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind vertex buffer
	UINT32 stride = sizeof(vertex_t); //  sizeof(float) * 8;
	UINT32 offset = 0;
	//ID3D11Buffer* buffersToSet[] = { vertex_buffer };
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// bind index buffer
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// iterate drawcalls
	for (auto& irange : index_ranges)
	{
		// fetch material and bind texture
		const material_t& mtl = materials[irange.mtl_index];
		//device_context->PSGetConstantBuffers(1, 1, &material_buffer);
		MapMaterialBuffers(device_context, material_buffer, mtl.Ka.xyz0(), mtl.Kd.xyz0(), mtl.Ks.xyz0());
		device_context->PSSetShaderResources(0, 1, &mtl.map_Kd_TexSRV);
		device_context->PSSetShaderResources(1, 1, &mtl.map_bump_TexSRV);
		device_context->PSSetSamplers(0, 1, &sampler_state);

		// make the drawcall
		device_context->DrawIndexed(irange.size, irange.start, 0);
	}
}
