#ifndef FILE_MESHACCESS
#define FILE_MESHACCESS

/*********************************************************************/
/* File:   meshaccess.hpp                                            */
/* Author: Joachim Schoeberl                                         */
/* Date:   25. Mar. 2000                                             */
/*********************************************************************/


#include <nginterface.h>
#include <nginterface_v2.hpp>

namespace ngfem
{
  class ElementTransformation;
  class IntegrationPoint;
}



namespace ngcomp
{
  using namespace ngfem;


  using netgen::Ng_Element;
  using netgen::Ng_Point;
  using netgen::Ng_Node;

  using netgen::Ng_GetPoint;
  using netgen::Ng_GetElement;
  using netgen::Ng_GetNode;






  inline ELEMENT_TYPE ConvertElementType (NG_ELEMENT_TYPE type)
  {
    switch (type)
      {
      case NG_PNT: 
	return ET_POINT;

      case NG_SEGM: case NG_SEGM3:
	return ET_SEGM;

      case NG_TRIG: case NG_TRIG6: 
	return ET_TRIG;
	
      case NG_QUAD: case NG_QUAD6:
	return ET_QUAD;
	
      case NG_TET: case NG_TET10:
	return ET_TET;

      case NG_PRISM: case NG_PRISM12:
	return ET_PRISM;

      case NG_PYRAMID:
	return ET_PYRAMID;

      case NG_HEX:
	return ET_HEX;
      }
    throw Exception ("Netgen2NgS type conversion: Unhandled element type");
  }





  /** 
      Access to mesh geometry.
      This class provides topology, as element to vertex,
      element to face etc. Internally, NGSolve calls the Netgen
      mesh handler.
  */

  class NGS_DLL_HEADER MeshAccess : public BaseStatusHandler
  {

    /// buffered global quantities:
    /// dimension of the domain. Set to -1 if no mesh is present
    int dim;
  
    // number of vertex, edge, face, and cell nodes
    int nnodes[4];

    // number of nodes of co-dimension i 
    // these are NC, NF, NE, NV  in 3D,
    // and NF, NE, NV, undef, in 2D
    int nnodes_cd[4];


    /// number of elements of dimension i
    int nelements[4];  
    /// number of elements of co-dimension i
    int nelements_cd[4];

    /// number of multigrid levels 
    int nlevels;

    /// max domain index
    int ndomains;

    /// max boundary index
    int nboundaries;

  public:
    ///
    MeshAccess ();
    ///
    virtual ~MeshAccess ();

    /// problem dimension
    int GetDimension() const { return dim; }  

    /// number of points. needed for 6 node trigs, old-style
    int GetNP() const  { return Ng_GetNP(); }

    /// number of vertices
    int GetNV() const  { return nnodes[0]; }  

    /// number of elements in the domain
    int GetNE() const  { return nelements_cd[0]; }  

    /// number of boundary elements
    int GetNSE() const { return nelements_cd[1]; }  

    /// number of edges in the whole mesh
    int GetNEdges() const { return nnodes[1]; }     

    /// number of faces in the whole mesh
    int GetNFaces() const { return nnodes[2]; }    



    /// number of distinct domains
    int GetNDomains () const  { return ndomains; }

    /// number of distinct boundaries
    int GetNBoundaries () const { return nboundaries; }

    /*
    ///
    template <int D>
    void GetPoint (int pi, Vec<D> & p) const
    { Ng_GetPoint (pi+1, &p(0)); }

    ///
    template <int D>
    Vec<D> GetPoint (int pi) const
    { 
      Vec<D> p;
      Ng_GetPoint (pi+1, &p(0)); 
      return p;
    }
    */

    template <int D>
    void GetPoint (int pi, Vec<D> & p) const
    { 
      Ng_Point pt = Ng_GetPoint (pi);
      for (int j = 0; j < D; j++)
	p(j) = pt[j];
    }

    ///
    template <int D>
    Vec<D> GetPoint (int pi) const
    { 
      Vec<D> p;
      Ng_Point pt = Ng_GetPoint (pi);
      for (int j = 0; j < D; j++)
	p(j) = pt[j];
      return p;
    }

    ///
    ELEMENT_TYPE GetElType (int elnr) const
    { return ConvertElementType (GetElement(elnr).GetType()); }

    ///
    int GetElIndex (int elnr) const
    { return Ng_GetElementIndex (elnr+1)-1; }

    void SetElIndex (int elnr, int index) const
    { Ng_SetElementIndex (elnr+1,index+1); }

    string GetElMaterial (int elnr) const
    { return Ng_GetElementMaterial (elnr+1); }

    string GetDomainMaterial (int domnr) const
    { return Ng_GetDomainMaterial(domnr+1); }

