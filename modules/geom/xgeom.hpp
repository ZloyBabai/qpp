#ifndef _QPP_GEOM_EXTRAS_H
#define _QPP_GEOM_EXTRAS_H

#include <data/types.hpp>
#include <geom/geom.hpp>
#include <io/strfun.hpp>
#include <vector>
#include <algorithm>
#include <initializer_list>
#include <stdexcept>

#ifdef PY_EXPORT
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <python/qppython.hpp>

namespace bp = boost::python;
namespace sn = boost::python::self_ns;
#endif

namespace qpp{

  // ---------------- Geometry with extra fields - for storing any additional data -------------------

  template<class REAL, class CELL>
  class xgeometry : public geometry<REAL,CELL>{
  
    std::vector<std::vector<REAL> >   _xreal;
    std::vector<std::vector<int> >    _xint;
    std::vector<std::vector<short> >  _xbool;
    std::vector<std::vector<STRING> > _xstring;

    std::vector<STRING>       _field_name;
    std::vector<basic_types>  _field_type;

    std::vector<int>          _field_idx;
    std::vector<bool>         _field_additive;

    int _nxreal, _nxint, _nxbool, _nxstring, _nfields;
    int ix_charge, ix_x, ix_y, ix_z, ix_atom, ix_mass, ix_number;

    using geometry<REAL,CELL>::_atm;
    using geometry<REAL,CELL>::_crd;
    using geometry<REAL,CELL>::size;
    //using geometry<DIM,REAL,CELL>::error;

    void init_xdefault()
    {
      _nxreal = _nxstring = _nxint = _nxbool = 0;
#ifdef PY_EXPORT
      py_fields.bind(this);
#endif
    }

  public:

    typedef xgeometry<REAL,CELL> SELF;

    using geometry<REAL,CELL>::DIM;
    using geometry<REAL,CELL>::name;
    using geometry<REAL,CELL>::cell;
    using geometry<REAL,CELL>::nat;

    inline int nfields() const
    { return _nfields; }

    inline STRING field_name(int i) const
    { return _field_name[i]; }

    inline basic_types field_type(int i) const
    { return _field_type[i]; }    

    // Constructors

    xgeometry(CELL & __cell,
	      int __nxstring, int __nxreal=0, int __nxint=0, int __nxbool=0, 
	      const STRING & __name = "") :
      _xstring(__nxstring), _xreal(__nxreal), _xint(__nxint), _xbool(__nxbool), 
      _field_name(__nxreal + __nxint + __nxbool + __nxstring),
      _field_type(__nxreal + __nxint + __nxbool + __nxstring),
      _field_idx(__nxreal + __nxint + __nxbool + __nxstring),
      _field_additive(__nxreal + __nxint + __nxbool + __nxstring),
      geometry<REAL,CELL>(__cell, __name)
    {
      init_xdefault();
      _nxreal   = __nxreal;
      _nxint    = __nxint; 
      _nxbool   = __nxbool;      
      _nxstring = __nxstring;
    }

    xgeometry(CELL & __cell,  const STRING & __name = "") : geometry<REAL,CELL>(__cell, __name)
    {
      init_xdefault();
    }

    xgeometry(int dim,  const STRING & __name = "") : geometry<REAL,CELL>(dim, __name)
    {
      init_xdefault();
    }


