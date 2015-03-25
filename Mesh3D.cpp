#include "StdAfx.h"
#include ".\mesh3d.h"


//--------------------------------------------------------------

Mesh3D::Mesh3D(void)
{

	//initialization

	
	vertices_list = NULL;
	faces_list = NULL;
	edges_list = NULL;

	xmax = ymax = zmax = 1.0f;
	xmin = ymin = zmin = -1.0f;

	m_closed = true;
	m_quad = false;
	m_tri = false;
}

//--------------------------------------------------------------

Mesh3D::~Mesh3D(void)
{
	clear_data();
}
//--------------------------------------------------------------

void Mesh3D::clear_data()
{

	clear_vertices();
	clear_edges();
	clear_faces();

}


//--------------------------------------------------------------

void Mesh3D::clear_vertices()
{
	if (vertices_list==NULL) {
		return;
	}
	VERTEX_ITER viter = vertices_list->begin();

	for (;viter!=vertices_list->end(); viter++) {
		delete *viter;
	}

	delete vertices_list;
	vertices_list  = NULL;
}
//--------------------------------------------------------------
void Mesh3D::clear_edges()
{
	m_edgemap.clear();
	if (edges_list==NULL) {
		return;
	}
	EDGE_ITER eiter = edges_list->begin();

	for (;eiter!=edges_list->end(); eiter++) {
		delete *eiter;
	}

	delete edges_list;
	edges_list = NULL;
}
//--------------------------------------------------------------
void Mesh3D::clear_faces()
{

	if (faces_list==NULL) {
		return;
	}
	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) {
		delete *fiter;
	}

	delete faces_list;
	faces_list = NULL;
}

//-------------------------------------------------------------
//create Mesh 

HE_vert* Mesh3D::insert_vertex(float x, float y, float z)
{
	HE_vert* hv = new HE_vert(x,y,z);
	if (vertices_list==NULL) {
		vertices_list = new VERTEX_LIST;
	}
	hv->id = (int)vertices_list->size();
	vertices_list->push_back(hv);
	
	return hv;
}
//-------------------------------------------------------------
HE_face* Mesh3D::insert_face(VERTEX_LIST& vec_hv)
{
	int vsize = (int)vec_hv.size();
	if (vsize<3) {
		return NULL;
	}

	if (faces_list==NULL) {
		faces_list = new FACE_LIST;
	}
	HE_face* hf = new HE_face;
	hf->valence = vsize;
	VERTEX_ITER viter = vec_hv.begin();
	VERTEX_ITER nviter = vec_hv.begin();
	nviter++;

	HE_edge *he1, *he2;
	std::vector<HE_edge*> v_he;
	int i;
	for (i=0; i<vsize-1; i++) {
		he1 = insert_edge( *viter, *nviter);
		he2 = insert_edge( *nviter, *viter);

		if (hf->edge==NULL) {
			hf->edge = he1;
		}
		he1->face = hf;

		he1->pair = he2;
		he2->pair = he1;
		v_he.push_back(he1);
		viter++,nviter++;
	}

	nviter = vec_hv.begin();

	he1 = insert_edge(*viter, *nviter);
	he2 = insert_edge(*nviter , *viter);
	he1->face = hf;
	if (hf->edge==NULL) {
		hf->edge = he1;
	}
	he1->pair = he2;
	he2->pair = he1;
	v_he.push_back(he1);

	for (i=0; i<vsize-1; i++) {
		v_he[i]->next = v_he[i+1];
	}
	v_he[i]->next = v_he[0];

	hf->id = (int)faces_list->size();
	faces_list->push_back(hf);
	

	return hf;

}
//-------------------------------------------------------------
HE_edge* Mesh3D::insert_edge(HE_vert* vstart, HE_vert* vend)
{
	if (edges_list==NULL) {
		edges_list = new EDGE_LIST;
	}


	if( m_edgemap[PAIR_VERTEX(vstart,vend)] != NULL )
		return m_edgemap[PAIR_VERTEX(vstart,vend)];


	HE_edge* he = new HE_edge;

	he->vert = vend;

	he->vert->degree++;

	vstart->edge = he;

	m_edgemap[PAIR_VERTEX(vstart,vend)] = he;

	he->id = (int)edges_list->size();
	edges_list->push_back(he);

	return he;
}
//--------------------------------------------------------------