    ///
    ELEMENT_TYPE GetSElType (int elnr) const
    { return ConvertElementType (GetSElement(elnr).GetType()); }
      
    ///
    int GetSElIndex (const int elnr) const
    /*
    {
      if (dim == 2)
        return Ng_GetElementIndex<1> (elnr)-1;
      else
        return Ng_GetElementIndex<2> (elnr)-1;
    }
    */
    { return Ng_GetSurfaceElementIndex (elnr+1)-1; }

    ///
    int GetSElSurfaceNumber (const int elnr) const
    { return Ng_GetSurfaceElementSurfaceNumber (elnr+1)-1; }
    ///
    int GetSElFDNumber (const int elnr) const
    { return Ng_GetSurfaceElementFDNumber (elnr+1)-1; }

    ///
    string GetSElBCName (const int elnr) const
    { return Ng_GetSurfaceElementBCName (elnr+1); }

    string GetBCNumBCName (const int bcnr) const
    { return Ng_GetBCNumBCName(bcnr); }

    //  ///
    //  string GetSElBCName (const int elnr) const;
    //  ///
    //  string GetBCNumBCName (const int bcnr) const;

    void GetSElNeighbouringDomains(const int elnr, int & in, int & out) const
    { 
      Ng_GetSurfaceElementNeighbouringDomains(elnr+1,in,out);
      //in--; out--;
    }

  
    /*
      update buffered global quantities.
      Must be called after every change in the mesh
    */
    void UpdateBuffers();


    /*
      Nodes are an abstraction for vertices, edges, faces, and cells
    */

    /// number of elements of dimension dim
    int GetNElements (int dim) const { return nelements[dim]; }

    /// number of nodes of type nt
    int GetNNodes (NODE_TYPE nt) const { return nnodes[nt]; }  

    /// the topology of a domain - element
    void GetTopologicElement (int elnr, TopologicElement & topel) const;

  

    /*
      So moechte ich es gern (JS):

      Anstelle von GetFaceEdges (fanr, edges) etc macht man dann 
      GetClosureNodes (NT_FACE, fanr, NodeSet (NT_EDGE), nodes);

      und GetTopologicElement in 3D ruft
      GetClosureNodes (NT_CELL, cell, NodeSet (NT_VERTEX, NT_EDGE, NT_FACE, NT_CELL), nodes);
    */
    void GetClosueNodes (NODE_TYPE nt, int nodenr, NODE_SET ns, Array<Node> & nodes);

    Ng_Element GetElement (int elnr) const
    {
      switch (dim)
	{
	case 1:	return Ng_GetElement<1> (elnr);
	case 2: return Ng_GetElement<2> (elnr);
	case 3: return Ng_GetElement<3> (elnr);
	default:
	  throw Exception ("GetElement, illegal dimension");
	}
    }

    template <int DIM>
    Ng_Element GetElement (int elnr) const
    {
      return Ng_GetElement<DIM> (elnr);
    }


    Ng_Element GetSElement (int elnr) const
    {
      switch (dim)
	{
	case 1:	return Ng_GetElement<0> (elnr);
	case 2: return Ng_GetElement<1> (elnr);
	case 3: return Ng_GetElement<2> (elnr);
	default:
	  throw Exception ("GetSElement, illegal dimension");
	}
    }

    template <int DIM>
    Ng_Node<DIM> GetNode (int nr) const
    {
      return Ng_GetNode<DIM> (nr);
    }

    // von astrid
    int GetElNV ( int elnr ) const;

    ///
    void GetElPNums (int elnr, Array<int> & pnums) const;
    ///
    void GetSElPNums (int selnr, Array<int> & pnums) const;

    void GetElVertices (int elnr, Array<int> & vnums) const
    {
      vnums = ArrayObject (GetElement(elnr).vertices);
    }
    ///
    void GetSElVertices (int selnr, Array<int> & vnums) const;
    ///
    void GetElEdges (int elnr, Array<int> & ednums) const
    {
      ednums = ArrayObject (GetElement(elnr).edges);
    }
    ///
    void GetElEdges (int elnr, Array<int> & ednums, Array<int> & orient) const;
    ///
    void GetSElEdges (int selnr, Array<int> & ednums) const;
    ///
    void GetSElEdges (int selnr, Array<int> & ednums, Array<int> & orient) const;
    ///
    void GetElFaces (int elnr, Array<int> & fnums) const;
    ///
    void GetElFaces (int elnr, Array<int> & fnums, Array<int> & orient) const;
    ///
    int GetSElFace (int selnr) const;
    ///
    void GetSElFace (int selnr, int & fnum, int & orient) const;
    ///
    void GetFacePNums (int fnr, Array<int> & pnums) const;
    ///
    void GetEdgePNums (int enr, int & pn1, int & pn2) const;
    ///
    void GetEdgePNums (int enr, Array<int> & pnums) const;
    ///
    void GetEdgeElements (int enr, Array<int> & elnums) const;
    ///
    void GetFaceEdges (int fnr, Array<int> & edges) const;
    ///
    void GetFaceElements (int fnr, Array<int> & elnums) const;
  