    void format(const std::vector<STRING> & fn, const std::vector<basic_types> & ft)
    {
      _field_name = fn;
      _field_type = ft;
      _nxstring = std::count(ft.begin(), ft.end(), type_string);
      _nxreal = std::count(ft.begin(), ft.end(), type_real);
      _nxint = std::count(ft.begin(), ft.end(), type_int);
      _nxbool = std::count(ft.begin(), ft.end(), type_bool);

      if (fn.size() != ft.size() || fn.size() != _nxstring + _nxreal + _nxint + _nxbool)
	{
	  // make error
	}

      _nfields = ft.size();
      _field_idx.resize(_nfields);
      _field_additive.resize(_nfields);
      std::fill(_field_additive.begin(),_field_additive.end(),false);

      int is=0, ib=0, ir=0, ii=0;

      ix_x = ix_y = ix_z = ix_atom = -1;
      ix_charge = ix_mass = ix_number = -1;

      for (int j=0; j<_nfields; j++)
	{
	  basic_types tp = field_type(j);
	  if ( tp == type_string )
	    {
	      _field_idx[j] = is++;
	      if ( field_name(j) == "atom" ) ix_atom = j;
	    }
	  else if ( tp == type_real || tp == attributes<REAL>::type )
	    {
	      _field_type[j] = type_real;
	      _field_idx[j] = ir++;
	      if ( field_name(j) == "x" )      ix_x = j;
	      else if ( field_name(j) == "y" ) ix_y = j;
	      else if ( field_name(j) == "z" ) ix_x = j;
	      else if ( field_name(j) == "charge" ) ix_charge = j;
	      else if ( field_name(j) == "mass" )   ix_mass   = j;
	    }
	  else if ( tp == type_int )
	    {
	      _field_idx[j] = ii++;
	      if ( field_name(j) == "number" ) ix_number = j;
	    }
	  else if ( tp == type_bool )
	    _field_idx[j] = ib++;
	  else
	    {
	    //make error
	    }
	}

      if (ix_atom == -1 && ix_x == -1 && ix_y==-1 && ix_z==-1)
	{
	  ix_atom = 0;
	  ix_x = 1; ix_y = 2; ix_z = 3;
	  if (ix_charge >= 0) ix_charge += 4;
	  if (ix_mass >= 0)   ix_mass   += 4;
	  if (ix_number >= 0) ix_number += 4;

	  _nfields  += 4;
	  _nxstring += 1;
	  _nxreal   += 3;

	  std::vector<STRING> inss = {"atom","x","y","z"};
	  _field_name.insert(_field_name.begin(), inss.begin(), inss.end());

	  std::vector<basic_types> inst = {type_string, type_real, type_real, type_real};
	  _field_type.insert(_field_type.begin(), inst.begin(), inst.end());

	  std::vector<int> insi = {0,0,1,2};
	  _field_idx.insert(_field_idx.begin(), insi.begin(), insi.end());

	  /*
	  for (int j=4; j<_nfields; j++)
	    if (field_type(j)==type_string)
	      _field_idx[j] += 1;
	    else if (field_type(j)==type_real)
	      _field_idx[j] += 3;
	  */

	}

      if (ix_atom == -1 || ix_x == -1 || ix_y == -1 || ix_z == -1)
	{
	  // make error
	}

      _xstring.resize(_nxstring);
      _xreal.resize(_nxreal);
      _xint.resize(_nxint);
      _xbool.resize(_nxbool);

      
    }

    xgeometry(CELL & __cell,  const std::vector<STRING> & fn, const std::vector<basic_types> & ft,
	      const STRING & __name = "") : geometry<REAL,CELL>(__cell, __name)
    {
      init_xdefault();
      format(fn,ft);
    }

    int nfields_string() const
    { return _nxstring; }

    int nfields_real() const
    { return _nxreal; }

    int nfields_int() const
    { return _nxint;}

    int nfields_bool() const
    { return _nxbool;}
    
    template<class T>
    T & xfield(int i, int j) 
    {
      basic_types ft = field_type(i);
      if (ft == type_real)
	ft = attributes<REAL>::type;
      if (attributes<T>::type != ft)
	throw std::invalid_argument("xgeometry::xfield - wrong type for the field " + t2s(i));

      if (attributes<T>::type == type_string)
	{
	  if (i==ix_atom)
	    return convert<T&,STRING&>::get(_atm[j]);
	  else
	    return  convert<T&,STRING&>::get(_xstring[_field_idx[i]][j]);
	}
      else if (attributes<T>::type == type_bool)
	return  convert<T&,short&>::get(_xbool[_field_idx[i]][j]);
      else if (attributes<T>::type == type_int)
	return convert<T&,int&>::get(_xint[_field_idx[i]][j]);
      else if (attributes<T>::type == attributes<REAL>::type)
	{
	  if (i==ix_x)
	    return convert<T&,REAL&>::get(_crd[j].x());
	  else if (i==ix_y)
	    return convert<T&,REAL&>::get(_crd[j].y());
	  else if (i==ix_z)
	    return convert<T&,REAL&>::get(_crd[j].z());
	  else
	    return convert<T&,REAL&>::get(_xreal[_field_idx[i]][j]);
	}
      else 
	throw std::invalid_argument("Illegal type of xgeometry extra field");
    }

