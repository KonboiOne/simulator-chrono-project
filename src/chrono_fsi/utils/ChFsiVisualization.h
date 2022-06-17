// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2021 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban
// =============================================================================

#pragma once

#include <string>

#include "chrono/physics/ChSystem.h"
#include "chrono/physics/ChParticleCloud.h"

#include "chrono_fsi/ChApiFsi.h"
#include "chrono_fsi/ChSystemFsi.h"

namespace chrono {
namespace fsi {

/// @addtogroup fsi_utils
/// @{

/// Run-time visualization support for Chrono::FSI systems.
/// Requires the Chrono::OpenGL module; if not available, most ChFsiVisualization functions are no-op.
///
/// Note that using run-time visualization for a Chrono::FSI system incurs the penalty of collecting positions of all
/// particles every time the Render() function is invoked.
///
/// To implement a moving camera (i.e., prescribe the camera position), get the current instance of the active
/// Chrono::OpenGL window and use the function ChOpenGLWindow::SetCamera().
class CH_FSI_API ChFsiVisualization {
  public:
    /// <summary>
    /// Create a run-time visualization object associated with a given Chrono::Fsi system.
    ChFsiVisualization(ChSystemFsi* sysFSI);
    ~ChFsiVisualization();

    /// Set title of the visualization window (default: "").
    void SetTitle(const std::string& title) { m_title = title; }

    /// Set camera position and target (look at) point.
    /// Must be called before Initialize().
    void SetCameraPosition(const ChVector<>& pos, const ChVector<>& target);

    /// Set camera up vector (default: Z).
    /// Must be called before Initialize().
    void SetCameraUpVector(const ChVector<>& up);

    /// Set scale for camera movement increments (default: 0.1).
    /// Must be called before Initialize().
    void SetCameraMoveScale(float scale);

    /// Set visualization radius for SPH particles (default: half initial spacing).
    void SetVisualizationRadius(double radius);

    /// Enable/disable rendering of fluid SPH particles (default: true).
    void EnableFluidMarkers(bool val) { m_sph_markers = val; }

    /// Enable/disable rendering of rigid-body BCE markers (default: true).
    void EnableRigidBodyMarkers(bool val) { m_rigid_bce_markers = val; }

    /// Enable/disable rendering of boundary BCE markers (default: false).
    void EnableBoundaryMarkers(bool val) { m_bndry_bce_markers = val; }

    /// Add additional proxy body to supplemental system.
    /// Must be called before Initialize().
    /// The provided body is set fixed to ground and it is the caller's responsibility to update the position of this
    /// body before a call to Render().
    void AddProxyBody(std::shared_ptr<ChBody> body);

    /// Initialize the run-time visualization system.
    /// If the Chrono::OpenGL module is not available, this function is no-op.
    void Initialize();

    /// Render the current state of the Chrono::Fsi system. This function, typically invoked from within the main
    /// simulation loop, can only be called after construction of the FSI system was completed (i.e., the system was
    /// initialized). This function querries the positions of all particles in the FSI system in order to update the
    /// positions of the proxy bodies.
    /// If the Chrono::OpenGL module is not available, this function is no-op.
    bool Render();

  private:
    ChSystemFsi* m_systemFSI;  ///< associated Chrono::FSI system
    ChSystem* m_system;        ///< supplemental Chrono system (holds proxy bodies)

    double m_radius;           ///< particle visualization radius
    bool m_sph_markers;        ///< render fluid SPH particles?
    bool m_rigid_bce_markers;  ///< render rigid-body BCE markers?
    bool m_bndry_bce_markers;  ///< render boundary BCE markers?

    std::shared_ptr<ChParticleCloud> m_particles;  ///< particle cloud proxy for SPH markers
    unsigned int m_bce_start_index;                ///< start index of BCE proxy bodies in m_system's body list

    std::string m_title;      ///< visualization window title
    ChVector<> m_cam_pos;     ///< current camera position
    ChVector<> m_cam_target;  ///< current camera look at point
    ChVector<> m_cam_up;      ///< camera up vector
    float m_cam_scale;        ///< camera move increment scale
};

/// @} fsi_utils

}  // namespace fsi
}  // namespace chrono
