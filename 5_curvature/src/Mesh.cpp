#include "Mesh.h"

ostream& operator<<(ostream& os, Halfedge* e)
{
	os << "(" << (e->Prev)()->v->index << " -> " << e->v->index << ") ";
	return os;
}

ostream& operator<<(ostream& os, vector<Halfedge*> P)
{
	os << "\t" << (P[0]->Prev)()->v->index;
	for (auto e : P) {
		os << " -> " << e->v->index << " ";

	}
	return os;
}

ostream& operator<<(ostream& os, Vertex* v)
{
	os << v->index << " ";
	return os;
}

ostream& operator<<(ostream& os, map<uint16_t, uint16_t> M)
{
	if (M.size() == 0) {
		os << "\tNone\n";
		return os;
	}

	for (auto elem : M)
		os << "\t" << elem.first << ": " << elem.second << endl;
	return os;
}

ostream& operator<<(ostream& os, map<size_t, vector<Vertex*>> M)
{
	if (M.size() == 0)
		return os;

	for (auto elem : M) {
		os << "" << elem.first << ":";
		for (auto v : elem.second) {
			os << "\t" << v->index << " ";
		}
		cout << endl;
	}
		
	return os;
}

ostream& operator<<(ostream& os, vector<vector<Halfedge*>> B)
{
	for (auto b : B) {
		cout << "\t(" ;
		for (auto e : b) {
			os << " -> " << e->index;
		}
		cout << ")" << endl;
	}
	return os;
}

ostream& operator<<(ostream& os, vector<bool> B)
{
	for (auto b : B) {
		cout << " ";
		os << b << " ";
	}
	cout << endl;
	return os;
}

//return the circulating (in CW order) half-edges of a vertex
vector<Halfedge*> Vertex::Edges(void)
{
	vector<Halfedge*> e_vec;
	Halfedge* e_begin = e;
	do {
		e_vec.push_back(e);
		e = e->next->oppo;
	} while (e != e_begin);

	return e_vec;
}

float Vertex::AngleDefect(void)
{
	vector<Halfedge*> edges = this->Edges();
	float angDefect = 2 * M_PI;
	for (auto e : edges) {
		if (e->face == NULL)
			return -100;
		else {
			glm::vec3 vec1 = glm::normalize(e->oppo->v->pos - e->v->pos);
			glm::vec3 vec2 = glm::normalize(e->next->v->pos - e->v->pos);
			float in_angle = glm::acos(glm::dot(vec1,vec2));
			angDefect -= in_angle;
		}
	}
	return angDefect;
}

float Vertex::UniformMeanCurvature(void)
{
	vector<Halfedge*> edges = this->Edges();
	glm::vec3 N(0);
	float alpha, beta;
	for (auto e : edges) {
		if (e->face == NULL)
			return -100;
		else {
			N -= e->v->pos - e->oppo->v->pos;
		}
	}
	return glm::length(N / float(edges.size()));
}

float Vertex::CotMeanCurvature(void)
{
	vector<Halfedge*> edges = this->Edges();
	glm::vec3 N{0, 0, 0};
	for (auto e : edges) {
		if (e->face == NULL)
			return -100;
		else {
			glm::vec3 v0 = e->v->pos;
			glm::vec3 v1 = e->oppo->v->pos;
			glm::vec3 v2 = e->next->v->pos;
			glm::vec3 v3 = e->oppo->Prev()->oppo->v->pos;

			glm::vec3 vec_20 = glm::normalize(v0 - v2);
			glm::vec3 vec_21 = glm::normalize(v1 - v2);
			glm::vec3 vec_30 = glm::normalize(v0 - v3);
			glm::vec3 vec_31 = glm::normalize(v1 - v3);
			glm::vec3 vec_10 = v0 - v1;

			float alpha = glm::acos(glm::dot(vec_20, vec_21));
			float beta= glm::acos(glm::dot(vec_30, vec_31));
			float cot_alpha = 1 / (glm::tan(alpha));
			float cot_beta = 1 / (glm::tan(beta));
			
			N -= float(0.5 * (cot_alpha + cot_beta)) * vec_10;
		}
	}
	return glm::length(N);
}

