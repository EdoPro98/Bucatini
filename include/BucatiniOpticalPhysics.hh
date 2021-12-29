//**************************************************
// \file BucatiniOpticalPhysics.hh
// \brief: Definition of BucatiniOpticalPhysics class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************//

#ifndef BucatiniOpticalPhysics_h
#define BucatiniOpticalPhysics_h 1

#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpMieHG.hh"
#include "G4OpRayleigh.hh"
#include "G4OpWLS.hh"
#include "G4Scintillation.hh"
#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class BucatiniOpticalPhysics : public G4VPhysicsConstructor {

public:
  BucatiniOpticalPhysics();
  virtual ~BucatiniOpticalPhysics();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

  G4OpWLS* GetWLSProcess() { return theWLSProcess; }
  G4Cerenkov* GetCerenkovProcess() { return theCerenkovProcess; }
  G4Scintillation* GetScintillationProcess() { return theScintProcess; }
  G4OpAbsorption* GetAbsorptionProcess() { return theAbsorptionProcess; }
  G4OpRayleigh* GetRayleighScatteringProcess() { return theRayleighScattering; }
  G4OpMieHG* GetMieHGScatteringProcess() { return theMieHGScatteringProcess; }
  G4OpBoundaryProcess* GetBoundaryProcess() { return theBoundaryProcess; }

private:
  G4OpWLS* theWLSProcess;
  G4Cerenkov* theCerenkovProcess;
  G4Scintillation* theScintProcess;
  G4OpAbsorption* theAbsorptionProcess;
  G4OpRayleigh* theRayleighScattering;
  G4OpMieHG* theMieHGScatteringProcess;
  G4OpBoundaryProcess* theBoundaryProcess;
};

#endif
