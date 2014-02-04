#ifndef _QPP_GEOM_H
#define _QPP_GEOM_H

#include <vector>
#include <cmath>
#include <lace/lace3d.hpp>
#include <lace/lace.hpp>
#include <symm/symm.hpp>
#include <io/qppdata.hpp>
#include <constants.hpp>

namespace qpp{

  //--------------------------------------------------------------//

  // The struct to store 1, 2 or 3 translation vectors
  template<int DIM,class VALTYPE=double, class charT = char, class traits = std::char_traits<charT> >
  struct periodic_cell :  public qpp_object<charT,traits>{
    lace::vector3d<VALTYPE> v[DIM];

    using typename qpp_object<charT,traits>::string;

    string _name;

    periodic_cell(string __name = "")
    {
      _name = __name;
    }

    periodic_cell(VALTYPE a, VALTYPE b, VALTYPE c,
		  VALTYPE alpha, VALTYPE beta, VALTYPE gamma, string __name = "")
    // for DIM==3
    {
      v[0] = lace::vector3d<VALTYPE>(a,VALTYPE(0),VALTYPE(0));
      v[1] = lace::vector3d<VALTYPE>(b*std::cos(gamma), b*std::sin(gamma),
				     VALTYPE(0));
      VALTYPE nx = std::cos(beta);
      VALTYPE ny = (std::cos(alpha) - nx*std::cos(gamma))/std::sin(gamma);
      VALTYPE nz = std::sqrt(1-nx*nx-ny*ny);
      v[2] = lace::vector3d<VALTYPE>(nx,ny,nz)*c;

      _name = __name;
    }

    periodic_cell(lace::vector3d<VALTYPE > a, lace::vector3d<VALTYPE > b=0, lace::vector3d<VALTYPE > c=0)
    {
      _name = "";
      if (DIM>0)
	v[0] = a;
      if (DIM>1)
	v[1] = b;
      if (DIM>2)
	v[2] = c;
    }

    inline lace::vector3d<VALTYPE> & operator()(int i)
    { return v[i]; } 

    inline VALTYPE & operator()(int i, int j)
    { return v[i](j); }
    
    inline lace::vector3d<VALTYPE> frac2cart(lace::vector3d<VALTYPE> frac) const
    // transforms fractional coordinates to cartesian
    // Works for any DIM, but the vector frac still should be 3d
    // if DIM<3, frac(i) components with i>=DIM are not used
    { 
      lace::vector3d<VALTYPE> res=VALTYPE(0);
      for (int i=0; i<DIM; i++)
    	res+=frac(i)*v[i];
      return res;
    }
    
    //   inline lace::vector3d<VALTYPE> frac2cart(simple_vector<VALTYPE,DIM> frac)
    // fractional to cartesian coordinates
    // works for DIM=1,2,3
    // here we use simple_vector to pass fractional coords
    //{
    //  lace::vector3d<VALTYPE> res=VALTYPE(0);
    //  for (int i=0; i<DIM; i++)
    //	res+=frac(i)*v[i];
    // return res;
    //}

    inline lace::vector3d<VALTYPE> cart2frac(lace::vector3d<VALTYPE> r) const
    // cartesian to fractional
    // works for DIM==3 only
    { 
      lace::matrix3d<VALTYPE> A(v[0],v[1],v[2]);
      return lace::solve3d(A, r);
    }

    inline lace::vector3d<VALTYPE> reduce(lace::vector3d<VALTYPE> r)  const
    // Brings the point r into the volume of unit cell
    // by translations
    // unit cell is defined as parallelepiped with one vertex in
    // the coordinate origin
    // the others are pointed by v[0],v[1],v[2] vectors
    {
      lace::vector3d<VALTYPE> f = cart2frac(r);
      f(0) -= int(f(0));
      f(1) -= int(f(1));
      f(2) -= int(f(2));
      return frac2cart(f);
    }

