//**************************************************
// \file BucatiniActionInitialization.cc
// \brief: Implementation of BucatiniActionInitialization class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "BucatiniActionInitialization.hh"
#include "BucatiniEventAction.hh"
#include "BucatiniHistoManager.hh"
#include "BucatiniPrimaryGeneratorAction.hh"
#include "BucatiniRunAction.hh"
#include "BucatiniSteppingAction.hh"

BucatiniActionInitialization::BucatiniActionInitialization()
    : G4VUserActionInitialization() {}

BucatiniActionInitialization::~BucatiniActionInitialization() {}

void BucatiniActionInitialization::BuildForMaster() const {
  HistoManager* histo = new HistoManager();
  SetUserAction(new BucatiniRunAction(histo));
}

void BucatiniActionInitialization::Build() const {
  HistoManager* histo = new HistoManager();

  SetUserAction(new BucatiniPrimaryGeneratorAction);
  SetUserAction(new BucatiniRunAction(histo));
  BucatiniEventAction* eventAction = new BucatiniEventAction(histo);
  SetUserAction(eventAction);
  SetUserAction(new BucatiniSteppingAction(eventAction));
}
