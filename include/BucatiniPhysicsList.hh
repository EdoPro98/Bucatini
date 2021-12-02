//**************************************************
// \file BucatiniPhysicsList.hh
// \brief: Definition of BucatiniPhysicsList class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniPhysicsList_h
#define BucatiniPhysicsList_h 1

#include "G4VModularPhysicsList.hh"

#include "BucatiniOpticalPhysics.hh"
#include "G4FastSimulationPhysics.hh"

class BucatiniPhysicsList : public G4VModularPhysicsList {

public:
  BucatiniPhysicsList(G4String);
  virtual ~BucatiniPhysicsList();

  BucatiniOpticalPhysics* OpPhysics;
  G4FastSimulationPhysics* FastSimPhysics;
};

#endif

//**************************************************
