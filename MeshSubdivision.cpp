#include "StdAfx.h"
#include ".\meshsubdivision.h"

//To store the new vertex in the corresponding face
struct NEW_VERT{
	HE_vert*  newvert;  //new vertex
	int faceID;			//corresponding face
}; 
//To store the new vertex in the corresponding edge
struct NEW_VERT_E{
	HE_vert*  newvert;  //new vertex
	int edgeID;			//corresponding edge
	int edge_pairID;
}; 
//To store the new vertex in the corresponding vertex
struct NEW_VERT_V{
	HE_vert*  newvert;  //new vertex
	int vertexID;			//corresponding vertex
}; 

//To store the new vertices of each original vertex
typedef std::vector <NEW_VERT> NEW_VERT_LIST;

typedef std::vector <NEW_VERT_E> NEW_VERT_E_LIST;

typedef std::vector <NEW_VERT_V> NEW_VERT_V_LIST;

//To calculate the coefficient of "alpha"
double alpha(int i,int n)
{
	double tmp= 1.0/(4.0*n);
	switch(i) {
		case 0: return 0.5+tmp;		break;
		case 1: return 0.125+tmp;	break;
		default: 
			assert(i<n);
			if (i==n-1) {
				return 0.125+tmp;
				break;
			}
			else return tmp;
	}	
}

//To create the new vertex
HE_vert* CreatNewVert(HE_vert* vert,HE_edge* edge, HE_face* face)
{
	VERTEX_LIST vertices;
	vertices.push_back(vert);
	int n=face->valence;
	for (int i=0;i<n-1;i++) {
		vertices.push_back(edge->vert);
		edge=edge->next;
	}	
	float x=0,y=0,z=0;
	for (int k=0;k<n;k++) {
		HE_vert* vert=vertices[k];
		x+=(float)alpha(k,n)*vert->x;
		y+=(float)alpha(k,n)*vert->y;
		z+=(float)alpha(k,n)*vert->z;
	}
	HE_vert* newvert=new HE_vert(x,y,z);
	return(newvert);
}

Mesh3D* MeshSubdivision::Doo_Sabin()
{
	if (m_pmesh==NULL) {
		return NULL;
	}

	Mesh3D* New_mesh = new Mesh3D;

	////////////////////////////////////////////////

	//your implementation
	//New_mesh:  subdivisioned mesh
	//m_pmesh:  original mesh

	int FaceNum=m_pmesh->get_num_of_faces_list();
	int EdgeNum=m_pmesh->get_num_of_edges_list();
	int VertNum=m_pmesh->get_num_of_vertices_list();

	VERTEX_LIST * Fverts=new VERTEX_LIST[FaceNum];	//to store vertex pointers of each F-face
	VERTEX_LIST * Everts=new VERTEX_LIST[EdgeNum/2];//to store vertex pointers of each E-face
	VERTEX_LIST * Vverts=new VERTEX_LIST[VertNum];	//to store vertex pointers of each V-face

	NEW_VERT_LIST* newverts=new NEW_VERT_LIST[VertNum]; //to store all the new vertices

	//to create the new vertices for each vertex
	for (int i=0; i<VertNum;i++) {
		HE_vert* vert=m_pmesh->get_vertex(i);
		HE_edge* edge=vert->edge;
		do {
			NEW_VERT newvert;
			HE_face* face=edge->face;
			newvert.faceID=face->id;
			newvert.newvert=CreatNewVert(vert,edge,face);
			newverts[i].push_back(newvert);
			edge=edge->pair->next;
		} while(edge!=vert->edge);
		assert(newverts[i].size()==vert->degree);
	}

	for (int i=0;i<VertNum;i++) {
		HE_vert* thisvert=m_pmesh->get_vertex(i);
		for (int j=(int)newverts[i].size()-1;j>=0;j--) {
			HE_vert* vert=newverts[i][j].newvert;
			assert(vert!=NULL);
			HE_vert * tmpv = New_mesh->insert_vertex(vert->x,vert->y,vert->z);//insert the new vertices
			delete vert;
			vert = tmpv;
			newverts[i][j].newvert = tmpv;
			Vverts[i].push_back(tmpv);	//form a new V-face of each vertex
		}
		assert(Vverts[i].size()==thisvert->degree);
	}

	//form a new F-face of each face
	for (int i=0;i<FaceNum;i++) {
		HE_face* face=m_pmesh->get_face(i);
		HE_edge* edge=face->edge;
		do{
			HE_vert* vert=edge->vert;
			int ID=vert->id;
			for (int j=(int)newverts[ID].size()-1;j>=0;j--) {
				if (newverts[ID][j].faceID==i){
					HE_vert* vert=newverts[ID][j].newvert;
                    Fverts[i].push_back(vert);	
				}
			}
			edge=edge->next;
		}while(edge!=face->edge);
		assert(Fverts[i].size()==face->valence);
	}
	
	//form a new E-face of each edge
	for (int i=0,k=0;i<EdgeNum;i++) {

		HE_edge* edge=m_pmesh->get_edge(i);
		HE_edge* edge_pair=edge->pair;

		HE_vert* vert=edge->vert;
		HE_vert* vert_pair=edge_pair->vert;
		
		HE_face* face=edge->face;
		HE_face* face_pair=edge_pair->face;

		if (edge->id>edge_pair->id) continue;

		int fID1=face->id;int fID2=face_pair->id;int ID1=vert->id;int ID2=vert_pair->id;

		for (int j=0;j<(int)newverts[ID1].size();j++) {
			if (newverts[ID1][j].faceID==fID1){
				HE_vert* vert=newverts[ID1][j].newvert;
				Everts[k].push_back(vert);
				break;
			}
		}
		for (int j=0;j<(int)newverts[ID2].size();j++) {
			if (newverts[ID2][j].faceID==fID1){
				HE_vert* vert=newverts[ID2][j].newvert;
				Everts[k].push_back(vert);
				break;
			}
		}
		for (int j=0;j<(int)newverts[ID2].size();j++) {
			if (newverts[ID2][j].faceID==fID2){
				HE_vert* vert=newverts[ID2][j].newvert;
				Everts[k].push_back(vert);
				break;
			}
		}
		for (int j=0;j<(int)newverts[ID1].size();j++) {
			if (newverts[ID1][j].faceID==fID2){
				HE_vert* vert=newverts[ID1][j].newvert;
				Everts[k].push_back(vert);
				break;
			}
		}	
		assert(Everts[k].size()==4);
		k++;
	}

	for (int i=0;i<FaceNum;i++) {
		New_mesh->insert_face(Fverts[i]);	//insert F-faces
	}
	for (int i=0;i<EdgeNum/2;i++) {
		New_mesh->insert_face(Everts[i]);	//insert E-faces
	}
	for (int i=0;i<VertNum;i++) {
		New_mesh->insert_face(Vverts[i]);	////insert V-faces
	}
		
	delete [] Fverts, Fverts=NULL;
	delete [] Everts, Everts=NULL;
	delete [] Vverts, Vverts=NULL;
	delete [] newverts,newverts=NULL;
	////////////////////////////////////////////////

	New_mesh->update_mesh();

	delete m_pmesh,	m_pmesh = NULL;
	return New_mesh;
} 

