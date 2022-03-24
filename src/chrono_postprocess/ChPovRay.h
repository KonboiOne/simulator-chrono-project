// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora, Radu Serban
// =============================================================================

#ifndef CHPOVRAY_H
#define CHPOVRAY_H

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "chrono/assets/ChVisualShape.h"
#include "chrono/physics/ChSystem.h"
#include "chrono_postprocess/ChPostProcessBase.h"

namespace chrono {
namespace postprocess {

/// Class for post processing implementation that generates scripts for POVray.
/// The script can be used in POVray to render photo-realistic animations.
class ChApiPostProcess ChPovRay : public ChPostProcessBase {
  public:
    ChPovRay(ChSystem* system);
    ~ChPovRay() {}

    /// Modes for displaying contacts.
    enum class ContactSymbol {
        VECTOR_SCALELENGTH = 0,
        VECTOR_SCALERADIUS,
        VECTOR_NOSCALE,
        SPHERE_SCALERADIUS,
        SPHERE_NOSCALE
    };

    /// Add a ChPhysicsItem object to the list of objects to render.
    /// An item is added to the list only if it has a visual model.
    void Add(std::shared_ptr<ChPhysicsItem> item);

    /// Remove a ChPhysicsItem object from the list of objects to render.
    void Remove(std::shared_ptr<ChPhysicsItem> item);

    /// Add all ChPhysicsItem objects in the system to the list of objects to render.
    void AddAll();

    /// Remove all ChPhysicsItem objects that were previously added.
    void RemoveAll();

    /// Set the path where all files (.ini, .pov, .assets etc) will be saved.
    /// The path can be absolute, or relative to the .exe current path.
    /// Note that the directory must be already existing.
    /// At the execution of ExportScript() it will create files & directiories like
    ///    base_path
    ///       render_frames.pov.ini
    ///       render_frames.pov
    ///       render_frames.pov.assets
    ///       anim
    ///          picture0000.bmp
    ///          ....
    ///       output
    ///          state0000.pov
    ///          state0000.dat
    ///          ....
    void SetBasePath(const std::string& mpath) { base_path = mpath; }

    /// Set the filename of the template for the script generation.
    /// If not set, it defaults to "_template_POV.pov" in the default Chrono data directory.
    void SetTemplateFile(const std::string& filename) { template_filename = filename; }

    /// Set the filename of the output POV-Ray script.
    /// If not set, it defaults to "render_frames.pov".
    void SetOutputScriptFile(const std::string& filename) { out_script_filename = filename; }

    /// Set the filename of the .bmp files generated by POV.
    /// It should NOT contain the .bmp suffix because POV will append the frame number (es. pic0001.bmp, pic0002.bmp,
    /// ...). If not set, it defaults to "pic".
    void SetPictureFilebase(const std::string& filename) { pic_filename = filename; }

    /// Set the filename of the output data generated
    /// by the function ExportData(), es: "state"; when the user will
    /// execute POV and run the .ini file generated by ExportScript(),
    /// the .ini script will call the .pov script that will load these data files.
    /// It should NOT contain the .dat suffix because ExportData()
    /// will append the frame number (es. state0001.dat, state0002.dat, ...).
    /// If not set, it defaults to "state".
    void SetOutputDataFilebase(const std::string& filename) { out_data_filename = filename; }

    /// Set the picture width and height - will write this in the output .ini file.
    void SetPictureSize(unsigned int width, unsigned int height) {
        picture_width = width;
        picture_height = height;
    };

    /// Set antialiasing - will write this in the output .ini file.
    void SetAntialiasing(bool active, unsigned int depth, double treshold) {
        antialias = active;
        antialias_depth = depth;
        antialias_treshold = treshold;
    };

    /// Set the default camera position and aim point - will write this in the output .pov file.
    void SetCamera(ChVector<> location, ChVector<> aim, double angle, bool ortho = false);

    /// Set the default light position and color - will write this in the output .pov file.
    void SetLight(ChVector<> location, ChColor color, bool cast_shadow);

    /// Set the background color - will write this in the output .pov file.
    void SetBackground(ChColor color) { background = color; }

    /// Set the ambient light - will write this in the output .pov file.
    void SetAmbientLight(ChColor color) { ambient_light = color; }

    /// Turn on/off the display of the COG (center of mass) of rigid bodies.
    /// If setting true, you can also set the size of the symbol, in meters.
    void SetShowCOGs(bool show, double msize = 0.04);

    /// Turn on/off the display of the reference coordsystems of rigid bodies.
    /// If setting true, you can also set the size of the symbol, in meters.
    void SetShowFrames(bool show, double msize = 0.05);

    /// Turn on/off the display of the reference coordsystems for ChLinkMate constraints.
    /// If setting true, you can also set the size of the symbol, in meters.
    void SetShowLinks(bool show, double msize = 0.04);

    /// Turn on/off the display of contacts, using spheres or arrows (see eChContactSymbol modes).
    /// The size of the arrow or of the sphere depends on force strength multiplied by 'scale'.
    /// Use 'max_size' to limit size of arrows if too long, or spheres if too large (they will be signaled by white
    /// color)
    /// Use 'width' for the radius of the arrow. If in 'SYMBOL_VECTOR_SCALERADIUS' mode, the length of the vector is
    /// always max_size.
    void SetShowContacts(bool show,
                         ContactSymbol mode,
                         double scale,
                         double width,
                         double max_size,
                         bool do_colormap,
                         double colormap_start,
                         double colormap_end);

