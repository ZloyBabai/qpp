#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <geom/lace3d.hpp>
#include <io/ccd_firefly.hpp>
#include <io/ccd_xyz.hpp>
#include <io/ccd_cp2k.hpp>
#include <fstream>

using namespace qpp;
using v3f = vector3<float>;
using v3d = vector3<double>;
using m3f = matrix3<float>;
using m3d = matrix3<double>;
using pd  = std::pair<double, double>;

TEST_CASE( "Computational chemistry data parsing : PC Gamess Firefly" ) {

  SECTION ("Parsing results of single-point calculations[PC Gamess Firefly]") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_sp.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    REQUIRE(cc_o.m_tot_nelec == 70) ;
    REQUIRE(cc_o.m_tot_charge == Approx(0)) ;
    REQUIRE(cc_o.m_DIM == 0);
    REQUIRE(cc_o.m_mult == 1);
    REQUIRE(cc_o.m_n_alpha == 35);
    REQUIRE(cc_o.m_n_beta == 35);
    REQUIRE(cc_o.m_tot_nat == 20);
    REQUIRE(cc_o.m_run_t == comp_chem_program_run_t::rt_energy);
    REQUIRE(cc_o.m_steps.back().m_scf_steps.size() == 9);
    REQUIRE(cc_o.m_steps.back().m_scf_steps.front().m_toten == Approx(-379.453243734));
    REQUIRE(cc_o.m_steps.back().m_scf_steps.back().m_toten == Approx(-379.777955008));
    REQUIRE(cc_o.m_steps.back().m_eigen_values.front() == Approx(-11.0409));
    REQUIRE(cc_o.m_steps.back().m_eigen_values.back() == Approx(1.1646));
    REQUIRE(cc_o.m_init_atoms_pos.size() == 20);
    REQUIRE(cc_o.m_init_atoms_names.size() == 20);
    REQUIRE(cc_o.m_init_atoms_pos[0] ==
        v3d(2.6342558126*bohr_to_angs, -0.4183391645*bohr_to_angs, 0.000));
    REQUIRE(cc_o.m_steps.back().m_mulliken_pop_per_atom.front().first == Approx(5.997223));
    REQUIRE(cc_o.m_steps.back().m_mulliken_pop_per_atom.front().second == Approx(0.002777));
    REQUIRE(cc_o.m_steps.back().m_mulliken_pop_per_atom.back().first == Approx(0.935496));
    REQUIRE(cc_o.m_steps.back().m_mulliken_pop_per_atom.back().second == Approx(0.064504));
    REQUIRE(cc_o.m_steps.back().m_lowdin_pop_per_atom.front().first == Approx(5.989834));
    REQUIRE(cc_o.m_steps.back().m_lowdin_pop_per_atom.front().second == Approx(0.010166));
    REQUIRE(cc_o.m_steps.back().m_lowdin_pop_per_atom.back().first == Approx(0.966150));
    REQUIRE(cc_o.m_steps.back().m_lowdin_pop_per_atom.back().second == Approx(0.033850));
    REQUIRE(*(cc_o.m_steps.back().m_dipole_moment) == v3d(v3d::Zero()));

  }

  SECTION ("Parsing results of geometry optimization[PC Gamess Firefly]") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_gopt_a.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    REQUIRE(cc_o.m_tot_nelec == 70) ;
    REQUIRE(cc_o.m_tot_charge == Approx(0)) ;
    REQUIRE(cc_o.m_DIM == 0);
    REQUIRE(cc_o.m_mult == 1);
    REQUIRE(cc_o.m_n_alpha == 35);
    REQUIRE(cc_o.m_n_beta == 35);
    REQUIRE(cc_o.m_tot_nat == 20);
    REQUIRE(cc_o.m_run_t == comp_chem_program_run_t::rt_geo_opt);
    REQUIRE(cc_o.m_init_atoms_pos.size() == 20);
    REQUIRE(cc_o.m_init_atoms_names.size() == 20);
    REQUIRE(cc_o.m_steps.size() == 24);
    REQUIRE(cc_o.m_is_terminated_normally == true);
    REQUIRE(cc_o.m_steps.front().m_atoms_pos.size() == cc_o.m_init_atoms_pos.size());
    REQUIRE(cc_o.m_steps.back().m_atoms_pos.size() == cc_o.m_init_atoms_pos.size());
    REQUIRE(cc_o.m_steps.back().m_atoms_pos[0] == v3d(0.2292900177, 1.3927230046, 0.0));
    REQUIRE(cc_o.m_steps.back().m_atoms_pos.back() == v3d(-0.0328883188, 4.8736185925, 0.0));
    REQUIRE(cc_o.m_steps.back().m_atoms_grads.front() == v3d(-0.0000150, -0.0000011, 0.0000000));
    REQUIRE(cc_o.m_steps.back().m_atoms_grads.back() == v3d(-0.0000191, 0.0000149, 0.0000000));

  }

  SECTION ("Parsing results of hessian calculation[PC Gamess Firefly]") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_ir.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    REQUIRE(cc_o.m_tot_nelec == 70) ;
    REQUIRE(cc_o.m_tot_charge == Approx(0)) ;
    REQUIRE(cc_o.m_DIM == 0 );
    REQUIRE(cc_o.m_mult == 1 );
    REQUIRE(cc_o.m_n_alpha == 35 );
    REQUIRE(cc_o.m_n_beta == 35 );
    REQUIRE(cc_o.m_tot_nat == 20);
    REQUIRE(cc_o.m_run_t == comp_chem_program_run_t::rt_vib);
    REQUIRE(cc_o.m_init_atoms_pos.size() == 20);
    REQUIRE(cc_o.m_init_atoms_names.size() == 20);
    REQUIRE(cc_o.m_vibs.size() == 60 );
    REQUIRE(cc_o.m_vibs[0].m_frequency == Approx(5.89) );
    REQUIRE(cc_o.m_vibs[0].m_intensity == Approx(0.00000) );
    REQUIRE(cc_o.m_vibs[6].m_frequency == Approx(38.65) );
    REQUIRE(cc_o.m_vibs[6].m_intensity == Approx(0.00004) );
    REQUIRE(cc_o.m_vibs[15].m_frequency == Approx(528.44) );
    REQUIRE(cc_o.m_vibs[15].m_intensity == Approx(0.05468) );
    REQUIRE(cc_o.m_vibs[59].m_frequency == Approx(3816.56) );
    REQUIRE(cc_o.m_vibs[59].m_intensity == Approx(0.00000) );
    REQUIRE(cc_o.m_vibs[25].m_disp[0] == v3d(0, 0, 0.00328228));
    REQUIRE(cc_o.m_vibs[25].m_disp[1] == v3d(0, 0, -0.00328228));
    REQUIRE(cc_o.m_vibs[25].m_disp[18] == v3d(0, 0, -0.39977811));
    REQUIRE(cc_o.m_vibs[25].m_disp[19] == v3d(0, 0, 0.39977811));

  }

  SECTION ("Parsing results of raman calculation[PC Gamess Firefly]") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_raman.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    REQUIRE(cc_o.m_tot_nelec == 70) ;
    REQUIRE(cc_o.m_tot_charge == Approx(0)) ;
    REQUIRE(cc_o.m_DIM == 0 );
    REQUIRE(cc_o.m_mult == 1 );
    REQUIRE(cc_o.m_n_alpha == 35 );
    REQUIRE(cc_o.m_n_beta == 35 );
    REQUIRE(cc_o.m_tot_nat == 20);
    REQUIRE(cc_o.m_run_t == comp_chem_program_run_t::rt_raman);
    REQUIRE(cc_o.m_init_atoms_pos.size() == 20);
    REQUIRE(cc_o.m_init_atoms_names.size() == 20);

  }

}

