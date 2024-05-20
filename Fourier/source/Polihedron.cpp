#include "Polihedron.h"
#include "Bindable/BindableBase.h"

Polihedron::~Polihedron()
{
	if (Vertexs)
		free(Vertexs);
	if (Triangles)
		free(Triangles);
	if (neighbours)
	{
		for (unsigned int i = 0; i < numVertexs; i++)
		{
			if (neighbours[i])
			{
				free(neighbours[i]);
				free(neighboursAngles[i]);
			}
		}
		free(neighbours);
		free(neighboursAngles);
		free(valencia);
		free(areaTriangles);
		free(Normals);
		free(vertexsWeight);
	}
}

Polihedron::Polihedron(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, UINT numV, const  Color* colors, bool vertexColor, bool transparency, bool doubleSided, std::mutex* mtx)
{

	create(gfx, vertexs, triangles, numT, numV, colors, vertexColor, transparency, doubleSided, mtx);

}

void Polihedron::create(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, UINT numV, const Color* colors, bool vertexColor, bool transparency, bool doubleSided, std::mutex* mtx)
{
	if (isInit)
		throw std::exception("You cannot create a polihedron over one that is already initialized");
	else
		isInit = true;

	Vertexs = (Vector3f*)vertexs;
	Triangles = (Vector3i*)triangles;
	numTriangles = numT;
	numVertexs = numV;

	Vertex* V = (Vertex*)calloc(3 * numT, sizeof(Vertex));

	for (UINT i = 0; i < numT; i++)
	{

		V[3 * i].vector = vertexs[triangles[i].x];
		V[3 * i + 1].vector = vertexs[triangles[i].y];
		V[3 * i + 2].vector = vertexs[triangles[i].z];

		if (vertexColor && colors)
		{
			V[3 * i].color = colors[triangles[i].x];
			V[3 * i + 1].color = colors[triangles[i].y];
			V[3 * i + 2].color = colors[triangles[i].z];
		}

		else if (colors)
		{
			V[3 * i].color = colors[i];
			V[3 * i + 1].color = colors[i];
			V[3 * i + 2].color = colors[i];
		}

		else
		{
			V[3 * i].color = Color::White;
			V[3 * i + 1].color = Color::White;
			V[3 * i + 2].color = Color::White;
		}

		Vector3f norm = ((V[3 * i + 1].vector - V[3 * i].vector) * (V[3 * i + 2].vector - V[3 * i].vector)).normalize();
		V[3 * i].norm = norm;
		V[3 * i + 1].norm = norm;
		V[3 * i + 2].norm = norm;
	}

	unsigned short* indexs = (unsigned short*)calloc(3 * numT, sizeof(unsigned short));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	if (mtx)
		mtx->lock();

	AddBind(std::make_unique<VertexBuffer>(gfx, V, 3 * numT));

	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * numT));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR + std::wstring(L"PolihedronVS.cso"))));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR + std::wstring(L"PolihedronPS.cso")));

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, doubleSided));

	AddBind(std::make_unique<Blender>(gfx, transparency));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	float unused = 0.f;
	pscBuff = {
		50.f,10.f,unused,unused,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,unused,
		50.f,10.f,unused,unused,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,unused,
		50.f,10.f,unused,unused,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,unused,
		50.f,10.f,unused,unused,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,unused,
	};

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	free(V);
	free(indexs);

	if(mtx)
		mtx->unlock();

	generateNeighbourhood();
}