    inline lace::vector3d<VALTYPE> reduce_cntr(lace::vector3d<VALTYPE> r) const
    // Brings the point r into the volume of unit cell
    // by translations
    // unit cell is defined as parallelepiped CENTRED in the
    // coordinate origin
    {
      lace::vector3d<VALTYPE> f = cart2frac(r);
      for (int i=0; i<3; i++)
        {
          f(i) -= int(f(i));
          if ( f(i) > VALTYPE(1)/2 ) f(i)-=1;
        }
      return frac2cart(f);
    }

    inline lace::vector3d<VALTYPE> reduce_wz(lace::vector3d<VALTYPE> r) const
    // Brings r into Wigner-Zeitz unit cell
    // fixme - implement this!
    {}

    inline bool in_cell(lace::vector3d<VALTYPE> r) const
    // Answers the question whether r belongs to the unit cell
    {
      lace::vector3d<VALTYPE> f = cart2frac(r);
      return 
        VALTYPE(0)<=f(0) && f(0) < VALTYPE(1) &&
        VALTYPE(0)<=f(1) && f(1) < VALTYPE(1) &&
        VALTYPE(0)<=f(2) && f(2) < VALTYPE(1);  
    }


    inline bool in_cell_cntr(lace::vector3d<VALTYPE> r) const
    // does r belong to unit cell centred at the coords origin?
    {
      lace::vector3d<VALTYPE> f = cart2frac(r);
      return 
        -VALTYPE(1)/2 <= f(0) && f(0) < VALTYPE(1)/2 &&
        -VALTYPE(1)/2 <= f(1) && f(1) < VALTYPE(1)/2 &&
        -VALTYPE(1)/2 <= f(2) && f(2) < VALTYPE(1)/2;   
    }

    inline bool in_cell_wz(lace::vector3d<VALTYPE> r) const
    // does r belong to Wigner-Zeitz unit cell
    // fixme - implement this!
    {}

    virtual string category()
    {
      return "vectors";
    }

    virtual string name()
    {
      return _name;
    }

    virtual int gettype()
    {
      int d;
      if (DIM==0)
	d = qppdata_dim0;
      else if (DIM==1)
	d = qppdata_dim1;
      else if (DIM==2)
	d = qppdata_dim2;
      else if (DIM==3)
	d = qppdata_dim3;
      return qppdata_vectors | d;
    }

    virtual void write(std::basic_ostream<charT,traits> &os, int offset=0)
    {
      for (int k=0; k<offset; k++) os << " ";
      os << "vectors";
      if (_name != "")
	os << " " << _name;
      os << "(" << DIM << "d){\n";

      for (int i=0; i<DIM; i++)
	{
	  for (int k=0; k<offset+2; k++) os << " ";
	  os << boost::format(" %11.6f %11.6f %11.6f\n") % v[i](0) % v[i](1) % v[i](2);	  
	}

      for (int k=0; k<offset; k++) os << " ";
      os << "}\n";
    }

  };

  // ------------------- index class ----------------------
  // Index is a handy tool to reference atoms in this geometry
  // as well as atoms in neighbouring cells
  // For that purpose it is a complex index - it contains, besides
  // the number of atom it is currently pointning at, also the 
  // indicies of the cell.
  template <int DIM>
  class index{
  protected:
    int at;
    int cll[DIM];    
    
  public:
    inline index& operator=(int _at)
    {
      at=_at;
      if (DIM>0)
	cll[0]=0;
      if (DIM>1)
	cll[1]=0;
      if (DIM>2)
	cll[2]=0;
      return *this;
    }
    
    inline operator int(){return at;}
    
    inline int atom(){return at;}
    
    inline int cell(int d){return cll[d];}
    
    inline void set(int _at, int i1=0, int i2=0, int i3=0)
    {
      at=_at;
      if (DIM>0)
	cll[0]=i1;
      if (DIM>1)
	cll[1]=i2;
      if (DIM>2)
	cll[2]=i3;
    }
    
    inline void setatom(int _at){at=_at;}
    
    inline void setcell(int d, int i){cll[d]=i;}
    
    index(){
      //	_check();
      set(0);
    }
    