TEST_CASE("Compilation of ccd model" ) {

  SECTION("Compile 0d models") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_sp.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    geometry<double, periodic_cell<double> > g(0);
    bool succes = compile_geometry(cc_o, g);
    REQUIRE(succes == true);
    REQUIRE(g.nat() == cc_o.m_tot_nat);
    bool all_atoms_the_same = true;
    for (size_t i = 0; i < g.nat(); i++)
      if (g.pos(i) != cc_o.m_init_atoms_pos[i]) all_atoms_the_same = false;
    REQUIRE(all_atoms_the_same);

  }

  SECTION("Compile 0d models - animation[geo_opt]") {

    std::ifstream isec("../examples/io/ref_data/firefly/dvb_gopt_a.out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_firefly_output(isec, cc_o);
    geometry<double, periodic_cell<double> > g(0);
    std::vector<geom_anim_record_t<double> > anim_rec;
    bool succes = compile_geometry(cc_o, g);
    bool succes_anims = compile_animation(cc_o, anim_rec);
    REQUIRE(succes);
    REQUIRE(succes_anims);
    REQUIRE(anim_rec[0].frame_data.size() == cc_o.m_steps.size());

    std::vector<geom_anim_record_t<double> > anim_rec_ws;
    bool succes_static = compile_static_animation(cc_o, anim_rec_ws);
    bool succes_static_with_steps = compile_animation(cc_o, anim_rec_ws);
    REQUIRE(succes_static);
    REQUIRE(succes_static_with_steps);
    REQUIRE(anim_rec_ws.size() == 2);
    REQUIRE(anim_rec_ws[0].m_anim_type == geom_anim_type::anim_static);

    std::ifstream isecv("../examples/io/ref_data/firefly/dvb_ir.out");
    comp_chem_program_data_t<double> cc_ov;
    read_ccd_from_firefly_output(isecv, cc_ov);
    std::vector<geom_anim_record_t<double> > anim_recv;
    bool succes_static_v = compile_static_animation(cc_ov, anim_recv);
    bool succes_static_with_v = compile_animation(cc_ov, anim_recv);
    REQUIRE(succes_static_v);
    REQUIRE(succes_static_with_v);
    REQUIRE(anim_recv.size() == (cc_ov.m_vibs.size()+1));

  }

}

