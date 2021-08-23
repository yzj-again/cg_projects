#include "ofApp.h"

bool FindOrthogonalVectors(glm::vec3& in, glm::vec3& out1, glm::vec3& out2)
{
	//find one non-zero component of in:
	int nz1 = -1;
	for (int i = 0; i < 3; i++)
	{
		if (in[i] != 0)
		{
			nz1 = i;
			break;
		}
	}
	if (nz1 == -1)
		return false;  //in is (0,0,0) !!! don't bother	

	//find another non-zero component:
	int nz2 = -1;
	int i = nz1;
	while (true)
	{
		i = (i + 1) % 3;

		if (i == nz1)
			break;  //again

		if (in[i] != 0)
		{
			nz2 = i;
			break;
		}
	}

	out1 = glm::vec3(0, 0, 0);
	//case1: if both nz1 & nz2 are found, out1 can be formed as: out1[nz1] = -in[nz2] ; out1[nz2] = -in[nz1] ; another component is zero
	if (nz2 != -1)
	{
		out1[nz1] = -in[nz2];
		out1[nz2] = -in[nz1];

		if (out1 == in || out1 == -in)
			out1[nz1] += 0.012345f;  //avoid out1 become linear to in
	}
	//case2: otherwise is even simpler: out1[nz1] = 0 and other 2 components are set to 1
	else
	{
		out1[(nz1 + 1) % 3] = 1;
		out1[(nz1 + 2) % 3] = 1;
	}

	//out2 is in cross out1
	out2 = cross(in, out1);

	//out1 may be ill-oriented so need to re-oriented it: out1 is out2 cross in
	out1 = cross(out2, in);

	return true;
}

//generate a cylinder ofMesh from point v1 to v2
//sides: #sides of the cylinder 
void Cylinder(glm::vec3 v1, glm::vec3 v2, float radius, ofMesh& mesh, glm::vec3 color, int sides)
{
	float angle = 2*glm::pi<float>() / sides;

	glm::vec3 n = v2 - v1;

	//two vectors orthogonal to the vector between v1&v2 
	glm::vec3 o1(0), o2(0);
	if (!FindOrthogonalVectors(n, o1, o2))
		return;

	o1 = normalize(o1) * radius / 2;
	o2 = normalize(o2) * radius / 2;

	auto base_idx = mesh.getNumVertices();
	//vertices
	for (int s = 0; s < sides; s++)  //ring at v1
	{
		mesh.addVertex(v1 + o1 * cos(angle * s) + o2 * sin(angle * s));
		mesh.addColor(ofColor(color.x, color.y, color.z));
	}
	for (int s = 0; s < sides; s++)  //ring at v2
	{
		mesh.addVertex(v2 + o1 * cos(angle * s) + o2 * sin(angle * s));
		mesh.addColor(ofColor(color.x, color.y, color.z));
	}

	//triangles
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	//cout << mesh.getNumIndices() << " ";
	for (int s = 0; s < sides; s++)
	{
		//2 triangles per quad
		mesh.addIndex(base_idx + s);
		mesh.addIndex(base_idx + sides + s);
		mesh.addIndex(base_idx + sides + (s + 1) % sides);
		
		mesh.addIndex(base_idx + sides + (s + 1) % sides);
		mesh.addIndex(base_idx + (s + 1) % sides);
		mesh.addIndex(base_idx + s);
	}
}