void Polihedron::generateNeighbourhood()
{
	areaTriangles = (float*)calloc(numTriangles, sizeof(float));
	vertexsWeight = (float*)calloc(numVertexs, sizeof(float));
	neighbours = (int**)calloc(numVertexs, sizeof(void*));
	neighboursAngles = (float**)calloc(numVertexs, sizeof(void*));
	valencia = (unsigned int*)calloc(numVertexs, sizeof(int));
	Normals = (Vector3f*)calloc(numTriangles, sizeof(Vector3f));

	for (unsigned int i = 0; i < numTriangles; i++)
	{
		int v1 = Triangles[i].x;
		int v2 = Triangles[i].y;
		int v3 = Triangles[i].z;

		Vector3f V1 = Vertexs[v1];
		Vector3f V2 = Vertexs[v2];
		Vector3f V3 = Vertexs[v3];

		Normals[i] = ((V2 - V1) * (V3 - V1)).normalize();

		if ((Normals[i] ^ (V1 + V2 + V3)) < 0)
		{
			v1 = Triangles[i].y;
			v2 = Triangles[i].x;

			V1 = Vertexs[v1];
			V2 = Vertexs[v2];
		}

		Vector3f V12 = (V1 * V2 * V1).normalize();
		Vector3f V13 = (V1 * V3 * V1).normalize();

		float cos1 = V13 ^ V12;
		float angle1 = acosf(cos1);

		Vector3f V21 = (V2 * V1 * V2).normalize();
		Vector3f V23 = (V2 * V3 * V2).normalize();

		float cos2 = V23 ^ V21;
		float angle2 = acosf(cos2);

		Vector3f V32 = (V3 * V2 * V3).normalize();
		Vector3f V31 = (V3 * V1 * V3).normalize();

		float cos3 = V31 ^ V32;
		float angle3 = acosf(cos3);

		areaTriangles[i] = angle1 + angle2 + angle3 - pi;

		angle1 = cos1 / sqrt(1.f - cos1 * cos1);
		angle2 = cos2 / sqrt(1.f - cos2 * cos2);
		angle3 = cos3 / sqrt(1.f - cos3 * cos3);

		vertexsWeight[v1] += areaTriangles[i] / 3.f;
		vertexsWeight[v2] += areaTriangles[i] / 3.f;
		vertexsWeight[v3] += areaTriangles[i] / 3.f;

		if (!neighbours[v1])
		{
			neighbours[v1] = (int*)calloc(2, sizeof(int));
			neighbours[v1][0] = v2;
			neighbours[v1][1] = v3;

			neighboursAngles[v1] = (float*)calloc(4, sizeof(float));
			neighboursAngles[v1][1] = angle2;
			neighboursAngles[v1][2] = angle3;

			valencia[v1] = 2u;
		}
		else
		{
			bool isthere = false;
			for (unsigned int j = 0; j < valencia[v1]; j++)
			{
				if (neighbours[v1][j] == v2)
				{
					isthere = true;
					if (neighbours[v1][(j + 1) % valencia[v1]] == v3)
					{
						neighboursAngles[v1][2 * j + 1] = angle2;
						neighboursAngles[v1][2 * ((j + 1) % valencia[v1])] = angle3;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v1] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v1] + 1), sizeof(float));
						for (unsigned int k = 0; k <= j; k++)
						{
							tneighbours[k] = neighbours[v1][k];
							tnangles[2 * k] = neighboursAngles[v1][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v1][2 * k + 1];
						}
						for (unsigned int k = j + 2; k < valencia[v1] + 1; k++)
						{
							tneighbours[k] = neighbours[v1][k - 1];
							tnangles[2 * k] = neighboursAngles[v1][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v1][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle2;
						tnangles[2 * (j + 1)] = angle3;
						tneighbours[j + 1] = v3;

						free(neighbours[v1]);
						free(neighboursAngles[v1]);

						neighbours[v1] = tneighbours;
						neighboursAngles[v1] = tnangles;
						valencia[v1]++;
					}
					break;
				}
				if (neighbours[v1][j] == v3)
				{
					isthere = true;
					if (neighbours[v1][(j - 1 + valencia[v1]) % valencia[v1]] == v2)
					{
						neighboursAngles[v1][2 * j] = angle3;
						neighboursAngles[v1][2 * ((j - 1 + valencia[v1]) % valencia[v1]) + 1] = angle2;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v1] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v1] + 1), sizeof(float));
						for (unsigned int k = 0; k < j; k++)
						{
							tneighbours[k] = neighbours[v1][k];
							tnangles[2 * k] = neighboursAngles[v1][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v1][2 * k + 1];
						}
						for (unsigned int k = j + 1; k < valencia[v1] + 1; k++)
						{
							tneighbours[k] = neighbours[v1][k - 1];
							tnangles[2 * k] = neighboursAngles[v1][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v1][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle2;
						tnangles[2 * (j + 1)] = angle3;
						tneighbours[j] = v2;

						free(neighbours[v1]);
						free(neighboursAngles[v1]);

						neighbours[v1] = tneighbours;
						neighboursAngles[v1] = tnangles;
						valencia[v1]++;
					}
					break;
				}
			}
			if (!isthere)
			{
				int* tneighbours = (int*)calloc(valencia[v1] + 2, sizeof(int));
				float* tnangles = (float*)calloc(2 * (valencia[v1] + 2), sizeof(float));
				for (unsigned int k = 0; k < valencia[v1]; k++)
				{
					tneighbours[k] = neighbours[v1][k];
					tnangles[2 * k] = neighboursAngles[v1][2 * k];
					tnangles[2 * k + 1] = neighboursAngles[v1][2 * k + 1];
				}
				tneighbours[valencia[v1]] = v2;
				tneighbours[valencia[v1] + 1] = v3;
				tnangles[2 * valencia[v1] + 1] = angle2;
				tnangles[2 * (valencia[v1] + 1)] = angle3;

				free(neighbours[v1]);
				free(neighboursAngles[v1]);

				neighbours[v1] = tneighbours;
				neighboursAngles[v1] = tnangles;
				valencia[v1] += 2;
			}
		}

		if (!neighbours[v2])
		{
			neighbours[v2] = (int*)calloc(2, sizeof(int));
			neighbours[v2][0] = v3;
			neighbours[v2][1] = v1;

			neighboursAngles[v2] = (float*)calloc(4, sizeof(float));
			neighboursAngles[v2][1] = angle3;
			neighboursAngles[v2][2] = angle1;

			valencia[v2] = 2u;
		}
		else
		{
			bool isthere = false;
			for (unsigned int j = 0; j < valencia[v2]; j++)
			{
				if (neighbours[v2][j] == v3)
				{
					isthere = true;
					if (neighbours[v2][(j + 1) % valencia[v2]] == v1)
					{
						neighboursAngles[v2][2 * j + 1] = angle3;
						neighboursAngles[v2][2 * ((j + 1) % valencia[v2])] = angle1;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v2] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v2] + 1), sizeof(float));
						for (unsigned int k = 0; k <= j; k++)
						{
							tneighbours[k] = neighbours[v2][k];
							tnangles[2 * k] = neighboursAngles[v2][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v2][2 * k + 1];
						}
						for (unsigned int k = j + 2; k < valencia[v2] + 1; k++)
						{
							tneighbours[k] = neighbours[v2][k - 1];
							tnangles[2 * k] = neighboursAngles[v2][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v2][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle3;
						tnangles[2 * (j + 1)] = angle1;
						tneighbours[j + 1] = v1;

						free(neighbours[v2]);
						free(neighboursAngles[v2]);

						neighbours[v2] = tneighbours;
						neighboursAngles[v2] = tnangles;
						valencia[v2]++;
					}
					break;
				}
				if (neighbours[v2][j] == v1)
				{
					isthere = true;
					if (neighbours[v2][(j - 1 + valencia[v2]) % valencia[v2]] == v3)
					{
						neighboursAngles[v2][2 * j] = angle1;
						neighboursAngles[v2][2 * ((j - 1 + valencia[v2]) % valencia[v2]) + 1] = angle3;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v2] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v2] + 1), sizeof(float));
						for (unsigned int k = 0; k < j; k++)
						{
							tneighbours[k] = neighbours[v2][k];
							tnangles[2 * k] = neighboursAngles[v2][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v2][2 * k + 1];
						}
						for (unsigned int k = j + 1; k < valencia[v2] + 1; k++)
						{
							tneighbours[k] = neighbours[v2][k - 1];
							tnangles[2 * k] = neighboursAngles[v2][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v2][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle3;
						tnangles[2 * (j + 1)] = angle1;
						tneighbours[j] = v3;

						free(neighbours[v2]);
						free(neighboursAngles[v2]);

						neighbours[v2] = tneighbours;
						neighboursAngles[v2] = tnangles;
						valencia[v2]++;
					}
					break;
				}
			}
			if (!isthere)
			{
				int* tneighbours = (int*)calloc(valencia[v2] + 2, sizeof(int));
				float* tnangles = (float*)calloc(2 * (valencia[v2] + 2), sizeof(float));
				for (unsigned int k = 0; k < valencia[v2]; k++)
				{
					tneighbours[k] = neighbours[v2][k];
					tnangles[2 * k] = neighboursAngles[v2][2 * k];
					tnangles[2 * k + 1] = neighboursAngles[v2][2 * k + 1];
				}
				tneighbours[valencia[v2]] = v3;
				tneighbours[valencia[v2] + 1] = v1;
				tnangles[2 * valencia[v2] + 1] = angle3;
				tnangles[2 * (valencia[v2] + 1)] = angle1;

				free(neighbours[v2]);
				free(neighboursAngles[v2]);

				neighbours[v2] = tneighbours;
				neighboursAngles[v2] = tnangles;
				valencia[v2] += 2;
			}
		}

		if (!neighbours[v3])
		{
			neighbours[v3] = (int*)calloc(2, sizeof(int));
			neighbours[v3][0] = v1;
			neighbours[v3][1] = v2;

			neighboursAngles[v3] = (float*)calloc(4, sizeof(float));
			neighboursAngles[v3][1] = angle1;
			neighboursAngles[v3][2] = angle2;

			valencia[v3] = 2u;
		}
		else
		{
			bool isthere = false;
			for (unsigned int j = 0; j < valencia[v3]; j++)
			{
				if (neighbours[v3][j] == v1)
				{
					isthere = true;
					if (neighbours[v3][(j + 1) % valencia[v3]] == v2)
					{
						neighboursAngles[v3][2 * j + 1] = angle1;
						neighboursAngles[v3][2 * ((j + 1) % valencia[v3])] = angle2;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v3] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v3] + 1), sizeof(float));
						for (unsigned int k = 0; k <= j; k++)
						{
							tneighbours[k] = neighbours[v3][k];
							tnangles[2 * k] = neighboursAngles[v3][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v3][2 * k + 1];
						}
						for (unsigned int k = j + 2; k < valencia[v3] + 1; k++)
						{
							tneighbours[k] = neighbours[v3][k - 1];
							tnangles[2 * k] = neighboursAngles[v3][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v3][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle1;
						tnangles[2 * (j + 1)] = angle2;
						tneighbours[j + 1] = v2;

						free(neighbours[v3]);
						free(neighboursAngles[v3]);

						neighbours[v3] = tneighbours;
						neighboursAngles[v3] = tnangles;
						valencia[v3]++;
					}
					break;
				}
				if (neighbours[v3][j] == v2)
				{
					isthere = true;
					if (neighbours[v3][(j - 1 + valencia[v3]) % valencia[v3]] == v1)
					{
						neighboursAngles[v3][2 * j] = angle2;
						neighboursAngles[v3][2 * ((j - 1 + valencia[v3]) % valencia[v3]) + 1] = angle1;
					}
					else
					{
						int* tneighbours = (int*)calloc(valencia[v3] + 1, sizeof(int));
						float* tnangles = (float*)calloc(2 * (valencia[v3] + 1), sizeof(float));
						for (unsigned int k = 0; k < j; k++)
						{
							tneighbours[k] = neighbours[v3][k];
							tnangles[2 * k] = neighboursAngles[v3][2 * k];
							tnangles[2 * k + 1] = neighboursAngles[v3][2 * k + 1];
						}
						for (unsigned int k = j + 1; k < valencia[v3] + 1; k++)
						{
							tneighbours[k] = neighbours[v3][k - 1];
							tnangles[2 * k] = neighboursAngles[v3][2 * (k - 1)];
							tnangles[2 * k + 1] = neighboursAngles[v3][2 * (k - 1) + 1];
						}
						tnangles[2 * j + 1] = angle1;
						tnangles[2 * (j + 1)] = angle2;
						tneighbours[j] = v1;

						free(neighbours[v3]);
						free(neighboursAngles[v3]);

						neighbours[v3] = tneighbours;
						neighboursAngles[v3] = tnangles;
						valencia[v3]++;
					}
					break;
				}
			}
			if (!isthere)
			{
				int* tneighbours = (int*)calloc(valencia[v3] + 2, sizeof(int));
				float* tnangles = (float*)calloc(2 * (valencia[v3] + 2), sizeof(float));
				for (unsigned int k = 0; k < valencia[v3]; k++)
				{
					tneighbours[k] = neighbours[v3][k];
					tnangles[2 * k] = neighboursAngles[v3][2 * k];
					tnangles[2 * k + 1] = neighboursAngles[v3][2 * k + 1];
				}
				tneighbours[valencia[v3]] = v1;
				tneighbours[valencia[v3] + 1] = v2;
				tnangles[2 * valencia[v3] + 1] = angle1;
				tnangles[2 * (valencia[v3] + 1)] = angle2;

				free(neighbours[v3]);
				free(neighboursAngles[v3]);

				neighbours[v3] = tneighbours;
				neighboursAngles[v3] = tnangles;
				valencia[v3] += 2;
			}
		}
	}
}