TEST_CASE("Testing parsing xyz files with ccd approach") {

  SECTION("Parsing single geometry - 0d") {
    std::ifstream isec("../examples/io/ref_data/xyz/zoloft.xyz");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_xyz_file(isec, cc_o);
    REQUIRE(cc_o.m_tot_nat == 37);
  }

  SECTION("Parsing multiframe geometry - 0d") {
    std::ifstream isec("../examples/io/ref_data/xyz/23elim.xyz");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_xyz_file(isec, cc_o);
    REQUIRE(cc_o.m_tot_nat == 22);
    REQUIRE(cc_o.m_steps.size() == 22);
  }

  SECTION("Parsing single geometry with big amount of atoms - 0d") {
    std::ifstream isec("../examples/io/ref_data/xyz/relax.xyz");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_xyz_file(isec, cc_o);
    REQUIRE(cc_o.m_tot_nat == 49651);
  }

}

TEST_CASE( "Computational chemistry data parsing : CP2K Output" ) {

  SECTION ("Parsing results of geometry optimization [CP2K]") {
    std::ifstream isec("../examples/io/ref_data/cp2k/baf2_gopt.cp2k_out");
    comp_chem_program_data_t<double> cc_o;
    read_ccd_from_cp2k_output(isec, cc_o);
    geometry<double, periodic_cell<double> > g(3);
    std::vector<geom_anim_record_t<double> > anim_rec;
    bool succes = compile_geometry(cc_o, g);
    bool succes_anims = compile_animation(cc_o, anim_rec);

    REQUIRE(cc_o.m_run_t == comp_chem_program_run_t::rt_geo_opt);
    REQUIRE(cc_o.m_DIM == 3);
    REQUIRE(cc_o.m_cell_v.size() == 3);
    REQUIRE(cc_o.m_cell_v[0] == v3d(12.566, 0.000,  0.000 ));
    REQUIRE(cc_o.m_cell_v[1] == v3d(0.000,  12.566, 0.000 ));
    REQUIRE(cc_o.m_cell_v[2] == v3d(0.000,  0.000,  12.566 ));

    REQUIRE(cc_o.m_tot_nat == 96);
    REQUIRE(cc_o.m_init_atoms_names.size() == 96);
    REQUIRE(cc_o.m_init_atoms_pos.size() == 96);
    REQUIRE(std::count(cc_o.m_init_atoms_names.begin(), cc_o.m_init_atoms_names.end(), "Ba") == 32);
    REQUIRE(std::count(cc_o.m_init_atoms_names.begin(), cc_o.m_init_atoms_names.end(), "F") == 64);
    REQUIRE(cc_o.m_n_spin_states == 1);
    REQUIRE(cc_o.m_mult == 1);
    REQUIRE(cc_o.m_tot_charge == Approx(0));
    REQUIRE(cc_o.m_steps.size() == 13);

    REQUIRE(succes);
    REQUIRE(succes_anims);
    REQUIRE(anim_rec.front().frame_data.size() == 5 );

    bool succes_ccd_compilation = compile_ccd(cc_o, ccd_cf_default_flags |
                                              ccd_cf_remove_empty_geom_steps);
    REQUIRE(succes_ccd_compilation);
    REQUIRE(cc_o.m_steps.size() == 5);
  }

}