    template<class T>
    T xfield(int i, int j) const
    {

      basic_types ft = field_type(i);
      if (ft == type_real)
	ft = attributes<REAL>::type;
      if (attributes<T>::type != ft)
	throw std::invalid_argument("xgeometry::xfield - wrong type for the field " + t2s(i));

      if (attributes<T>::type == type_string)
	{
	  if (i==ix_atom)
	    return convert<T,STRING>::get(_atm[j]);
	  else
	    return  convert<T,STRING>::get(_xstring[_field_idx[i]][j]);
	}
      else if (attributes<T>::type == type_bool)
	return  convert<T,short>::get(_xbool[_field_idx[i]][j]);
      else if (attributes<T>::type == type_int)
	return convert<T,int>::get(_xint[_field_idx[i]][j]);
      else if (attributes<T>::type == attributes<REAL>::type)
	{
	  if (i==ix_x)
	    return convert<T,REAL>::get(_crd[j].x());
	  else if (i==ix_y)
	    return convert<T,REAL>::get(_crd[j].y());
	  else if (i==ix_z)
	    return convert<T,REAL>::get(_crd[j].z());
	  else
	    return convert<T,REAL>::get(_xreal[_field_idx[i]][j]);
	}
      else 
	throw std::invalid_argument("Illegal type of xgeometry extra field");
    }
   
    template<class T>
    T & xfield(const STRING & f, int j)
    {
      int i = 0;
      while (_field_name[i]!=f && i<nfields()) i++;

      if (i<nfields())
	return xfield<T>(i,j);
      else
	{
	  // Make error
	}
    }

    template<class T>
    T xfield(const STRING & f, int j) const
    {
      int i = 0;
      while (_field_name[i]!=f && i<nfields()) i++;
      
      if (i<nfields())
	return xfield<T>(i,j);
      else
	{
	  // Make error
	}
    }
    
    // ----------------------------------------------------

    virtual void erase(int j)
    {     
      geometry<REAL,CELL>::erase(j);
      for (int i=0; i<_nxreal; i++)
	_xreal[i].erase(_xreal[i].begin()+j);
      for (int i=0; i<_nxint; i++)
	_xint[i].erase(_xint[i].begin()+j);
      for (int i=0; i<_nxbool; i++)
	_xbool[i].erase(_xbool[i].begin()+j);
    }

    void add(const STRING & a, const vector3d<REAL> & r,
	     std::initializer_list<STRING> xts, 
	     std::initializer_list<REAL> xtr = {}, 
	     std::initializer_list<int> xti = {}, 
	     std::initializer_list<bool> xtb = {})
    {

      int i = 0;
      for (const auto & xs : xts)
	_xstring[i++].push_back(xs);

      i = 0;
      for (const auto & xr : xtr)
	_xreal[i++].push_back(xr);

      i = 0;
      for (const auto & xi : xti)
	_xint[i++].push_back(xi);

      i = 0;
      for (const auto & xb : xtb)
	_xbool[i++].push_back(xb);

      geometry<REAL,CELL>::add(a,r);
    }

    void insert(const int j, const STRING & a, const vector3d<REAL> &r,
		std::initializer_list<STRING> xts, 
		std::initializer_list<REAL> xtr = {}, 
		std::initializer_list<int> xti = {}, 
		std::initializer_list<bool> xtb = {})
    {
      int i = 0;
      for ( const auto & xs : xts ) 
	_xstring[i].insert( _xstring[i++].begin() + j, xs);

      i = 0; 
      for ( const auto & xr : xtr ) 
	_xreal[i].insert( _xreal[i++].begin() + j, xr);

      i = 0; 
      for ( const auto & xi : xti ) 
	_xint[i].insert( _xint[i++].begin() + j, xi);

      i = 0; 
      for ( const auto & xb : xtb ) 
	_xbool[i].insert( _xbool[i++].begin() + j, xb);

      geometry<REAL,CELL>::insert(j,a,r);
    }