void Polihedron::updateShape(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors, bool vertexColor, std::mutex* mtx)
{
	if (isInit)
		throw std::exception("You cannot update the shape to an uninitialized polihedron");

	Vertex* V = (Vertex*)calloc(3 * numT, sizeof(Vertex));

	free(Vertexs);
	free(Triangles);
	Vertexs = (Vector3f*)vertexs;
	Triangles = (Vector3i*)triangles;
	numTriangles = numT;

	for (UINT i = 0; i < numT; i++)
	{

		V[3 * i].vector = vertexs[triangles[i].x];
		V[3 * i + 1].vector = vertexs[triangles[i].y];
		V[3 * i + 2].vector = vertexs[triangles[i].z];

		if (vertexColor && colors)
		{
			V[3 * i].color = colors[triangles[i].x];
			V[3 * i + 1].color = colors[triangles[i].y];
			V[3 * i + 2].color = colors[triangles[i].z];
		}

		else if (colors)
		{
			V[3 * i].color = colors[i];
			V[3 * i + 1].color = colors[i];
			V[3 * i + 2].color = colors[i];
		}

		else
		{
			V[3 * i].color = Color::White;
			V[3 * i + 1].color = Color::White;
			V[3 * i + 2].color = Color::White;
		}

		Vector3f norm = ((V[3 * i + 1].vector - V[3 * i].vector) * (V[3 * i + 2].vector - V[3 * i].vector)).normalize();
		V[3 * i].norm = norm;
		V[3 * i + 1].norm = norm;
		V[3 * i + 2].norm = norm;
	}

	if (mtx)
		mtx->lock();

	changeBind(std::make_unique<VertexBuffer>(gfx, V, 3 * numT), 0u);


	unsigned short* indexs = (unsigned short*)calloc(3 * numT, sizeof(unsigned short));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	changeBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * numT), 1u);

	free(V);
	free(indexs);

	if (mtx)
		mtx->lock();
}

