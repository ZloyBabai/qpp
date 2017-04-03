#include <iostream>
#include <geom/geom.hpp>
#include <boost/format.hpp>

// -------------- Type table & neighbours table for simple molecule ---------------------

int main()
{
  qpp::geometry<3> g;
  
  g.cell(0) = lace::vector3d<double>(4,0,0);
  g.cell(1) = lace::vector3d<double>(0,6,0);
  g.cell(2) = lace::vector3d<double>(0,0,4);

  /*
  g.update_types = true;
  g.update_neighbours = true;
  */

  g.ngbr.default_distance = 1.2;
  g.ngbr.set_distance("Carbon","Carbon",1.8);
  g.ngbr.set_distance("Carbon","Oxygen",1.6);
  g.ngbr.set_distance("Coronium","Newtonium",8);
  g.ngbr.set_distance("Oxygen","Nitrogen",1.53);
  g.ngbr.set_distance("Carbon","Nitrogen",1.35);

  g.add("Carbon",       -1.676283510,      0.000000000,      1.911126199);
  g.add("Oxygen",       -1.753146399,      1.141923181,      1.514919538);
  g.add("Oxygen",       -1.604477009,     -1.066694960,      1.079653080);
  g.add("Hydrogen",       -1.628395866,     -0.711301327,      0.174843873);
  g.add("Coronium",       0,5,0);
  g.add("Carbon",       -1.647886655,     -0.422100383,      3.375898205);
  g.add("Hydrogen",       -2.475776531,     -1.122649475,      3.538396631);
  g.add("Coronium",       0,0,0);
  g.add("Newtonium",      0,0,0);
  g.add("Hydrogen",       -2.565816675,      1.205870339,      4.156284306);
  g.add("Hydrogen",       -0.956315270,      1.314146931,      4.156322052);
  g.add("Hydrogen",       -0.733613146,     -1.005425023,      3.538407817);
  g.insert(6, "Nitrogen",       -1.721124037,      0.666230718,      4.332560746);

  g.shadow(0) = true;
  g.shadow(1) = true;
  //  g.shadow(4) = true;

  g.build_type_table();
  g.ngbr.build_disttable();
  //  g.build_ngbr_table();


  std::cout << " --- Atomic types: ---\n";

  for (int i=0; i<g.n_atom_types(); i++)
    std::cout << i << " = " << g.atom_of_type(i) << "\n";

  std::cout << "\n --- Neighbouring distances table: ---\n";

  std::cout << "     ";
  for (int i=0; i<g.n_atom_types(); i++)
    std::cout << boost::format("%10i") % i;
  std::cout << "\n";

  for (int i=0; i<g.n_atom_types(); i++)
    {
      std::cout << boost::format("%5i") % i;
      for (int j=0; j<g.n_atom_types(); j++)
	std::cout << boost::format("%10.5f") % g.ngbr.distance(i,j);
      std::cout << "\n";
    }

  for (int i=0; i<g.size(); i++)
    std::cout << g.type_table(i) << " " << g.coord(i).x() << " " << g.coord(i).y() << " " << g.coord(i).z() << "\n";

  /*
  g._grain_setup();
  std::cout << g.grainsize << "\n";
  std::cout << g.Rmin << g.Rmax << g.grain_nx << " " << g.grain_ny << " " << g.grain_nz << "\n";

  g._graining();

  std::cout << "alive\n";

  int l=0;
  for (int i=0; i < g.grain_nx; i++ )
    for (int j=0; j < g.grain_ny; j++ )
      for (int k=0; k < g.grain_nz; k++ )
	for (int a=0; a< g.grains(i,j,k).size(); a++)
	  std::cout << i << " " << j << " " << k << " " << (g.grains(i,j,k)[a]) << "\n";
  */
  g.ngbr.build();

  std::cout << " --------------- ngbr table ------------------\n";

  for (int i=0; i<g.size(); i++)
    for (int j=0; j<g.ngbr.n(i); j++)
      {
	qpp::index<3> k = g.ngbr.table(i,j);
	std::cout << g.atom(i) << " " << i << " " << g.atom(k) << " " << k << " " <<  
	  norm(g.position(i) - g.position(k)) << "\n";
      }

  /*
  std::cout << "Iterator work:\n";
  qpp::geometry<3>::iterator i(g);
  for (i=i.begin(); i!=i.end(); i++)
    std::cout << i << " ";
  */
}