    virtual void add(const STRING &a, const vector3d<REAL> & r)
    {
      for (int i=0; i<_nxstring; i++)
	_xstring[i].push_back( "" );
      for (int i=0; i<_nxreal; i++)
	_xreal[i].push_back( REAL(0) );
      for (int i=0; i<_nxint; i++)
	_xint[i].push_back(0);
      for (int i=0; i<_nxbool; i++)
	_xbool[i].push_back( false );
      geometry<REAL,CELL>::add(a,r);
    }

    virtual void insert(const int j, STRING a, const vector3d<REAL> &r)
    {
      for (int i=0; i<_nxstring; i++)
	_xstring[i].insert(_xstring[i].begin()+j, "" );
      for (int i=0; i<_nxreal; i++)
	_xreal[i].insert(_xreal[i].begin()+j, REAL(0) );
      for (int i=0; i<_nxint; i++)
	_xint[i].insert(_xint[i].begin()+j, 0);
      for (int i=0; i<_nxbool; i++)
	_xbool[i].insert(_xbool[i].begin()+j, false);
      geometry<REAL,CELL>::insert(j,a,r);
    }
    
    /*
    virtual void add(STRING a, REAL _x, REAL _y, REAL _z)
    {
      add(a, {_x,_y,_z});
    }
    
    virtual void insert(const int j, STRING a, const REAL _x, const REAL _y, const REAL _z)
    {
      insert(j,a, {_x,_y,_z});
    }
    */
    // ---------------------------------------------------------

    template<class T>
    void parse_field(int i, int j, T t)
    {
      basic_types f = field_type(i);
      if (f==type_string)
	xfield<STRING>(i,j) = convert<STRING,T>::get(t);
      else if (f==type_real)
	xfield<REAL>(i,j) = convert<REAL,T>::get(t);
      else if (f==type_int)
	xfield<int>(i,j) = convert<int,T>::get(t);
      else if (f==type_bool)
	xfield<bool>(i,j) = convert<bool,T>::get(t);
    }

    // ----------------------------------------

    template<class T, typename... Args>
    void parse(int i, int j, T t)
    {
      parse_field(i,j,t);
    }

    template<class T, typename... Args>
    void parse(int i, int j, T t, Args... args)
    {
      parse_field(i,j,t);
      parse(i+1, j, args...);
    }

    // ----------------------------------------

    template<typename... Args>
    void xfill(int j, Args... args)
    { 
      parse(0,j,args...); 
    }

    template<typename... Args>
    void xadd(Args... args)
    {
      add("",{REAL(0),REAL(0),REAL(0)});
      xfill(size()-1, args... );
    }

    template<typename... Args>
    void xinsert(int at, Args... args)
    {
      insert(at,"",{REAL(0),REAL(0),REAL(0)});
      xfill(at, args... );
    }

    // ----------------------------------------
    
    virtual void write(std::basic_ostream<CHAR,TRAITS> &os, int offset=0) const
    {
      for (int k=0; k<offset; k++) os << " ";
      os << "geometry";
      if (name != "")
	os << " " << name;
      os << "(";
      cell.write(os);
      
      for (int i=0; i<nfields(); i++)
	{
	  std::cout << ", " << field_name(i) << " = ";
	  if (field_type(i)==type_string)
	    std::cout << "string";
	  else if (field_type(i)==type_real)
	    std::cout << "real";
	  else if (field_type(i)==type_int)
	    std::cout << "int";
	  else if (field_type(i)==type_bool)
	    std::cout << "bool";
	}
      os << ")\n";

      for (int k=0; k<offset+2; k++) os << " ";
      os << "{\n";      

      for (int i=0; i<size(); i++)
	{
	  for (int k=0; k<offset+3; k++) os << " ";
	  for (int j=0; j<nfields(); j++)
	    if (field_type(j)==type_string)
	      os << " " << xfield<STRING>(j,i);
	    else if (field_type(j)==type_real)
	      os << " " << xfield<REAL>(j,i);
	    else if (field_type(j)==type_int)
	      os << " " << xfield<int>(j,i);
	    else if (field_type(j)==type_bool)
	      os << " " << t2s(xfield<bool>(j,i));
	  os << "\n";
	}

      for (int k=0; k<offset+2; k++) os << " ";      
      os << "}\n";
    }