void Polihedron::updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ)
{
	vscBuff.rotation = rotationQuaternion({ 1,0,0 }, rotationX) * rotationQuaternion({ 0,1,0 }, rotationY) * rotationQuaternion({ 0,0,1 }, rotationZ);

	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotationQuaternion(axis, angle);
	else
		vscBuff.rotation *= rotationQuaternion(axis, angle);

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw std::exception("You cannot update rotation to an uninitialized polihedron");

	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updatePosition(Graphics& gfx, Vector3f position, bool additive)
{
	if (!additive)
		vscBuff.translation = position.getVector4();
	else
	{
		vscBuff.translation.x += position.x;
		vscBuff.translation.y += position.y;
		vscBuff.translation.z += position.z;
	}

	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	if (!isInit)
		throw std::exception("You cannot update screen position to an uninitialized polihedron");

	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position)
{
	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	pPSCB->Update(gfx, pscBuff);
}

void Polihedron::updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position)
{
	pscBuff.lightsource[id] = { intensity , color , position };
	pPSCB->Update(gfx, pscBuff);
}

void Polihedron::clearLights(Graphics& gfx)
{
	pscBuff = {};
	pPSCB->Update(gfx, pscBuff);
}

Quaternion Polihedron::getRotation() const
{
	return vscBuff.rotation;
}

Vector3f Polihedron::getPosition() const
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}

const Vector3f* Polihedron::getVertexs() const
{
	return Vertexs;
}

const Vector3i* Polihedron::getTriangles() const
{
	return Triangles;
}

const Vector3f* Polihedron::getNormals() const
{
	return Normals;
}

unsigned int Polihedron::getNumTriangles() const
{
	return numTriangles;
}

unsigned int Polihedron::getNumVertexs() const
{
	return numVertexs;
}

const unsigned int* Polihedron::getValencies() const
{
	return valencia;
}

const int** Polihedron::getNeighbours() const
{
	return (const int**)neighbours;
}

const float** Polihedron::getNeighboursAngles() const
{
	return (const float**)neighboursAngles;
}

const float* Polihedron::getAreaTriangles() const
{
	return (const float*)areaTriangles;
}

const float* Polihedron::getVertexsWeight() const
{
	return vertexsWeight;
}