    index(int _at, int i1=0, int i2=0, int i3=0)
    {
      //	_check();
      set(_at,i1,i2,i3);
    }      
    
    inline bool operator==(index<DIM> i)
    {
      if (DIM==0)
	return at == i.at;
      else if (DIM==1)
	return at == i.at && cll[0] == i.cll[0];
      else if (DIM==2)
	return at == i.at && cll[0] == i.cll[0] && cll[1] == i.cll[1];
      else if (DIM==3)
	return at == i.at && cll[0] == i.cll[0] && cll[1] == i.cll[1] && cll[2] == i.cll[2];
    }
    
  };

  /*  template<typename _CharT, class _Traits, int DIM>
  std::basic_ostream<_CharT, _Traits>&
  operator<<(std::basic_ostream<_CharT, _Traits>& __os, index<DIM> i)
  {
    std::basic_ostringstream<_CharT, _Traits> __s;
    __s.flags(__os.flags());
    __s.imbue(__os.getloc());
    __s.precision(__os.precision());
    
    __s << i.atom();
    if (DIM>0)
      {
	__s << "("  << i.cell(0);
	for (int d=1; d<DIM; d++)
	  __s << "," << i.cell(d);
	__s << ")";
      }
    return __os << __s.str();
  }
  */
  //--------------------------------------------------------------//


  // The geometry class stores atoms together with their
  // coordinates. As ATOM is a template parameter, you can
  // use almost everything as POINT, even emply class.
  //
  // In this latter case you get just points with coordinates
  // Such object can be used to store, say, displacement 
  // vectors or vibrational mode vectors.
  //
  // If ATOM is something more substatial, you can store any information
  // about atoms as well

  // geometry is an ancestor for molecule
  template< int DIM, class VALTYPE=double, class ATLABEL = std::string, class charT = char, 
	    class traits = std::char_traits<charT> >
  class geometry : public qpp_object<charT,traits>{

  protected:

    int nat;
    // Number of atoms/points

    std::vector<ATLABEL> atm;
    // Storage of atoms

    std::vector<lace::vector3d<VALTYPE> > crd;
    // Their coordinates

    using typename qpp_object<charT,traits>::string;

    string _name;

  private:

    VALTYPE *_ngbr_disttable;

  public:
    periodic_cell<DIM,VALTYPE,charT,traits> cell;
    bool update_types, update_neighbours;
    
    // Unit cell vectors for 1,2,3d periodicity
    
    // ------------------- iterator class --------------------
    // Iterator allows you run through all (or some) atoms of this cell
    // and of surrounding cells
    
    class iterator : public index<DIM>{
      
      index<DIM> a, b;
      // a - from
      // b - to
      
      using index<DIM>::at;
      using index<DIM>::cll;
      
    public:
      
      iterator(const geometry<DIM, VALTYPE, ATLABEL, charT, traits> &g)
      // default iterator goes through neighbouring cells only
      {
	a.setatom(0);
	for (int d=0; d < DIM; d++)
	  a.setcell(d,-1);
	b.setatom(g.nat - 1);
	for (int d=0; d < DIM; d++)
	  b.setcell(d,1);	
      }
      
      iterator(index<DIM> _a, index<DIM> _b)
      {
	a = _a;
	b = _b;
      }
      
      inline index<DIM> begin(){return a;}
      
      //      inline index end(){return b;}

      inline index<DIM> end(){return index<DIM>(-1,0,0,0);}

      inline iterator& operator=(index<DIM> i)
      {
	at = i.atom();
	for (int d = 0; d<DIM; d++)
	  cll[d] = i.cell(d);
      }

      inline bool operator==(index<DIM> i)
      {
	bool res = (at == i.atom());
	if (res)
	  for (int d = 0; d<DIM; d++)
	    if ( cll[d] != i.cell(d) ) 
	      {
		res = false;
		break;
	      }
	return res;
      }

