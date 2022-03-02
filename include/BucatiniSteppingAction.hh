#ifndef BucatiniSteppingAction_h
#define BucatiniSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class BucatiniDetectorConstruction;
class BucatiniEventAction;

class BucatiniSteppingAction : public G4UserSteppingAction {

public:
  BucatiniSteppingAction(BucatiniEventAction* eventAction);
  ~BucatiniSteppingAction();

  void UserSteppingAction(const G4Step* step);

  void globalSteppingAction(const G4Step* step);
  void opticalSteppingAction(const G4Step* step);

private:
  BucatiniEventAction* fEventAction;
};

#endif

//**************************************************
