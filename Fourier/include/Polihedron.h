#pragma once
#include "Drawable.h"
#include <mutex>

class Polihedron : public Drawable
{
public:

	Polihedron() {}
	~Polihedron();
	Polihedron(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, UINT numV, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true, std::mutex* mtx = NULL);

	void create(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, UINT numV, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true, std::mutex* mtx = NULL);

	void generateNeighbourhood();
	void updateShape(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false, std::mutex* mtx = NULL);
	void updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ);
	void updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative = false);
	void updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative = false);
	void updatePosition(Graphics& gfx, Vector3f position, bool additive = false);
	void updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement);
	void updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position);
	void updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position);
	void clearLights(Graphics& gfx);

	Quaternion getRotation() const;
	Vector3f getPosition() const;
	const Vector3f* getVertexs() const;
	const Vector3i* getTriangles() const;
	const Vector3f* getNormals() const;
	unsigned int getNumTriangles() const;
	unsigned int getNumVertexs() const;
	const unsigned int* getValencies() const;
	const int** getNeighbours() const;
	const float** getNeighboursAngles() const;
	const float* getAreaTriangles() const;
	const float* getVertexsWeight() const;

private:
	int** neighbours = NULL;
	unsigned int* valencia = NULL;
	float** neighboursAngles = NULL;
	float* areaTriangles = NULL;
	float* vertexsWeight = NULL;
	Vector3f* Normals = NULL;
	Vector3f* Vertexs = NULL;
	Vector3i* Triangles = NULL;
	unsigned int numTriangles = 0u;
	unsigned int numVertexs = 0u;

	struct Vertex {
		Vector3f vector;
		Vector3f norm;
		Color color;
	};

	struct VSconstBuffer {
		_float4vector translation = { 0.f, 0.f, 0.f, 0.f };
		Quaternion rotation = 1.f;
		_float4vector screenDisplacement = { 0.f, 0.f, 0.f, 0.f };
	}vscBuff;

	struct PSconstBuffer {
		struct {
			_float4vector intensity = { 0.f,0.f,0.f,0.f };
			_float4color  color = { 1.f,1.f,1.f,1.f };
			_float4vector position = { 0.f,0.f,0.f,0.f };
		}lightsource[8];
	}pscBuff;

	ConstantBuffer<VSconstBuffer>* pVSCB = NULL;
	ConstantBuffer<PSconstBuffer>* pPSCB = NULL;

};