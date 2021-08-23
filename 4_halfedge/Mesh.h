#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <glm.hpp>

using namespace std;

class Vertex;
class Face;
class Halfedge;
class Mesh;


class Vertex
{
public:
	vector<Halfedge*> Edges(void);

	Halfedge* e = NULL;
	size_t index = NULL;
	glm::vec3 pos;
};


class Face
{
public:
	vector<Halfedge*> Edges(void);
	
	Halfedge* e = NULL;
	size_t index = NULL;
};


class Halfedge
{
public:
	Halfedge* Prev(void);

	Halfedge* oppo = NULL;
	Halfedge* next = NULL;
	Vertex* v = NULL;
	Face* face = NULL;
	size_t index = NULL;
};


class Mesh
{
private:
	void ParseVertexString(string str);
	void ParseFaceString(string str);
	void UpdateEdgeMap(int vert_idx, Halfedge* halfedge);
	void AccumulatePropertyMap(map<uint16_t, uint16_t>& M, uint16_t k);
	void SetupOppoEdge(void);
	void GenerateBoundEdge(void);
	void BuildFromObj(std::string obj_path);

public:
	Mesh(std::string obj_path);
	vector<Halfedge*> ShortestPath(Vertex* v_source, Vertex* v_target);
	vector<vector<Halfedge*>> Boundaries();
	void ExamineProperty(void);

	list<Halfedge*> halfedges;
	list<Face*> faces;
	list<Vertex*> vertices;
	map<int, vector<Halfedge*>> out_edges_map;
};