#ifndef QPP_WS_ATOM_LIST_H
#define QPP_WS_ATOM_LIST_H


#include <geom/lace3d.hpp>
#include <geom/geom.hpp>
#include <geom/xgeom.hpp>
#include <geom/ngbr.hpp>
#include <geom/extents_observer.hpp>
#include <geom/tws_tree.hpp>
#include <data/color.hpp>
#include <qppcad/ws_item.hpp>
#include <qppcad/camera.hpp>
#include <qppcad/file_formats.hpp>
#include <functional>
#include <vector>
#include <iostream>
#include <algorithm>

namespace qpp{

  class ws_atom_list : public ws_item {
    public:
      int iDim;
      bool bNeedToRebuildNBT;
      xgeometry<float, periodic_cell<float> > *geom;
      bonding_table<float> *bt;
      neighbours_table<float> *nt;
      tws_tree<float, periodic_cell<float> > *tws_tr;
      extents_observer<float, periodic_cell<float> > *ext_obs;

      ws_atom_list(workspace* parent);

      void vote_for_view_vectors(vector3<float> &vOutLookPos,
                                 vector3<float> &vOutLookAt) override ;
      void update() override;
      void render() override;
      void render_ui() override;
      bool mouse_click(ray<float> *ray) override;

      bool support_translation() override;
      bool support_rotation() override;
      bool support_scaling() override;
      bool support_content_editing() override;
      bool support_selection() override;
      bool support_rendering_bounding_box() override;

      float get_bb_prescaller() override;

      void shift(const vector3<float> vShift);

      ///
      /// \brief load_from_file
      /// \param eFileFormat
      /// \param sFileName
      /// \param bAutoCenter
      ///
      void load_from_file(qc_file_format eFileFormat, std::string sFileName,
                          bool bAutoCenter = false);

      void rebuild_ngbt();

      ~ws_atom_list() override {

      }

  };

}

#endif