      inline bool operator!=(index<DIM> i)
      {
	if (DIM==0)
	  return at != i.atom();
	else if (DIM==1)
	  return at != i.atom() || cll[0] != i.cell(0);
	else if (DIM==2)
	  return at != i.atom() || cll[0] != i.cell(0) || cll[1] != i.cell(1);
	else if (DIM==3)
	  return at != i.atom() || cll[0] != i.cell(0) || cll[1] != i.cell(1) || cll[2] != i.cell(2);
      }
               
      iterator& operator++(int)      
      {
	if (*this == b)
	  {
	    *this = end();
	    return *this;
	  }

	at++;
	if (at > b.atom() && DIM>0)
	  {
	    at=a.atom();
	    int d=0;
	    while(d < DIM)
	      {
		cll[d]++;
		if (cll[d] > b.cell(d))
		  {
		    for(int dd=0; dd<=d; dd++)
		      cll[d] = a.cell(dd);
		    d++;
		  }
		else 
		  break;
	      }
	  }
	return *this;

      }  

    };

    // ---------------------------------------------------------
  public:

    virtual string category()
    {
      return "geometry";
    }

    virtual string name()
    {
      return _name;
    }

    void setname(string __name)
    {
      _name = __name;
    }

    virtual int gettype()
    {
      int d;
      if (DIM==0)
	d = qppdata_dim0;
      else if (DIM==1)
	d = qppdata_dim1;
      else if (DIM==2)
	d = qppdata_dim2;
      else if (DIM==3)
	d = qppdata_dim3;
      return qppdata_geometry | d;
    }

    inline int size(){return crd.size();}

    inline ATLABEL& atom(index<DIM> i){return atm[i.atom()];}

    // coord gives the coordinates of i-th atom in the cell
    inline lace::vector3d<VALTYPE>& coord(int i){return crd[i];}

    // Unlike coord(i), full_coord(i) gives the coordinates of either
    // this atom in this unit cell or the coordinates of its image
    // in neighbouring cells
    // In other words
    // coord(i) = cell_coord(i) + atom_coord(i)
    inline lace::vector3d<VALTYPE> full_coord(index<DIM> i)
    {
      if (DIM==0)
	return crd[i.atom()];
      if (DIM==1)
	return crd[i.atom()] + cell(0)*i.cell(0);
      if (DIM==2)
	return crd[i.atom()] + cell(0)*i.cell(0) 
	  + cell(1)*i.cell(1);
      if (DIM==3)
	return crd[i.atom()] + cell(0)*i.cell(0) 
	  + cell(1)*i.cell(1) + cell(2)*i.cell(2);
    }    

    inline lace::vector3d<VALTYPE> cell_coord(int i1=0, int i2=0, int i3=0)
    {
      lace::vector3d<VALTYPE> r=0e0;
      if (DIM>0)
	r += i1*cell(0);
      if (DIM>1)
	r += i2*cell(1);
      if (DIM>2)
	r += i3*cell(2);
      return r;
    } 

    void scale(VALTYPE s)
    {
      for (int i=0; i<DIM; i++)
	cell(i) *= s;
      for (int i=0; i<nat; i++)
	crd[i] *= s;
    }

    virtual void write(std::basic_ostream<charT,traits> &os, int offset=0)
    {
      for (int k=0; k<offset; k++) os << " ";
      os << "geometry";
      if (_name != "")
	os << " " << _name;
      os << "(" << DIM << "d){\n";
      
      for (int i=0; i<size(); i++)
	{
	  for (int k=0; k<offset+2; k++) os << " ";
	  os << atm[i] << boost::format(" %11.6f %11.6f %11.6f\n") % crd[i].x() % crd[i].y() %crd[i].z();
	  
	}

      for (int k=0; k<offset; k++) os << " ";
      os << "}\n";
    }

    //------------------- Type table operations -------------------------

  private:
    std::vector<ATLABEL> _atm_types;
    std::vector<int> _type_table;

  public:

    // Number of atomic types in molecule
    inline int n_atom_types() const
    {
      return _atm_types.size();
    }
    
    // Reference to atom of type number t (not the atom number t in atomic list!)
    inline ATLABEL atom_of_type(int t) const
    {
      return _atm_types[t];
    }

