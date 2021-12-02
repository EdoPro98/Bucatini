//**************************************************
// \file BucatiniSteppingAction.cc
// \brief: Implementation of BucatiniSteppingAction.cc
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "BucatiniSteppingAction.hh"
#include "BucatiniDetectorConstruction.hh"
#include "BucatiniEventAction.hh"

#include "G4Electron.hh"
#include "G4Material.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4Positron.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"

#include "Randomize.hh"

BucatiniSteppingAction::BucatiniSteppingAction(BucatiniEventAction *eventAction)
    : G4UserSteppingAction(), fEventAction(eventAction) {}

BucatiniSteppingAction::~BucatiniSteppingAction() {}

void BucatiniSteppingAction::UserSteppingAction(const G4Step *step) {

  const G4ParticleDefinition *particleDef = step->GetTrack()->GetDefinition();
  const G4VPhysicalVolume *preStepVolume = step->GetTrack()->GetVolume();

  // Kill Photons going outside calo and return
  if (particleDef == G4OpticalPhoton::Definition() &&
      preStepVolume->GetName() == "world") {
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }
  if (particleDef == G4OpticalPhoton::Definition() &&
      preStepVolume->GetName() == "shield") {
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  // Save global information (edep in calo / edep in leak)
  globalSteppingAction(step);

  // Manage optical photons
  // opticalSteppingAction(step);
}

void BucatiniSteppingAction::opticalSteppingAction(const G4Step *step) {}

//
void BucatiniSteppingAction::globalSteppingAction(const G4Step *step) {
  // Get step info
  const G4VPhysicalVolume *preStepVolume = step->GetTrack()->GetVolume();
  const double energyDeposited = step->GetTotalEnergyDeposit();
  const G4ParticleDefinition *particleDef = step->GetTrack()->GetDefinition();
  const int particlePDG = step->GetTrack()->GetDefinition()->GetPDGEncoding();
  const std::string volumeName = preStepVolume->GetName();

  // energy escaped
  if (volumeName == "leakageAbsorber") {
    fEventAction->addEnergyLeaked(step->GetTrack()->GetKineticEnergy());
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  // energy deposited in calo
  if (volumeName != "world" && volumeName != "leakageAbsorber") {
    fEventAction->addEnergyDeposited(energyDeposited);

    // em specific energy
    if (particleDef == G4Electron::Definition() ||
        particleDef == G4Positron::Definition()) {
      fEventAction->addEnergyEM(energyDeposited);
    }

    // S and C fiber energy deposit
    if (volumeName.find("fiberCoreS") !=
        std::string::npos) { // scintillating fiber
      fEventAction->addEnergyScinFibers(energyDeposited);
    }

    if (volumeName.find("fiberCoreC") != std::string::npos) { // Cherenkov fiber
      fEventAction->addEnergyCherFibers(energyDeposited);
    }
  }
}
