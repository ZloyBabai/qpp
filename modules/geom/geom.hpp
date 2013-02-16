#ifndef _QPP_GEOM_H
#define _QPP_GEOM_H

#include <lace/lace3d.hpp>
#include <lace/lace.hpp>
#include <symm/symm.hpp>
#include <vector>
#include <cmath>

namespace qpp{

  //fixme - bring all numeric constansts and tolerance thresholds to one place
  double symmtol = 1e-5;

  // Very simple realization of vectors of fixed dimesion
  // Presumably dimesion is 1, 2, or 3 (however, it can larger)
  // simple vectors can be added, subtracted and multiplied by number
  //
  // Defined here just for convinience of manipulating 
  // fractional coordinates and related values
  //
  // No attempts are made to optimize the data manipulation within
  // this type, due to very small dimensions the overhead is 
  // considered to be negligible
  //  template <class ELEM,int DIM>
  //class simple_vector{
  //  ELEM data[DIM];
  //public:
    
  //  simple_vector()
  //  {}

  //  simple_vector(ELEM *_data)
  //  {
  //    for (int i=0; i<DIM; i++)
  //	data[i]=_data[i];
  //  }
    
  //  inline ELEM & operator()(int i)
  //  { return data[i]; }
    
  //  inline simple_vector<ELEM,DIM>  operator+(simple_vector b)
  //  {
  //    ELEM cdata[DIM];
  //    for (int i=0; i<DIM; i++)
  //	cdata[i] = data[i]+b.data[i];
  //   return simple_vector<ELEM,DIM>(cdata);
  //  }

  //  inline simple_vector<ELEM,DIM>  operator-(simple_vector b)
  // {
  //  ELEM cdata[DIM];
  //  for (int i=0; i<DIM; i++)
  //	cdata[i] = data[i]-b.data[i];
  //  return simple_vector<ELEM,DIM>(cdata);
  //}

  //inline simple_vector<ELEM,DIM>  operator*(ELEM b)
  //{
  //  ELEM cdata[DIM];
  //  for (int i=0; i<DIM; i++)
  //	cdata[i] = data[i]*b;
  //  return simple_vector<ELEM,DIM>(cdata);
  //}
    
  //};
  
  //template <class ELEM,int DIM>
  //inline simple_vector<ELEM,DIM>  operator*(ELEM a, simple_vector<ELEM,DIM> b)
  //{
  //  ELEM cdata[DIM];/
  //  for (int i=0; i<DIM; i++)
  //    cdata[i] = a*b(i);
  //  return simple_vector<ELEM,DIM>(cdata);
  // }

  // functions to simplify the creation of 1,2 and 3-dimensional
  // simple vectors

  //template <class ELEM>  
  //simple_vector<ELEM,1> sv1(ELEM x)
  //{
  //simple_vector<ELEM,1> v;
  ///v(0) = x;
  // return v;
  //}

  //template <class ELEM>  
  //simple_vector<ELEM,2> sv2(ELEM x, ELEM y)
  // {
  //simple_vector<ELEM,2> v;
  //v(0) = x; v(1)=y;
  //return v;
  //}

  //template <class ELEM>  
  //simple_vector<ELEM,3> sv1(ELEM x, ELEM y, ELEM z)
  //{
  //simple_vector<ELEM,3> v;
  //v(0) = x; v(1) = y; v(2) = z;
  //return v;
  //}
    
  //--------------------------------------------------------------//

  // The struct to store 1, 2 or 3 translation vectors
  template<int DIM,class VALTYPE=double>
  struct periodic_cell{
    lace::vector3d<VALTYPE> v[DIM];

    periodic_cell()
    {}