    // Number of type of certain ATOM at
    inline int type_of_atom(const ATLABEL at) const
    {
      int t;
      for (t=0; t < _atm_types.size(); t++)
	if ( _atm_types[t] == at )
	  break;
      if (t == _atm_types.size())
	return -1;
      else 
	return t;
    }
    
    // 
    inline int type_table(int i) const
    {return _type_table[i];}


    virtual void build_type_table()
    {
      _atm_types.clear();
      _type_table.resize(size());
      
      for (int i=0; i<size(); i++)
	{
	  int t = type_of_atom(atom(i));
	  if (t == -1)
	    {
	      t = _atm_types.size();
	      _atm_types.push_back(atom(i));
	    }
	  _type_table[i] = t;
	}
      _ngbr_disttable_resize();
    }
    
    void clear_type_table()
    {
      _atm_types.clear();
      _type_table.resize(size());  
    }

    // ------------------- Neighbours table operations -----------------------
  private:
    std::vector<std::vector<index<DIM> > > _ngbr_table;
    
    struct _ngbr_record{ 
      ATLABEL at1, at2; 
      VALTYPE d;

      _ngbr_record(ATLABEL _at1, ATLABEL _at2, VALTYPE _d)
      {
	at1 = _at1;
	at2 = _at2;
	d = _d;
      }
    };

  public:
    lace::vector3d<VALTYPE> Rmin, Rmax;
    VALTYPE grainsize;
    int grain_nx, grain_ny, grain_nz;

    std::vector<std::vector<index<DIM> > > _grains; 

    std::vector<_ngbr_record> _ngbr_records;

    inline int _ngbr_record_match(ATLABEL at1, ATLABEL at2, int i)
    {
      return ( at1 == _ngbr_records[i].at1 && at2 == _ngbr_records[i].at2 ) ||
	( at2 == _ngbr_records[i].at1 && at1 == _ngbr_records[i].at2 );
    }

    inline void _ngbr_disttable_resize()
    {
      if (_ngbr_disttable!=NULL)
	delete _ngbr_disttable;
      _ngbr_disttable = new VALTYPE[n_atom_types()*n_atom_types()];
    }

    void _grain_setup()
    {
      grainsize = VALTYPE(0);
      for (int i=0; i<n_atom_types()*n_atom_types(); i++)
	if (_ngbr_disttable[i] > grainsize )
	  grainsize = _ngbr_disttable[i];
      Rmin = Rmax = coord(0);
      for (int i=1; i<size(); i++)
	{
	  if ( Rmin.x() > coord(i).x() ) 
	    Rmin.x() = coord(i).x();
	  if ( Rmax.x() < coord(i).x() ) 
	    Rmax.x() = coord(i).x();
	  if ( Rmin.y() > coord(i).y() ) 
	    Rmin.y() = coord(i).y();
	  if ( Rmax.y() < coord(i).y() ) 
	    Rmax.y() = coord(i).y();
	  if ( Rmin.z() > coord(i).z() ) 
	    Rmin.z() = coord(i).z();
	  if ( Rmax.z() < coord(i).z() ) 
	    Rmax.z() = coord(i).z();
	}
      grain_nx = int( (Rmax.x()-Rmin.x())/grainsize ) + 3;
      grain_ny = int( (Rmax.y()-Rmin.y())/grainsize ) + 3;
      grain_nz = int( (Rmax.z()-Rmin.z())/grainsize ) + 3;

      //debug
      //std::cout << "Grain setup:\n" << grain_nx<<"x"<<grain_ny<<"x"<<grain_nz<<"\n";
      //std::cout << "grain size= " << grainsize << " corners= " << Rmin << Rmax << "\n";
    }

    inline std::vector<index<DIM> > & grains(int i, int j, int k)
    {
      return _grains[i*grain_ny*grain_nz + j*grain_nz + k];
    }