    /// Set thickness for wireframe mode of meshes.
    /// If a ChTriangleMeshShape asset was set as SetWireframe(true), it will be rendered in POVray as a cage of thin
    /// cylinders. This setting sets how thick the tubes.
    void SetWireframeThickness(const double mt) { this->wireframe_thickness = mt; }
    double GetWireframeThickness() { return this->wireframe_thickness; }

    /// Set a string (a text block) of custom POV commands that you can optionally
    /// append to the POV script file, for example adding other POV lights, materials, etc.
    /// What you put in this string will be put at the end of the generated POV script, just
    /// before loading the data.
    void SetCustomPOVcommandsScript(const std::string& mtext) { this->custom_script = mtext; }
    const std::string& GetCustomPOVcommandsScript() { return this->custom_script; }

    /// Set a string (a text block) of custom POV commands that you can optionally
    /// append to the POV script files that are load at each timestep,
    /// es. state0001.pov, state0002.pov, for example adding other POV lights, materials, etc.
    void SetCustomPOVcommandsData(const std::string& mtext) { this->custom_data = mtext; }
    const std::string& GetCustomPOVcommandsData() { return this->custom_data; }

    /// When ExportData() is called, it saves .dat files in incremental
    /// way, starting from zero: data0000.dat, data0001.dat etc., but you can
    /// override the formatted number by calling SetFramenumber(), before.
    void SetFramenumber(unsigned int mn) { this->framenumber = mn; }

    /// Export the script that will be used by POV to process all the exported data and render the complete animation.
    /// It contains the definition of geometric shapes, lights, etc. and a POV-Ray function that moves meshes in the
    /// position specified by data files saved at each step.
    void ExportScript() { this->ExportScript(this->out_script_filename); }

    /// As ExportScript(), but overrides the filename.
    virtual void ExportScript(const std::string& filename) override;

    /// This function is used at each timestep to export data
    /// formatted in a way that it can be load with the POV
    /// scripts generated by ExportScript().
    /// The generated filename must be set at the beginning of
    /// the animation via SetOutputDataFilebase(), and then a
    /// number is automatically appended and incremented at each
    /// ExportData(), ex.
    ///  state0001.dat, state0002.dat,
    /// The user should call this function in the while() loop
    /// of the simulation, once per frame.
    void ExportData();

    /// As ExportData(), but overrides the automatically generated filename.
    virtual void ExportData(const std::string& filename) override;

    /// Set if the assets for the entre scenes at all timesteps must be appended into one
    /// single large file "rendering_frames.pov.assets". If not, assets will be written inside
    /// each state0001.dat, state0002.dat, etc files; this would waste more disk space but would be
    /// a bit faster in POV parsing and would allow assets whose settings change during time (ex time-changing colors)
    void SetUseSingleAssetFile(bool muse) { this->single_asset_file = muse; }

  private:
    void UpdateRenderList();
    void ExportAssets(ChStreamOutAsciiFile& assets_file);
    void ExportShapes(const std::vector<ChVisualModel::ShapeInstance>& shapes, ChStreamOutAsciiFile& assets_file);
    void ExportMaterials(const std::vector<std::shared_ptr<ChVisualMaterial>>& materials,
                         ChStreamOutAsciiFile& assets_file);
    void ExportObjData(const std::vector<ChVisualModel::ShapeInstance>& shapes,
                       const ChFrame<>& parentframe,
                       ChStreamOutAsciiFile& pov_file);

    std::unordered_set<std::shared_ptr<ChPhysicsItem>> m_items;

    std::unordered_map<size_t, std::shared_ptr<ChVisualShape>> m_pov_shapes;
    std::unordered_map<size_t, std::shared_ptr<ChVisualMaterial>> m_pov_materials;

    std::string base_path;
    std::string pic_path;
    std::string out_path;
    std::string template_filename;
    std::string pic_filename;

    std::string out_script_filename;
    std::string out_data_filename;

    unsigned int framenumber;

    ChVector<> camera_location;
    ChVector<> camera_aim;
    ChVector<> camera_up;
    double camera_angle;
    bool camera_orthographic;
    bool camera_found_in_assets;

    ChVector<> def_light_location;
    ChColor def_light_color;
    bool def_light_cast_shadows;

    bool COGs_show;
    double COGs_size;
    bool frames_show;
    double frames_size;
    bool links_show;
    double links_size;
    bool contacts_show;
    double contacts_maxsize;
    double contacts_scale;
    ContactSymbol contacts_scale_mode;
    double contacts_width;
    double contacts_colormap_startscale;
    double contacts_colormap_endscale;
    bool contacts_do_colormap;
    double wireframe_thickness;
    ChColor background;
    ChColor ambient_light;

    bool antialias;
    int antialias_depth;
    double antialias_treshold;

    unsigned int picture_width;
    unsigned int picture_height;

    std::string custom_script;
    std::string custom_data;

    bool single_asset_file;
};

}  // end namespace postprocess
}  // end namespace chrono

#endif