    // Some named fields ----------------------------------------------

    bool has_charge()
    { return ix_charge >= 0; }

    REAL charge(int i) const
    {
      if (ix_charge >= 0)
	return _xreal[_field_idx[ix_charge]][i]; 
      else
	throw std::runtime_error("\"charge\" field is requested for the geometry which does not have charges");
    }

    REAL & charge(int i)
    {
      if (ix_charge >= 0)
	return _xreal[_field_idx[ix_charge]][i]; 
      else
	throw std::runtime_error("\"charge\" field is requested for the geometry which does not have charges");
    }

    //-----------------------------------------------------------

    bool has_number()
    { return ix_number >= 0; }

    int number(int i) const
    {
      if (ix_number >= 0)
	return _xint[_field_idx[ix_number]][i]; 
      else
	throw std::runtime_error("\"number\" field is requested for the geometry which does not have numbers");
    }

    int & number(int i)
    {
      if (ix_number >= 0)
	return _xint[_field_idx[ix_number]][i]; 
      else
	throw std::runtime_error("\"number\" field is requested for the geometry which does not have numbers");
    }

    //-----------------------------------------------------------

    bool has_mass()
    { return ix_mass >= 0; }

    REAL mass(int i) const
    {
      if (ix_mass >= 0)
	return _xreal[_field_idx[ix_mass]][i]; 
      else
	throw std::runtime_error("\"mass\" field is requested for the geometry which does not have masses");
    }

    REAL & mass(int i)
    {
      if (ix_mass >= 0)
	return _xreal[_field_idx[ix_mass]][i]; 
      else
	throw std::runtime_error("\"mass\" field is requested for the geometry which does not have masses");
    }

#ifdef PY_EXPORT

    xgeometry(CELL & __cell, const bp::list f, const STRING & __name = "") : 
      geometry<REAL,CELL>(__cell, __name)
    {
      init_xdefault();

      std::vector<STRING> fn;
      std::vector<basic_types> ft;

      for (int i=0; i<bp::len(f); i++)
	{
	  if (!bp::extract<bp::tuple>(f[i]).check())
	    TypeError("In xgeometry constructor - bad format list");

	  bp::tuple t = bp::extract<bp::tuple>(f[i]);

	  if (bp::len(t)!=2 || !bp::extract<STRING>(t[0]).check() || !bp::extract<STRING>(t[1]).check())
	    TypeError("In xgeometry constructor - bad format list");

	  fn.push_back(bp::extract<STRING>(t[0]));
	  STRING s = bp::extract<STRING>(t[1]);

	  if (s=="string")
	    ft.push_back(type_string);
	  else if (s=="real")
	    ft.push_back(type_real);
	  else if (s=="int")
	    ft.push_back(type_int);
	  else if (s=="bool")
	    ft.push_back(type_bool);
	  else
	    KeyError("In xgeometry constructor - bad field type");
	}
      format(fn,ft);
    }

    virtual bp::list py_getitem(int j) const
    {
      if (j<0) j+=nat();
      if (j<0 || j>= nat()) IndexError("xgeometry:: index out of range");
      bp::list l;
      for (int i=0; i<nfields(); i++)
	{
	  if (field_type(i)==type_string)
	    l.append(xfield<STRING>(i,j));
	  else if (field_type(i)==type_real)
	    l.append(xfield<REAL>(i,j));
	  else if (field_type(i)==type_int)
	    l.append(xfield<int>(i,j));
	  else if (field_type(i)==type_bool)
	    l.append(xfield<bool>(i,j));
	}
      return l;
    }

