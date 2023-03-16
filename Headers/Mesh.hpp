#pragma once

enum MeshType
{
	MT_Cube,
	MT_Quad,
	MT_Sphere
};

class Mesh
{
	// Attributs
private:
	unsigned int VAO;
	unsigned int VBO;

	int size;

public:
	MeshType mt;
	// Methods
public:
	Mesh();
	
	void InitCube();
	void InitQuad();
	void InitSphere();

	void Draw();
};