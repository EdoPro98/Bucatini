#include "BucatiniSteppingAction.hh"
#include "BucatiniEventAction.hh"

#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4Positron.hh"
#include "G4Step.hh"

#include "Randomize.hh"

BucatiniSteppingAction::BucatiniSteppingAction(BucatiniEventAction* eventAction)
    : G4UserSteppingAction(), fEventAction(eventAction) {}

BucatiniSteppingAction::~BucatiniSteppingAction() {}

void BucatiniSteppingAction::UserSteppingAction(const G4Step* step) {

  const G4ParticleDefinition* particleDef = step->GetTrack()->GetDefinition();
  const G4VPhysicalVolume* preStepVolume = step->GetTrack()->GetVolume();
  

  // Kill Photons going outside calo and return
  if (preStepVolume->GetName() == "world" && particleDef == G4OpticalPhoton::Definition()) {
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }
  if (preStepVolume->GetName() == "shield" && particleDef == G4OpticalPhoton::Definition()) {
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  // Save global information (edep in calo / edep in leak)
  globalSteppingAction(step);

  // Manage optical photons
  // opticalSteppingAction(step);
}

void BucatiniSteppingAction::opticalSteppingAction(const G4Step* step) {}

//
void BucatiniSteppingAction::globalSteppingAction(const G4Step* step) {
  // Get step info
  const G4VPhysicalVolume* preStepVolume = step->GetTrack()->GetVolume();
  const double energyDeposited = step->GetTotalEnergyDeposit();
  const G4ParticleDefinition* particleDef = step->GetTrack()->GetDefinition();
  const std::string volumeName = preStepVolume->GetName();

  if (step->GetTrack()->GetTrackID() == 1 && step->GetTrack()->GetCurrentStepNumber() == 1) {
    fEventAction->savePrimaryEnergy(step->GetTrack()->GetKineticEnergy());
  }

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
    if (particleDef == G4Electron::Definition() || particleDef == G4Positron::Definition()) {
      fEventAction->addEnergyEM(energyDeposited);
    }

    // S and C fiber energy deposit
    if (volumeName.find("fiberCoreS") != std::string::npos) { // S fiber
      fEventAction->addEnergyScinFibers(energyDeposited);
    }

    if (volumeName.find("fiberCoreC") != std::string::npos) { // C fiber
      fEventAction->addEnergyCherFibers(energyDeposited);
    }
  }
}