void Mesh3D::set_nextedge_for_border_vertices()
{

	if (!isvalid()) {
		return;
	}

	EDGE_ITER eiter = edges_list->begin();

	for (; eiter!=edges_list->end(); eiter++) 
	{
		if ((*eiter)->next==NULL&& (*eiter)->face==NULL) {
			(*eiter)->pair->vert->edge = *eiter;
		}
	}

	for (eiter = edges_list->begin(); eiter!=edges_list->end(); eiter++) 
	{
		if ( (*eiter)->next ==NULL ) {
			HE_vert* hv = (*eiter)->vert;
			if (hv -> edge != (*eiter)->pair)
			{
				(*eiter)->next = hv -> edge;
			}
		}
	}

}

//--------------------------------------------------------------

bool Mesh3D::isborder(HE_vert* hv)
{
	HE_edge* edge = hv->edge;     
	do {
		if (edge==NULL||edge->pair->face==NULL||edge->face==NULL) {
			return true;
		}
		edge = edge->pair->next;   

	} while (edge != hv->edge);

	return false;

}
//--------------------------------------------------------------

bool Mesh3D::isborder(HE_face* hf)
{
	HE_edge* edge = hf->edge;     

	do {

		if (isborder(edge)) {
			return true;
		}

		edge = edge->next;

	} while (edge != hf->edge);

	return false;
}
//--------------------------------------------------------------

bool Mesh3D::isborder(HE_edge* he)
{
	if(he->face==NULL||he->pair->face==NULL)
		return true;
	return false;
}
//--------------------------------------------------------------

void Mesh3D::check_closed()
{
	m_closed = true; 
	VERTEX_ITER viter = vertices_list->begin();
	for (;viter!=vertices_list->end(); viter++) {
		if (isborder(*viter)) {
			m_closed = false;
			return;
		}
	}

}
//--------------------------------------------------------------

void Mesh3D::check_meshtype()
{
	FACE_ITER fiter = faces_list->begin();
	m_tri = true;
	m_quad = true;
	for (;fiter!=faces_list->end(); fiter++) {
		if ((*fiter)->valence!=3) {
			m_tri = false;
			break;
		}
	}

	 fiter = faces_list->begin();
	for (;fiter!=faces_list->end(); fiter++) {
		if ((*fiter)->valence!=4) {
			m_quad = false;
			break;
		}
	}
}
//--------------------------------------------------------------
void Mesh3D::update_mesh()
{
	if (!isvalid()) {
		return;
	}
	set_nextedge_for_border_vertices();
    check_closed();
	check_meshtype();
	m_edgemap.clear();
	compute_all_normal();
	compute_boundingbox();

}
//-------------------------------------------------------------
//FILE IO
//--------------------------------------------------------------

bool Mesh3D::load_data(std::ifstream &fin)
{

	try{

		clear_data();

		int vsize, fsize, esize;

		std::string head;

		fin>>head;

		if (head == "OFF") {
		}
		else
			return false;

		fin>>vsize>>fsize>>esize;


		float x, y, z;
		for (int i = 0; i<vsize; i++) {
			fin>>x>>y>>z;
			HE_vert* hv = insert_vertex(x,y,z);
		}

		for (int i = 0; i<fsize; i++) {

			VERTEX_LIST v_list;
			int valence;
			fin>>valence;

			for (int j=0; j<valence; j++) {
				int id;
				fin>>id;
				HE_vert* hv = get_vertex(id);
				if (hv==NULL) {
					throw 1;
				}
				v_list.push_back( hv );
			}

			insert_face(v_list);


		}

		
		update_mesh();
		compute_all_normal();
	}
	catch (...) {
		//catch any error
		clear_data();
		xmax = ymax = zmax = 1.0f;
		xmin = ymin = zmin = -1.0f;
		return false;

	}

	return true;
}
//--------------------------------------------------------------

void Mesh3D::write_data(std::ofstream &fout)
{

	fout<<"OFF\n";
	//output the number of vertices_list, faces_list-> edges_list
	fout<<(int)vertices_list->size()<<" "<<(int)faces_list->size()<<" "<<(int)edges_list->size()/2<<"\n";

	//output coordinates of each vertex
	VERTEX_ITER viter = vertices_list->begin();
	for (;viter!=vertices_list->end(); viter++) {

		fout<<(*viter)->x<<" "<<(*viter)->y<<" "<<(*viter)->z<<"\n";
	}

	//output the valence of each face and its vertices_list' id

	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) {

		fout<<(*fiter)->valence;

		HE_edge* edge = (*fiter)->edge;     

		do {
			fout<<" "<<edge->vert->id;
			edge = edge->next;

		} while (edge != (*fiter)->edge);
		fout<<"\n";
	}

}