    void _graining()
    {
      for (int i=0; i<_grains.size(); i++)
	_grains[i].clear();
      _grains.clear();
      _grains.resize(grain_nx*grain_ny*grain_nz);
      iterator at(*this);
      for ( at = at.begin(); at != at.end(); at++)
	{
	  lace::vector3d<VALTYPE> r = full_coord(at);
	  int i = int( (r.x()-Rmin.x())/grainsize ) + 1;
	  int j = int( (r.y()-Rmin.y())/grainsize ) + 1;
	  int k = int( (r.z()-Rmin.z())/grainsize ) + 1;

	  if ( i>=0 && i<grain_nx && j>=0 && j<grain_ny && k>=0 && k<grain_nz)
	    grains(i,j,k).push_back(at);
	}

      //      debug
      /*
      std::cout << "graining finished\n";
      for (int i=0; i<grain_nx; i++)
	for (int j=0; j<grain_ny; j++)
	  for (int k=0; k<grain_nz; k++)
	    {
	      std::cout << i << " " << j << " " << k;
	      for (int l=0; l<grains(i,j,k).size(); l++)
		std::cout << grains(i,j,k)[l];
	      std::cout << "\n";
	    }
      */
    }

  public:

    VALTYPE default_ngbr_distance;

    // Number of neighbours of i-th atom
    inline int n_ngbr(int i)
    {
      return _ngbr_table[i].size();
    }
    
    // j-th neighbour of i-th atom
    inline index<DIM> ngbr_table(int i, int j)
    {
      return _ngbr_table[i][j];
    }

    index<DIM> ngbr(index<DIM> i, int j)
    {
      index<DIM> res = ngbr_table(i,j);
      for (int a=0; a<DIM; a++)
	res.setcell( a, res.cell(a)+i.cell(a) );
      return res;
    }

    VALTYPE ngbr_distance(ATLABEL at1, ATLABEL at2)
    {
      bool found = false;
      int i;
      for (i=0; i<_ngbr_records.size(); i++)
	if (_ngbr_record_match(at1,at2,i))
	  {
	    found = true;
	    break;
	  }
      return found? _ngbr_records[i].d : default_ngbr_distance;
    }

    void set_ngbr_distance(ATLABEL at1, ATLABEL at2, VALTYPE d)
    {
      bool found = false;
      int i;
      for (i=0; i<_ngbr_records.size(); i++)
	if (_ngbr_record_match(at1,at2,i))
	  {
	    found = true;
	    break;
	  }
      if (found)
	_ngbr_records[i].d = d;
      else
	_ngbr_records.push_back(_ngbr_record(at1,at2,d));
    }

    VALTYPE ngbr_disttable(int i, int j)
    {
      if (_ngbr_disttable!=NULL)
	return _ngbr_disttable[i*n_atom_types()+j];
      else
	return default_ngbr_distance;
    }

    inline void build_ngbr_disttable()
    {
      int n = n_atom_types();
      for (int i=0; i<n; i++)
	for (int j=0; j<=i; j++)
	  _ngbr_disttable[n*i+j] = _ngbr_disttable[n*j+i] = 
	    ngbr_distance(atom_of_type(i),atom_of_type(j));
    }

    void build_ngbr_table()
    {
      _grain_setup();
      _graining();
      for (int i=0; i<_ngbr_table.size(); i++)
	_ngbr_table[i].clear();

      _ngbr_table.resize(size());

      for (int i = 1; i < grain_nx; i++)
	for (int j = 1; j < grain_ny; j++)
	  for (int k = 1; k < grain_nz; k++)
	    {
	      int g1 = i*grain_ny*grain_nz + j*grain_nz + k;
	      if (_grains[g1].size()>0)
		for (int di=-1; di<=0; di++)
		  for (int dj=-1; dj<=-di; dj++) 
		    for (int dk=-1; dk<=-di || dk<=-dj; dk++)
		      {
			int g2 = (i+di)*grain_ny*grain_nz + (j+dj)*grain_nz + k+dk;
			for (int c2 = 0; c2 < _grains[g2].size(); c2++)
			  for (int c1 = 0; c1 < ( g1==g2? c2 : _grains[g1].size()); c1++)
			    {
			      index<DIM> at1 = _grains[g1][c1];
			      index<DIM> at2 = _grains[g2][c2];
			      VALTYPE r = norm(full_coord(at1) - full_coord(at2));
			      if ( r <= ngbr_disttable(type_table(at1), type_table(at2)))
				{
				  if ( at1 == index<DIM>(at1,0,0,0) )
				    {
				      _ngbr_table[at1].push_back(at2);
				      if ( at2 != index<DIM>(at2,0,0,0) )
					{
					  for (int dd=0; dd<DIM; dd++)
					    at1.setcell(dd,-at2.cell(dd));
					  _ngbr_table[at2].push_back(at1);	
					}
				    }
				  if ( at2 == index<DIM>(at2,0,0,0) )
				    _ngbr_table[at2].push_back(at1);				  
				}
			    }
		      }
	    }
    }

