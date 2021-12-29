//**************************************************
// \file BucatiniPhysicsList.cc
// \brief: Implementation of ATLTBHECDetectorConstruction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "BucatiniPhysicsList.hh"
#include "BucatiniOpticalPhysics.hh"

#include "G4FastSimulationPhysics.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4PhysListFactory.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

BucatiniPhysicsList::BucatiniPhysicsList(G4String physName)
    : G4VModularPhysicsList() {

  // Define physics list factor and modular physics list
  G4PhysListFactory factory;
  G4VModularPhysicsList* phys = nullptr;

  // Check is physName corresponds to a real physics list run an exception if
  // not the case
  if (factory.IsReferencePhysList(physName)) {
    phys = factory.GetReferencePhysList(physName);
  } else {
    G4Exception("PhysicsList::PhysicsList", "InvalidSetup", FatalException,
                "PhysicsList does not exist");
  }

  // Register physics from physName
  for (G4int i = 0;; ++i) {
    G4VPhysicsConstructor* elem =
        const_cast<G4VPhysicsConstructor*>(phys->GetPhysics(i));

    if (elem == nullptr) {
      break;
    }

    RegisterPhysics(elem);
  }

  RegisterPhysics(OpPhysics = new BucatiniOpticalPhysics());
  FastSimPhysics = new G4FastSimulationPhysics();
  FastSimPhysics->ActivateFastSimulation("opticalphoton");
  RegisterPhysics(FastSimPhysics);
}

BucatiniPhysicsList::~BucatiniPhysicsList() {}