//--------------------------------------------------------------
//-------------------------------------------------------------
//For rendering

void Mesh3D::compute_boundingbox()
{

	if (vertices_list->size()<3) {
		return;
	}

	xmax = ymax = zmax = -10e10f;
	xmin = ymin = zmin = 10e10f;

	VERTEX_ITER viter = vertices_list->begin();
	for (; viter!=vertices_list->end(); viter++) {
		xmin = min( (*viter)->x,xmin );
		ymin = min( (*viter)->y,ymin );
		zmin = min( (*viter)->z,zmin );
		xmax = max( (*viter)->x,xmax );
		ymax = max( (*viter)->y,ymax );
		zmax = max( (*viter)->z,zmax );
	}
}

//--------------------------------------------------------------
void Mesh3D::compute_faces_list_normal()
{
	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) 
	{
		compute_perface_normal(*fiter);
	}
}
//--------------------------------------------------------------
void Mesh3D::compute_perface_normal(HE_face* hf)
{
	int i = 0;
	HE_edge* pedge = hf->edge;
	HE_edge* nedge = hf->edge->next;

	float nx=0, ny=0, nz=0;
	for (i=0; i<hf->valence; i++) {

		//cross product
		HE_vert* p = pedge->vert;
		HE_vert* c = pedge->next->vert;
		HE_vert* n = nedge->next->vert;
		float pcx,pcy,pcz,ncx,ncy,ncz;
		pcx = p->x - c->x; pcy = p->y - c->y; pcz = p->z - c->z;
		ncx = n->x - c->x; ncy = n->y - c->y; ncz = n->z - c->z;

		nx+= pcy*ncz-pcz*ncy; ny+= pcz*ncx-pcx*ncz; nz+= pcx*ncy-pcy*ncx;
		pedge = nedge;
		nedge = nedge->next;
		if (hf->valence==3) {
			break;
		}
	}

	float sumarea = sqrt(nx*nx+ny*ny+nz*nz);
	if (sumarea==0.0f) {
		hf->normal[0] = 0.0f;
		hf->normal[1] = 0.0f;
		hf->normal[2] = 0.0f;
	}
	else
	{
		hf->normal[0] = -nx/sumarea;
		hf->normal[1] = -ny/sumarea;
		hf->normal[2] = -nz/sumarea;
	}
}
//--------------------------------------------------------------
void Mesh3D::compute_vertices_list_normal()
{
	VERTEX_ITER viter = vertices_list->begin();

	for (; viter!=vertices_list->end(); viter++) 
	{
		compute_pervertex_normal(*viter);
	}

}
//--------------------------------------------------------------
void Mesh3D::compute_pervertex_normal(HE_vert* hv)
{

	HE_edge* edge = hv->edge; 
	
	if (edge==NULL) {
		hv->normal[0] = 0.0f;
		hv->normal[1] = 0.0f;
		hv->normal[2] = 0.0f;
		return;
	}

	float x = 0, y = 0, z = 0;

	do {
		if (edge->face!=NULL) {
			x += edge->face->normal[0];
			y += edge->face->normal[1];
			z += edge->face->normal[2];
		}

		edge = edge->pair->next;   

	} while (edge != hv->edge);

	float sqnorm = sqrt(x*x+y*y+z*z);
	if (sqnorm==0.0f) {
		hv->normal[0] = 0.0f;
		hv->normal[1] = 0.0f;
		hv->normal[2] = 0.0f;
	}
	else
	{
		hv->normal[0] = x/sqnorm;
		hv->normal[1] = y/sqnorm;
		hv->normal[2] = z/sqnorm;
	}

}

//--------------------------------------------------------------
void Mesh3D::compute_all_normal()
{
	compute_faces_list_normal();
	compute_vertices_list_normal();
}


//--------------------------------------------------------------

void Mesh3D::gl_draw(bool smooth)
{

	if (!isvalid()) {
		return;
	}
	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) 
	{
		HE_edge* edge = (*fiter)->edge;     

		if (!smooth) {
			glNormal3fv((*fiter)->normal);
		}


		glBegin(GL_POLYGON);
		do {

			if(smooth)
				glNormal3fv(edge->vert->normal);

			glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);
			edge = edge->next;

		} while (edge != (*fiter)->edge);
		glEnd();
	}

}