    // --------------- Constructors & destructors --------------------

    geometry(string __name = ""){
      nat=0;
      _name = __name;
      update_types = false;
      update_neighbours = false;
      _ngbr_disttable = NULL;
      default_ngbr_distance = VALTYPE(0);
    }

    geometry(lace::vector3d<VALTYPE> v1, lace::vector3d<VALTYPE> v2=0e0, 
	     lace::vector3d<VALTYPE> v3=0e0, string __name = "")
    {
      if (DIM>0)
	cell(0)=v1;
      if (DIM>1)
	cell(1)=v2;
      if (DIM>2)
	cell(2)=v3;
      nat = 0;
      _name = __name;
      update_types = false;
      update_neighbours = false;
      _ngbr_disttable = NULL;
      default_ngbr_distance = VALTYPE(0);
    }

    ~geometry()
    {
      if (_ngbr_disttable!=NULL)
	delete _ngbr_disttable;
    }

    void copy(geometry<DIM, VALTYPE, ATLABEL, charT, traits> &G)
    {
      clear();
      nat = G.nat;
      atm = G.atm;
      crd = G.crd;
    }

    // ----------------------- Manipulations with atoms -----------------------

    virtual void add(ATLABEL a, const lace::vector3d<VALTYPE> & r)
    {
      atm.push_back(a);
      crd.push_back(r);
      nat++;
      if (update_types)
	{
	  int t = type_of_atom(a);
	  if (t == -1)
	    {
	      t = _atm_types.size();
	      _atm_types.push_back(a);
	      if (update_neighbours)
		{
		  _ngbr_disttable_resize();
		  build_ngbr_disttable();
		}	      
	    }
	  _type_table.push_back(t);
	}
    }

    virtual void add(ATLABEL a, const VALTYPE _x, const VALTYPE _y, const VALTYPE _z)
    {
      add(a,lace::vector3d<VALTYPE>(_x,_y,_z));
    }

    virtual void erase(const int i)
    {
      atm.erase(atm.begin()+i);
      crd.erase(crd.begin()+i);
      nat--;
      if (update_types)
	_type_table.erase(_type_table.begin()+i);
    }

    virtual void insert(const int i, ATLABEL a, const lace::vector3d<VALTYPE> &r)
    {
      atm.insert(atm.begin()+i,a);
      crd.insert(crd.begin()+i,r);
      nat++;
      if (update_types)
	{
	  int t = type_of_atom(a);
	  if (t == -1)
	    {
	      t = _atm_types.size();
	      _atm_types.push_back(a);
	      if (update_neighbours)
		{
		  _ngbr_disttable_resize();
		  build_ngbr_disttable();
		}
	    }
	  _type_table.insert(_type_table.begin()+i,t);
	}
    }
    
    virtual void insert(const int i, ATLABEL a, const VALTYPE _x, const VALTYPE _y, const VALTYPE _z)
    {
      insert(i,a,lace::vector3d<VALTYPE>(_x,_y,_z));
    }

    void clear()
    {
      crd.clear();
      atm.clear();
      nat = 0;
      clear_type_table();
    }

  };

};

#endif