//return the circulating (in CCW order) half-edges of a face
vector<Halfedge*> Face::Edges(void)
{
	vector<Halfedge*> e_vec;
	Halfedge* e_begin = e;
	do {
		e_vec.push_back(e);
		e = e->next;
	} while (e != e_begin);

	return e_vec;
}

//return the ¡°previous¡± half-edge of a half-edge. Works for both interior and boundary half-edges
Halfedge* Halfedge::Prev(void)
{
	Halfedge* e_last = this;
	while (e_last->next != this)
		e_last = e_last->next;
	return e_last;
}

Mesh::Mesh(std::string obj_path)
{
	BuildFromObj(obj_path);
}

//return the boundaries of a mesh
vector<vector<Halfedge*>> Mesh::Boundaries()
{
	vector<vector<Halfedge*>> boundaries;
	vector<bool> traversed_edges;
	traversed_edges.resize(halfedges.size());

	for (auto e : halfedges) {
		if (e->face == NULL && traversed_edges[e->index] == false) {
			vector<Halfedge*> boundary;
			boundary.clear();
			Halfedge* e_last = e;
			do{
				boundary.push_back(e_last);
				traversed_edges[e_last->index] = true;
				
				e_last = e_last->next;
			}while (e_last != e);
			boundaries.push_back(boundary);
		}
	}
	return boundaries;
}

void Mesh::ParseVertexString(string str)
{
	Vertex* v;
	v = new Vertex;

	istringstream ss(str);
	string tmp;
	float digit;
	int num_digit = 0;

	while (ss >> tmp) {
		if (stringstream(tmp) >> digit) {
			v->pos[num_digit] = digit;
			num_digit += 1;
		}
	}
	v->index = vertices.size();
	vertices.push_back(v);
}

void Mesh::ParseFaceString(string str)
{
	istringstream ss(str);
	string tmp;
	uint16_t vert_idx;
	vector<int> v_vec;
	size_t halfedge_base_idx = halfedges.size();
	list<Vertex*>::iterator vert_it = vertices.begin();

	Face* f;
	f = new Face;
	f->index = faces.size();
	faces.push_back(f);
	while (ss >> tmp) {
		if (stringstream(tmp) >> vert_idx) {
			assert(vert_idx > 0 && vert_idx <= vertices.size());
			vert_idx -= 1;

			v_vec.push_back(vert_idx);
			Halfedge* e;
			e = new Halfedge;
			e->v = *next(vert_it, vert_idx);
			(*next(vert_it, vert_idx))->e = e;
			e->index = halfedges.size();
			e->face = f;
			halfedges.push_back(e);
		}
	}

	list<Halfedge*>::iterator halfedge_it = halfedges.begin();
	advance(halfedge_it, halfedge_base_idx);
	for (size_t i = 0; i < v_vec.size() - 1; i++) {
		(*next(halfedge_it, i))->next = *next(halfedge_it, i + 1);

		Mesh::UpdateEdgeMap(v_vec[i], *next(halfedge_it, i + 1));
	}
	(*next(halfedge_it, v_vec.size() - 1))->next = *halfedge_it;
	Mesh::UpdateEdgeMap(*(v_vec.end()-1), *next(halfedge_it, 0));

	f->e = *halfedge_it;
}

void Mesh::UpdateEdgeMap(int vert_idx, Halfedge* halfedge)
{
	if (out_edges_map.find(vert_idx) == out_edges_map.end()) {
		// key not exists
		vector<Halfedge*> edges_vec;
		edges_vec.push_back(halfedge);
		out_edges_map[vert_idx] = edges_vec;
	}
	else {
		// key exists
		out_edges_map[vert_idx].push_back(halfedge);
	}
}

void Mesh::AccumulatePropertyMap(map<uint16_t, uint16_t> &M, uint16_t k)
{
	if (M.find(k) == M.end()) {
		// key not exists
		M[k] = 1;
	}
	else {
		// key exists
		M[k] += 1;
	}
}

void Mesh::SetupOppoEdge(void)
{
	for (auto e : halfedges) {
		if (e->oppo != NULL)
			continue;
		vector<Halfedge*> out_edges = out_edges_map[e->v->index];
		Vertex* v_start = e->Prev()->v;
		for (auto e_out : out_edges) {
			if (e_out->v == v_start) {
				e->oppo = e_out;
				e_out->oppo = e;
			}
		}
	}
}

