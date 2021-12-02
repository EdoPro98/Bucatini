//**************************************************
// \file BucatiniSteppingAction.hh
// \brief: Definition of BucatiniSteppingAction.hh
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniSteppingAction_h
#define BucatiniSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class BucatiniDetectorConstruction;
class BucatiniEventAction;

class BucatiniSteppingAction : public G4UserSteppingAction {

public:
  BucatiniSteppingAction(BucatiniEventAction *eventAction);
  virtual ~BucatiniSteppingAction();

  virtual void UserSteppingAction(const G4Step *step);

  void globalSteppingAction(const G4Step *step);
  void opticalSteppingAction(const G4Step *step);

private:
  BucatiniEventAction *fEventAction;
  bool fIsFirstStep = true;
};

#endif

//**************************************************