    virtual void py_setitem(int j, const bp::list & l)
    {       
      if (j<0) j+=nat();
      if (j<0 || j>= nat()) IndexError("xgeometry:: index out of range");
      if (bp::len(l) != nfields())
	TypeError("xgeometry:: bad list of fields");

      for (int i=0; i<nfields(); i++)
	{
	  if (field_type(i)==type_string)
	    {
	      if (!bp::extract<STRING>(l[i]).check())
		TypeError("xgeometry:: bad list of fields");
	      xfield<STRING>(i,j) = bp::extract<STRING>(l[i]);
	    }
	  else if (field_type(i)==type_real)
	    {
	      if (!bp::extract<REAL>(l[i]).check())
		TypeError("xgeometry:: bad list of fields");
	      xfield<REAL>(i,j) = bp::extract<REAL>(l[i]);
	    }
	  else if (field_type(i)==type_int)
	    {
	      if (!bp::extract<int>(l[i]).check())
		TypeError("xgeometry:: bad list of fields");
	      xfield<int>(i,j) = bp::extract<int>(l[i]);
	    }
	  else if (field_type(i)==type_bool)
	    {
	      if (!bp::extract<bool>(l[i]).check())
		TypeError("xgeometry:: bad list of fields");
	      xfield<bool>(i,j) = bp::extract<bool>(l[i]);
	    }
	}
    }    

    object py_getfield1(int i) 
    {
      KeyError("xgeometry::field accepts 2 indicies");
    }

    void py_setfield1(int i, const object & o)
    {
      KeyError("xgeometry::field accepts 2 indicies");
    }

    object py_getfield(int i, int j) 
    {
      if (i<0) i += nfields();
      if (i<0 || i >= nfields())
	IndexError("xgeometry: field index out of range");

      if (j<0) j+= nat();
      if (j<0 || j>=nat())
	IndexError("xgeometry: atom index out of range");

      if (field_type(i)==type_string)
	return object(xfield<STRING>(i,j));
      else if (field_type(i)==type_real)
	return object(xfield<REAL>(i,j));
      else if (field_type(i)==type_int)
	return object(xfield<int>(i,j));
      else if (field_type(i)==type_bool)
	return object(xfield<bool>(i,j));
    }

    void py_setfield(int i, int j, const object & o)
    {
      if (i<0) i += nfields();
      if (i<0 || i >= nfields())
	IndexError("xgeometry: field index out of range");

      if (j<0) j+= nat();
      if (j<0 || j>=nat())
	IndexError("xgeometry: atom index out of range");

      basic_types t = field_type(i);
      if (t==type_string)
	{
	  if ( !extract<STRING>(o).check())
	    TypeError("xgeometry: string value of the field expected");
	  xfield<STRING>(i,j) = extract<STRING>(o);
	}
      else if (t==type_real)
	{
	  if ( !extract<REAL>(o).check())
	    TypeError("xgeometry: real value of the field expected");
	  xfield<REAL>(i,j) = extract<REAL>(o);
	}
      else if (t==type_int)
	{
	  if ( !extract<int>(o).check())
	    TypeError("xgeometry: int value of the field expected");
	  xfield<int>(i,j) = extract<int>(o);
	}
      else if (t==type_bool)
	{
	  if ( !extract<bool>(o).check())
	    TypeError("xgeometry: bool value of the field expected");
	  xfield<bool>(i,j) = extract<bool>(o);
	}
    }

    py_2indexed_property<SELF, object, object, int, &SELF::py_getfield1, &SELF::py_setfield1, 
			 &SELF::py_getfield, &SELF::py_setfield > py_fields;

    virtual void py_add_list(const bp::list & l)
    {
      geometry<REAL,CELL>::add("",0,0,0);
      py_setitem(nat()-1,l);
    }

    static void py_export(const char * pyname)
    {
      py_2indexed_property<SELF, object, object, int, &SELF::py_getfield1, &SELF::py_setfield1, 
			   &SELF::py_getfield, &SELF::py_setfield >::py_2export("noname");

      class_<xgeometry<REAL,CELL>, bases<geometry<REAL,CELL> > >
	(pyname, init<CELL&, optional<const STRING&> >() )
	.def(init<CELL&,const list &, optional<const STRING &> >())
	.def_readwrite("field", & SELF::py_fields)
	.def("nfields", & SELF::nfields)
	.def("field_name", & SELF::field_name)
      ;
    }


#endif
    
  };

};

#endif