    periodic_cell(VALTYPE a, VALTYPE b, VALTYPE c,
		  VALTYPE alpha, VALTYPE beta, VALTYPE gamma)
    // for DIM==3
    {
      v[0] = lace::vector3d<VALTYPE>(a,VALTYPE(0),VALTYPE(0));
      v[1] = lace::vector3d<VALTYPE>(b*std::cos(gamma), b*std::sin(gamma),
				     VALTYPE(0));
      VALTYPE nx = std::cos(beta);
      VALTYPE ny = (std::cos(alpha) - nx*std::cos(gamma))/std::sin(gamma);
      VALTYPE nz = std::sqrt(1-nx*nx-ny*ny);
      v[2] = lace::vector3d<VALTYPE>(nx,ny,nz)*c;
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
    
    //      inline operator int(){return at;}
    
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

  //--------------------------------------------------------------//


  // The geometry class stores atoms together with their
  // coordinates. As POINT is a template parameter, you can
  // use almost everything as POINT, even emply class.
  //
  // In this latter case you get just points with coordinates
  // Such object can be used to store, say, displacement 
  // vectors or vibrational mode vectors.
  //
  // If ATOM is something more substatial, you can store any information
  // about atoms as well

  // geometry is an ancestor for molecule
  template<int DIM, class VALTYPE>
  class geometry{

    int nat;
    // Number of atoms/points

    //    std::vector<POINT> atm;
    // Storage of atoms

    std::vector<lace::vector3d<VALTYPE> > crd;
    // Their coordinates

  public:
    periodic_cell<DIM,VALTYPE> cell;
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

      iterator(const geometry<DIM,VALTYPE> &g)
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
	  cll(d) = i.cell(d);
      }

      inline bool operator==(index<DIM> i)
      {
	bool res = (at == i.atom());
	if (res)
	  for (int d = 0; d<DIM; d++)
	    if ( cll(d) != i.cell(d) ) 
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
	  return at != i.atom() || cll(0) != i.cell(0);
	else if (DIM==2)
	  return at != i.atom() || cll(0) != i.cell(0) || cll(1) != i.cell(1);
	else if (DIM==3)
	  return at != i.atom() || cll(0) != i.cell(0) || cll(1) != i.cell(1) || cll(2) != i.cell(2);
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
		cll(d)++;
		if (cll(d) > b.cell(d))
		  {
		    for(int dd=0; dd<=d; dd++)
		      cll(d) = a.cell(dd);
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

    geometry(){nat=0;}

    geometry(lace::vector3d<VALTYPE> v1, lace::vector3d<VALTYPE> v2=0e0, 
	     lace::vector3d<VALTYPE> v3=0e0)
    {
      if (DIM>0)
	cell(0)=v1;
      if (DIM>1)
	cell(1)=v2;
      if (DIM>2)
	cell(2)=v3;
      nat = 0;
    }

    void add_point(const lace::vector3d<VALTYPE> & r)
    {
      //      atm.push_back(a);
      crd.push_back(r);
      nat++;
    }

    void add_point(VALTYPE _x, VALTYPE _y, VALTYPE _z)
    {
      //      atm.push_back(a);
      crd.push_back(lace::vector3d<VALTYPE>(_x,_y,_z));
      nat++;
    }

    void del_point(const int i)
    {
      //      atm.erase(i);
      crd.erase(crd.begin()+i);
      nat--;
    }

    void insert_point(const int i, const lace::vector3d<VALTYPE> &r)
    {
      //      atm.insert(i,a);
      crd.insert(crd.begin()+i,r);
      nat++;
    }
    
    void insert_point(const int i, const VALTYPE _x, const VALTYPE _y, const VALTYPE _z)
    {
      //      atm.insert(i,a);
      crd.insert(crd.begin()+i,lace::vector3d<VALTYPE>(_x,_y,_z));
      nat++;
    }

    inline int size(){return crd.size();}

    //    inline POINT& atom(index<DIM> i){return atm[i.atom()];}

    // Position gives the position of i-th atom
    inline lace::vector3d<VALTYPE>& point_coord(int i){return crd[i];}

    // Unlike position(i), coord(i) gives the coordinates of either
    // this atom in this unit cell or the coordinates of its image
    // in neighbouring cells
    // In other words
    // coord(i) = cell_coord(i) + atom_coord(i)
    inline lace::vector3d<VALTYPE> coord(index<DIM> i)
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

    void clear()
    {
      crd.clear();
      // atm.clear();
      nat = 0;
    }

  };

  // -----------------------------------------------------
  
  // Molecule vector is the 3N - dimensional vector, i.e.
  // the object which ascribes usual 3d vector to every atom
  // of some molecule.
  // These can be the displacements of atoms or normal mode vector,
  // or something else.

  template<int DIM, class VALTYPE>
  class molecule_vector
  {
    geometry<DIM,VALTYPE> *owner;

  public:
    std::vector<lace::vector3d<VALTYPE> > coord;

    //    friend 
    // molecule_vector<POINT,DIM,VALTYPE> operator*(VALTYPE s, const molecule_vector<POINT,DIM,VALTYPE> &v);

  public:

    molecule_vector(){owner=NULL;}

    molecule_vector(geometry<DIM,VALTYPE> *_owner)
    {
      setgeom(_owner);      
    }

    molecule_vector(const molecule_vector<DIM,VALTYPE> & v)
    {
      setgeom(v.owner);
      if (owner != NULL)
	for (int i=0; i<size(); i++)
	  coord[i] = v.coord[i];
    }

    inline geometry<DIM,VALTYPE> *geom() const
    {
      return owner;
    }

    void setgeom(geometry<DIM,VALTYPE> *_owner)
    {
      owner = _owner;
      if (owner!=NULL)
	{
	  coord.resize(owner -> size());
	  for (int i=0; i < coord.size(); i++)
	    coord[i] = VALTYPE(0);
	}
    }

    inline int size() const
    {
      return coord.size();
    }

    inline VALTYPE & operator()(int i, int j)
    {
      return coord[i](j);
    }

    inline lace::vector3d<VALTYPE> & operator()(int i)
    {
      return coord[i];
    }

    inline VALTYPE & operator[](int i)
    {
      return coord[i/3](i%3);
    }

    molecule_vector<DIM,VALTYPE>  operator+(const molecule_vector<DIM,VALTYPE> &v)
    {
      assert(size() == v.size());
      molecule_vector<DIM,VALTYPE> res(owner);
      for (int i=0; i < coord.size(); i++)
	res.coord[i] = coord[i] + v.coord[i];
      return res;
    }

    molecule_vector<DIM,VALTYPE>  operator-(const molecule_vector<DIM,VALTYPE> &v)
    {
      assert(size() == v.size());
      molecule_vector<DIM,VALTYPE> res(owner);
      for (int i=0; i < coord.size(); i++)
	res.coord[i] = coord[i] - v.coord[i];
      return res;
    }

    molecule_vector<DIM,VALTYPE>  operator*(VALTYPE s)
    {
      molecule_vector<DIM,VALTYPE> res(owner);
      for (int i=0; i < coord.size(); i++)
	res.coord[i] = s*coord[i];
      return res;
    }

    molecule_vector<DIM,VALTYPE>  operator/(VALTYPE s)
    {
      molecule_vector<DIM,VALTYPE> res(owner);
      for (int i=0; i < coord.size(); i++)
	res.coord[i] = coord[i]/s;
      return res;
    }

    VALTYPE norm()
    {
      VALTYPE s = VALTYPE(0);
      for (int i=0; i < coord.size(); i++)
	s += lace::scal(coord[i],coord[i]);
      return std::sqrt(s);
    }

  };

  template<int DIM, class VALTYPE>
  molecule_vector<DIM,VALTYPE> operator*(VALTYPE s, const molecule_vector<DIM,VALTYPE> &v)
  {
    molecule_vector<DIM,VALTYPE> res(v.geom());
    for (int i=0; i < v.coord.size(); i++)
      res.coord[i] = s*v.coord[i];
    return res;
  }

  template<int DIM, class VALTYPE>
  typename lace::numeric_type<VALTYPE>::real scal(molecule_vector<DIM,VALTYPE> &v1, 
						  molecule_vector<DIM,VALTYPE> &v2)
  {
    typename lace::numeric_type<VALTYPE>::real s;
    for (int i=0; i<v1.size()*3; i++)
      s += lace::conj(v1[i])*v2[i];
    return s;
  }

  template<int DIM, class VALTYPE, class TRANSFORM>
  molecule_vector<DIM,VALTYPE> operator*(TRANSFORM T, molecule_vector<DIM,VALTYPE> &v)
  {
    lace::matrix3d<VALTYPE> R(T);
    molecule_vector<DIM,VALTYPE> res(v.geom());
    for (int i=0; i < v.size(); i++)
      {
	lace::vector3d<VALTYPE> rnew = T * v.geom()->atom_coord(i);
	int j;
	bool found = false;
	for (j=0; j<v.size(); j++)
	  if ( lace::norm(rnew - v.geom()->atom_coord(j) ) <= symmtol )
	    {
	      found = true; break;
	    }
	// fixme - think about this behavoir for non-symmetric mols
	if (!found) j = i;
	res(j) = R*v(i);
      }
    
    return res;
  }
 
};

#endif
