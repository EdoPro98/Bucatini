//**************************************************
// \file BucatiniOpticalPhysics.cc
// \brief: Implementation of BucatiniOpticalPhysics class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "G4EmSaturation.hh"
#include "G4LossTableManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"

#include "BucatiniOpticalPhysics.hh"

BucatiniOpticalPhysics::BucatiniOpticalPhysics()
    : G4VPhysicsConstructor("Optical") {

  theWLSProcess = nullptr;
  theScintProcess = nullptr;
  theCerenkovProcess = nullptr;
  theBoundaryProcess = nullptr;
  theAbsorptionProcess = nullptr;
  theRayleighScattering = nullptr;
  theMieHGScatteringProcess = nullptr;
}

BucatiniOpticalPhysics::~BucatiniOpticalPhysics() {}

void BucatiniOpticalPhysics::ConstructParticle() {
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BucatiniOpticalPhysics::ConstructProcess() {

  theScintProcess = new G4Scintillation();
  theScintProcess->SetTrackSecondariesFirst(true);
  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  theScintProcess->AddSaturation(emSaturation);

  theCerenkovProcess = new G4Cerenkov();
  theCerenkovProcess->SetMaxNumPhotonsPerStep(2000.);
  theCerenkovProcess->SetTrackSecondariesFirst(true);

  theAbsorptionProcess = new G4OpAbsorption();
  theBoundaryProcess = new G4OpBoundaryProcess();
  // theRayleighScattering = new G4OpRayleigh();
  // theMieHGScatteringProcess = new G4OpMieHG();
  // theWLSProcess = new G4OpWLS();

  // Get optical photon process manager
  G4ProcessManager* pManager =
      G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  // Add processes to optical photon with optical photon process manager
  pManager->AddDiscreteProcess(theAbsorptionProcess);
  pManager->AddDiscreteProcess(theBoundaryProcess);
  // pManager->AddDiscreteProcess(theRayleighScattering);
  // pManager->AddDiscreteProcess(theMieHGScatteringProcess);
  // pManager->AddDiscreteProcess(theWLSProcess);

  // Loop on particles and apply scintillation and cherenkov processes to any
  // physical candidate
  auto theParticleIterator = GetParticleIterator();
  theParticleIterator->reset();
  while ((*theParticleIterator)()) {

    // Get particle
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4String particleName = particle->GetParticleName();

    // Get particle process manager
    pManager = particle->GetProcessManager();

    // Add Cherenkov process to each candidate
    if (theCerenkovProcess->IsApplicable(*particle)) {
      pManager->AddProcess(theCerenkovProcess);
      pManager->SetProcessOrdering(theCerenkovProcess, idxPostStep);
    }

    // Add Scintillation process to each candidate
    if (theScintProcess->IsApplicable(*particle)) {
      pManager->AddProcess(theScintProcess);
      pManager->SetProcessOrderingToLast(theScintProcess, idxAtRest);
      pManager->SetProcessOrderingToLast(theScintProcess, idxPostStep);
    }
  }
}
