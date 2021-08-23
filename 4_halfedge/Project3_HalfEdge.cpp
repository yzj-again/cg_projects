#include <iostream>
#include "Mesh.h"

int main()
{
	//Mesh mesh = Mesh("../data/lilium.obj");
	//Mesh mesh = Mesh("../data/roof.obj");
	Mesh mesh = Mesh("../data/cow.obj");

	vector<Halfedge*> path = mesh.ShortestPath(
		*(next(mesh.vertices.begin(), 1700)), *(next(mesh.vertices.begin(), 1778)));
	
	return 0;
}
