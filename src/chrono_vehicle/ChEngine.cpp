// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2023 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban
// =============================================================================
//
// Base class for a vehicle engine.
//
// =============================================================================

#include "chrono_vehicle/ChEngine.h"

namespace chrono {
namespace vehicle {

ChEngine::ChEngine(const std::string& name)
    : ChPart(name){
    m_motorshaft = chrono_types::make_shared<ChShaft>();
}

ChEngine::~ChEngine() {
    if (m_motorshaft->GetSystem()) {
        m_motorshaft->GetSystem()->RemoveShaft(m_motorshaft);
    }
}

void ChEngine::Initialize(std::shared_ptr<ChChassis> chassis) {
    m_motorshaft->SetInertia(1.0);
    chassis->GetSystem()->AddShaft(m_motorshaft);
}

void ChEngine::InitializeInertiaProperties() {
    m_mass = 0;
    m_inertia = ChMatrix33<>(0);
    m_com = ChFrame<>();
    m_xform = ChFrame<>();
}

void ChEngine::UpdateInertiaProperties() {}

}  // end namespace vehicle
}  // end namespace chrono