void Mesh::ExamineProperty(void)
{
	/*cout << "Number of Vertices: " << vertices.size() << endl;
	cout << "Number of Faces: " << faces.size() << endl;
	cout << "Number of Halfedges: " << halfedges.size() << endl;*/

	map<uint16_t, uint16_t> vert_valence, face_degree, bound_length;
	for (auto elem : out_edges_map) {
		AccumulatePropertyMap(vert_valence, elem.second.size());
	}

	for (auto f : faces) {
		AccumulatePropertyMap(face_degree, f->Edges().size());
	}

	for (auto boundary : Boundaries()) {
		AccumulatePropertyMap(bound_length, boundary.size());
	}

	cout << "Vertice Valence: \n" << vert_valence << endl;
	cout << "Face Degree: \n" << face_degree << endl;
	cout << "Boundary Length: \n" << bound_length << endl;
}

void Mesh::GenerateBoundEdge(void)
{
	/* Check each halfedge's pointer of opposite edge, if NULL, then 
	 create a boundary halfedge*/
	size_t num_edges = halfedges.size();
	list<Halfedge*>::iterator e_it = halfedges.begin();
	for (int i = 0; i < num_edges; i++) {
		if ((*e_it)->oppo != NULL) {
			advance(e_it, 1);
			continue;
		}
		Halfedge* e_bound;
		e_bound = new Halfedge;
		e_bound->oppo = *e_it;
		(*e_it)->oppo = e_bound;

		e_bound->v = (*e_it)->Prev()->v;
		e_bound->index = halfedges.size();
		halfedges.push_back(e_bound);
		out_edges_map[(*e_it)->v->index].push_back(e_bound);

		advance(e_it, 1);
	}

	/* Check each halfedge's face, if NULL, then
	 connect it with neighbour boundary halfedges*/
	num_edges = halfedges.size();
	e_it = halfedges.begin();
	for (int i = 0; i < num_edges; i++) {
		if ((*e_it)->face != NULL) {
			advance(e_it, 1);
			continue;
		}

		(*e_it)->next = (*e_it)->oppo;
		do {
			(*e_it)->next = (*e_it)->next->Prev()->oppo;
		} while ((*e_it)->next->face != NULL);

		advance(e_it, 1);
	}
}

void Mesh::BuildFromObj(std::string obj_path)
{	
	cout << "--------------------------------" << endl;
	cout << "Buiding object: " << obj_path << endl;
	ifstream file(obj_path);
	string str;

	Vertex V;
	Face F;
	
	while (std::getline(file, str))
	{
		switch (str[0]) {
		case 'v':
			ParseVertexString(str);
			break;
		case 'f':
			ParseFaceString(str);
			break;
		}
	}

	SetupOppoEdge();
	GenerateBoundEdge();
	ExamineProperty();
}

vector<Halfedge*> Mesh::ShortestPath(Vertex* v_source, Vertex* v_target)
{
	assert(v_source->index >= 0 && v_source->index < vertices.size());
	assert(v_target->index >= 0 && v_target->index < vertices.size());
	cout << "--------------------------------" << endl;
	cout << "Finding shortest path from " << v_source->index << " to " << v_target << endl;

	map<size_t, vector<Vertex*>> dist_map;
	vector<bool> traversed_vert;
	traversed_vert.resize(vertices.size());

	size_t d = 0;
	dist_map[d] = vector<Vertex*>{v_source};
	traversed_vert[v_source->index] = true;
	while (traversed_vert[v_target->index] == false) {
		vector<Vertex*> front_verts;
		for (auto V : dist_map[d]) {
			vector<Halfedge*> out_edges = out_edges_map[V->index];
			for (auto e : out_edges) {
				if (traversed_vert[e->v->index] == false) {
					front_verts.push_back(e->v);
					e->v->e = e; // set the incoming halfedge to facilitate back-traversing
					traversed_vert[e->v->index] = true;
				}
			}

		}
		d++;
		dist_map[d] = front_verts;
	}
	list<Halfedge*> path_list;
	Vertex* v_current = v_target;
	while (v_current != v_source) {
		path_list.push_front(v_current->e);
		v_current = v_current->e->oppo->v;
	}
	vector<Halfedge*> path_vec(path_list.begin(), path_list.end());
	cout << path_vec << endl;
	return path_vec;
}