    void GetSegmentPNums (int snr, Array<int> & pnums) const;
    int GetSegmentIndex (int snr) const;

    ///
    int GetNFacets() const { return nnodes_cd[1]; } 
    ///
    void GetElFacets (int elnr, Array<int> & fnums) const;
    ///
    void GetSElFacets (int selnr, Array<int> & fnums) const;
    ///
    void GetFacetPNums (int fnr, Array<int> & pnums) const;
    ///
    ELEMENT_TYPE GetFacetType (int fnr) const;

    ///
    void GetFacetElements (int fnr, Array<int> & elnums) const
    { (GetDimension() == 2) ? GetEdgeElements(fnr, elnums) : GetFaceElements(fnr, elnums); }    

    // void GetVertexElements (int vnr, Array<int> & elnrs) const;
    ///
    int GetElOrder (int enr) const
    { return Ng_GetElementOrder (enr+1); } 
    ///
    INT<3> GetElOrders (int enr) const
    { 
      INT<3> eo; 
      Ng_GetElementOrders(enr+1,&eo[0],&eo[1],&eo[2]); 
      return eo; 
    } 
    ///
    void SetElOrder (int enr, int order) const
    { Ng_SetElementOrder (enr+1,order); }
    ///
    void SetElOrders (int enr, int ox, int oy, int oz) const
    { Ng_SetElementOrders (enr+1, ox,oy,oz); }
    
    ///
    int GetSElOrder (int enr) const
    { return Ng_GetSurfaceElementOrder (enr+1); } 
    ///
    INT<2> GetSElOrders (int enr) const
    { 
      INT<2> eo; 
      Ng_GetSurfaceElementOrders(enr+1,&eo[0],&eo[1]); 
      return eo; 
    } 
    ///
    void SetSElOrder (int enr, int order) const
    { Ng_SetSurfaceElementOrder (enr+1,order); }
    ///
    void SetSElOrders (int enr, int ox, int oy) const
    { Ng_SetSurfaceElementOrders (enr+1, ox,oy); }
    

    ///
    double ElementVolume (int elnr) const;
    ///
    double SurfaceElementVolume (int selnr) const;



    /// multigrid:
    int GetNLevels() const
    { return nlevels; }  // Ng_GetNLevels(); }

    ///
    void GetParentNodes (int pi, int * parents) const
    { 
      Ng_GetParentNodes (pi+1, parents);
      parents[0]--; parents[1]--; 
    }
    ///
    int GetParentElement (int ei) const
    { return Ng_GetParentElement (ei+1)-1; }
    ///
    int GetParentSElement (int ei) const
    { return Ng_GetParentSElement (ei+1)-1; }
  
    /// anisotropic clusters:
    int GetClusterRepVertex (int pi) const
    { return Ng_GetClusterRepVertex (pi+1)-1; }
    ///
    int GetClusterRepEdge (int pi) const
    { return Ng_GetClusterRepEdge (pi+1)-1; }
    ///
    int GetClusterRepFace (int pi) const
    { return Ng_GetClusterRepFace (pi+1)-1; }
    ///
    int GetClusterRepElement (int pi) const
    { return Ng_GetClusterRepElement (pi+1)-1; }

    ///
    // void GetElementTransformation (int elnr, ElementTransformation & eltrans) const;

    ///
    ngfem::ElementTransformation & GetTrafo (int elnr, bool boundary, LocalHeap & lh) const;

    ///
    // void GetSurfaceElementTransformation (int elnr, ElementTransformation & eltrans) const;




    /*
    // compatibility for a while ...
    void GetElementTransformation (int elnr, ElementTransformation & eltrans,
				   LocalHeap & lh) const
    {
      GetElementTransformation (elnr, eltrans);
    }

    void GetSurfaceElementTransformation (int elnr, ElementTransformation & eltrans,
					  LocalHeap & lh) const
    {
      GetSurfaceElementTransformation (elnr, eltrans);
    }
    */



    ///
    // ElementTransformation GetSurfaceTrafo (int elnr) const;
  
    void SetPointSearchStartElement(const int el) const;

