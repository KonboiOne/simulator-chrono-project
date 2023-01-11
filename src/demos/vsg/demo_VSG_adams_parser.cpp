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
// Authors: Conlain Kelly
// =============================================================================
//
// Demo for the ADAMS -> Chrono parser
//
// =============================================================================

#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/utils/ChParserAdams.h"
#include "chrono/core/ChRealtimeStep.h"

#include "chrono_vsg/ChVisualSystemVSG.h"

#include <cassert>
#include <cmath>
#include <functional>

using namespace chrono;
using namespace chrono::utils;
using namespace chrono::vsg3d;

int main(int argc, char* argv[]) {
    // Get ADAMS input file (relative to the 'data' directory)
    std::string filename;
    if (argc > 1) {
        filename = std::string(argv[1]);
    } else {
        filename = "testing/joints/adams_models/test_Revolute_Case01.adm";
    }
    filename = GetChronoDataFile(filename);

    // Make a system
    ChSystemSMC sys;

    // Create parser instance and set options.
    // Use LOADED to read the ADAMS primitives
    ChParserAdams parser;
    parser.SetVisualizationType(ChParserAdams::VisType::LOADED);
    parser.SetVerbose(true);
    parser.Parse(sys, filename);

    // Get a full report on parsed elements
    auto rep = parser.GetReport();
    std::cout << "---------" << std::endl;
    rep.Print();
    std::cout << "---------" << std::endl;

    // Adda a ground for perspective (no collision)
    auto my_ground = chrono_types::make_shared<ChBodyEasyBox>(40, 2, 40, 1000, true, false);
    sys.AddBody(my_ground);
    my_ground->SetBodyFixed(true);
    my_ground->SetPos(ChVector<>(0, -2.9, 0));
    my_ground->SetNameString(std::string("ground"));
    my_ground->GetVisualShape(0)->SetTexture(GetChronoDataFile("textures/concrete.jpg"));

    // Create the Irrlicht visualization system
    auto vis = chrono_types::make_shared<ChVisualSystemVSG>();
    vis->AttachSystem(&sys);
    vis->SetWindowTitle("Model loaded from ADAMS file");
    vis->AddCamera(ChVector<>(0, 0, 2));
    vis->SetCameraVertical(CameraVerticalDir::Y);
    vis->SetWindowSize(ChVector2<int>(800, 600));
    vis->SetWindowPosition(ChVector2<int>(100, 300));
    vis->SetUseSkyBox(true);
    vis->SetCameraAngleDeg(40);
    vis->SetLightIntensity(1.0);
    vis->SetLightDirection(1.5*CH_C_PI_2, CH_C_PI_4);
    vis->Initialize();

    // Simulation loop
    double timestep = 0.005;
    ChRealtimeStepTimer realtime_timer;
    while (vis->Run()) {
        vis->Render();
        sys.DoStepDynamics(timestep);
        realtime_timer.Spin(timestep);
    }

    return 0;
}