//--------------------------------------------------------------
Mesh3D* MeshSubdivision::Catmull_Clark()
{
	if (m_pmesh==NULL) {
		return NULL;
	}

	Mesh3D* New_mesh = new Mesh3D;
	
	////////////////////////////////////////////////

	//your implementation
	//New_mesh:  subdivisioned mesh
	//m_pmesh:  original mesh
	
	
/************************************************************************/
  //you can do in the follow framework, or other way as you like.    
	int FaceNum=m_pmesh->get_num_of_faces_list();
	int EdgeNum=m_pmesh->get_num_of_edges_list();
	int VertNum=m_pmesh->get_num_of_vertices_list();
	NEW_VERT_LIST		Fnewverts;
	NEW_VERT_E_LIST		Enewverts;
	NEW_VERT_V_LIST		Vnewverts;

	//1 generate the new face points
	for (int i =0;i<FaceNum;i++)
	{
		VERTEX_LIST Fvertices;
		HE_face* face=m_pmesh->get_face(i);
		HE_edge* edge=face->edge;
		int n=face->valence;
		for (int i=0;i<n;i++) {
			Fvertices.push_back(edge->vert);
			edge=edge->next;
		}
		float x=0,y=0,z=0;
		for (int k=0;k<n;k++) {	//get point position.
			HE_vert* vert=Fvertices[k];
			x+=(1.0f/n)*vert->x;
			y+=(1.0f/n)*vert->y;
			z+=(1.0f/n)*vert->z;
		}
		NEW_VERT newvert;
		newvert.faceID=face->id;
		newvert.newvert=new HE_vert(x,y,z);
		HE_vert * tmpv = New_mesh->insert_vertex(newvert.newvert->x,newvert.newvert->y,newvert.newvert->z);
		newvert.newvert=tmpv;
		Fnewverts.push_back(newvert);	//store points.
	}
	assert(Fnewverts.size()==FaceNum);
	//2 generate the new edge points
	for (int i=0;i<EdgeNum;i++)
	{
		VERTEX_LIST Evertices;
		HE_edge* edge=m_pmesh->get_edge(i);
		HE_edge* edge_pair=edge->pair;

		if (edge->id>edge_pair->id) continue;

		HE_vert* vert=edge->vert;
		Evertices.push_back(vert);
		HE_vert* vert_pair=edge_pair->vert;
		Evertices.push_back(vert_pair);

		HE_face* face=edge->face;
		HE_face* face_pair=edge_pair->face;

		for(int i=0;i<FaceNum;i++)
		{
			if(Fnewverts[i].faceID==face->id)
			{
				Evertices.push_back(Fnewverts[i].newvert);
			}
			if(Fnewverts[i].faceID==face_pair->id)
			{
				Evertices.push_back(Fnewverts[i].newvert);
			}
		}
		float x=0,y=0,z=0;
		for (int k=0;k<4;k++) {		//get point position
			HE_vert* vert=Evertices[k];
			x+=(1.0f/4)*vert->x;
			y+=(1.0f/4)*vert->y;
			z+=(1.0f/4)*vert->z;
		}
		//add point to mesh.
		//new the orialmesh and newmesh.
		NEW_VERT_E newvert;
		newvert.edgeID=edge->id;
		newvert.edge_pairID=edge_pair->id;
		newvert.newvert=new HE_vert(x,y,z);
		HE_vert * tmpv = New_mesh->insert_vertex(newvert.newvert->x,newvert.newvert->y,newvert.newvert->z);
		newvert.newvert=tmpv;
		Enewverts.push_back(newvert);	
	}
	assert(Enewverts.size()==EdgeNum/2);
	//3 calculate the new vertex points
	for(int i=0;i<VertNum;i++)
	{
		VERTEX_LIST E_vertices;
		VERTEX_LIST Fvertices;

		HE_vert* vert=m_pmesh->get_vertex(i);
		HE_edge* edge=vert->edge;

		int n=vert->degree;
		do
		{
			for(int k=0;k<FaceNum;k++)
			{
				if(Fnewverts[k].faceID==edge->face->id)
				{
					Fvertices.push_back(Fnewverts[k].newvert);
				}
				
			}
			for(int k=0;k<EdgeNum/2;k++)
			{
				if(Enewverts[k].edgeID==edge->id||Enewverts[k].edge_pairID==edge->id)
				{
					E_vertices.push_back(Enewverts[k].newvert);
				}

			}
			edge=edge->pair->next;
		}while(edge!=vert->edge);
		//get the point position.
		float fx=0,fy=0,fz=0;
		for (int k=0;k<n;k++) {
			HE_vert* vertx=Fvertices[k];
			fx+=(-1.0f/(n*n))*vertx->x;
			fy+=(-1.0f/(n*n))*vertx->y;
			fz+=(-1.0f/(n*n))*vertx->z;
		}
		float ex=0,ey=0,ez=0;
		for (int k=0;k<n;k++) {
			HE_vert* vertx=E_vertices[k];
			ex+=(4.0f/(n*n))*vertx->x;
			ey+=(4.0f/(n*n))*vertx->y;
			ez+=(4.0f/(n*n))*vertx->z;
		}
		float x=0,y=0,z=0;
		x+=ex+fx+((float)(n-3)/n)*vert->x;
		y+=ey+fy+((float)(n-3)/n)*vert->y;
		z+=ez+fz+((float)(n-3)/n)*vert->z;
		//new the mesh.
		NEW_VERT_V newvert;
		newvert.vertexID=vert->id;
		newvert.newvert=new HE_vert(x,y,z);
		HE_vert * tmpv = New_mesh->insert_vertex(newvert.newvert->x,newvert.newvert->y,newvert.newvert->z);
		newvert.newvert=tmpv;
		Vnewverts.push_back(newvert);
	}
	assert(Vnewverts.size()==VertNum);

	//4 form new faces to give the new mesh
	for(int i=0;i<FaceNum;i++)
	{
		HE_face* face=m_pmesh->get_face(i);
		int n=face->valence;
		VERTEX_LIST * VertFace=new VERTEX_LIST[n];
		HE_vert* cvert;
		for(int j=0;j<FaceNum;j++)
		{
			if(Fnewverts[j].faceID==face->id)
				{
					cvert=Fnewverts[j].newvert;
				}
		}
		HE_edge* edge=face->edge;
		int k=0;
		do{
			
			HE_vert* nextEvert;
			HE_vert* Evert;
			HE_vert* Vvert;
			for(int l=0;l<VertNum;l++)
			{
				if(Vnewverts[l].vertexID==edge->vert->id)
				{
					Vvert=Vnewverts[l].newvert;
				}
			}

			for(int l=0;l<EdgeNum/2;l++)
			{
				if(Enewverts[l].edgeID==edge->id||Enewverts[l].edge_pairID==edge->id)
				{
					Evert=Enewverts[l].newvert;
				}
				if(Enewverts[l].edgeID==edge->next->id||Enewverts[l].edge_pairID==edge->next->id)
				{
					nextEvert=Enewverts[l].newvert;
				}
			}
			
			VertFace[k].push_back(cvert);
			VertFace[k].push_back(Evert);
			VertFace[k].push_back(Vvert);
			VertFace[k].push_back(nextEvert);

			New_mesh->insert_face(VertFace[k]);
			k++;
			edge=edge->next;
		}while(edge!=face->edge);
	}
/************************************************************************/	
	
	////////////////////////////////////////////////

	New_mesh->update_mesh();

	delete m_pmesh,	m_pmesh = NULL;
	return New_mesh;
}