    int FindElementOfPoint (FlatVector<double> point,
			    ngfem::IntegrationPoint & ip, 
			    bool build_searchtree,
			    const Array<int> * const indices = NULL) const;
    int FindElementOfPoint (FlatVector<double> point,
			    ngfem::IntegrationPoint & ip, 
			    bool build_searchtree,
			    const int index) const;
    int FindSurfaceElementOfPoint (FlatVector<double> point,
				   ngfem::IntegrationPoint & ip, 
				   bool build_searchtree,
				   const Array<int> * const indices = NULL) const;
    int FindSurfaceElementOfPoint (FlatVector<double> point,
				   ngfem::IntegrationPoint & ip, 
				   bool build_searchtree,
				   const int index) const;

    bool IsElementCurved (int elnr) const
    { return bool (Ng_IsElementCurved (elnr+1)); }

    void GetPeriodicVertices ( Array<ngstd::INT<2> > & pairs) const;
    int GetNPairsPeriodicVertices () const;
    void GetPeriodicVertices (int idnr, Array<ngstd::INT<2> > & pairs) const;
    int GetNPairsPeriodicVertices (int idnr) const;  

    void GetPeriodicEdges ( Array<ngstd::INT<2> > & pairs) const;
    int GetNPairsPeriodicEdges () const;
    void GetPeriodicEdges (int idnr, Array<ngstd::INT<2> > & pairs) const;
    int GetNPairsPeriodicEdges (int idnr) const;  


    virtual void PushStatus (const char * str) const;
    virtual void PopStatus () const;
    virtual void SetThreadPercentage (double percent) const;
    virtual void GetStatus (string & str, double & percent) const;

    virtual void SetTerminate(void) const;
    virtual void UnSetTerminate(void) const;
    virtual bool ShouldTerminate(void) const;
  
    ///// Added by Roman Stainko ....
    void GetVertexElements( int vnr, Array<int>& elems) const;

    void GetVertexSurfaceElements( int vnr, Array<int>& elems) const;


  private:
    Array<bool> higher_integration_order;
  public:
    void SetHigherIntegrationOrder(int elnr);
    void UnSetHigherIntegrationOrder(int elnr);



    void LoadMeshFromString(const string & str)
    {
      Ng_LoadMeshFromString(const_cast<char*>(str.c_str()));
    }



    // void PrecomputeGeometryData(int intorder);

    void InitPointCurve(double red = 1, double green = 0, double blue = 0) const;
    void AddPointCurvePoint(const Vec<3> & point) const;

    void GetDistantProcs (Node node, Array<int> & procs) const;
    int GetGlobalNodeNum (Node node) const;
  };







  class ProgressOutput
  {
    const MeshAccess & ma;
    string task;
    int total;
    double prevtime;
    bool is_root;
  public:
    ProgressOutput (const MeshAccess & ama,
		    string atask, int atotal);
    void Update (int nr);
    void Done();
  };



#ifdef PARALLEL
  template <typename T>
  void AllReduceNodalData (NODE_TYPE nt, Array<T> & data, MPI_Op op, const MeshAccess & ma,
			   MPI_Comm comm = ngs_comm)
  {
    int ntasks = MyMPI_GetNTasks (comm);
    if (ntasks <= 1) return;

    DynamicTable<T> dist_data(ntasks);
    Array<int> distprocs;

    for (int i = 0; i < ma.GetNNodes(nt); i++)
      {
	ma.GetDistantProcs (Node (nt, i), distprocs);
	for (int j = 0; j < distprocs.Size(); j++)
	  dist_data.Add (distprocs[j], data[i]);
      }
    
    Array<int> nsend(ntasks);
    for (int i = 0; i < ntasks; i++)
      nsend[i] = dist_data[i].Size();

    Table<T> recv_data(nsend);

    Array<MPI_Request> requests;
    for (int i = 0; i < ntasks; i++)
      {
	if (nsend[i])
	  requests.Append (MyMPI_ISend (dist_data[i], i, MPI_TAG_SOLVE, comm));
	if (nsend[i])
	  requests.Append (MyMPI_IRecv (recv_data[i], i, MPI_TAG_SOLVE, comm));
      }

    if (requests.Size())
      MyMPI_WaitAll (requests);

    Array<int> cnt(ntasks);
    cnt = 0;
    
    MPI_Datatype type = MyGetMPIType<T>();
    for (int i = 0; i < data.Size(); i++)
      {
	ma.GetDistantProcs (Node (nt, i), distprocs);
	for (int j = 0; j < distprocs.Size(); j++)
	  {
	    int dist_proc = distprocs[j];
	    MPI_Reduce_local (&recv_data[dist_proc][cnt[dist_proc]++], 
			      &data[i], 1, type, op);
	  }
      }
  }

#else
  template <typename T, typename MPI_Op>
  void AllReduceNodalData (NODE_TYPE nt, Array<T> & data, MPI_Op op, const MeshAccess & ma,
			   MPI_Comm comm = ngs_comm)
  { ; }
#endif



}

#endif
