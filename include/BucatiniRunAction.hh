#ifndef BucatiniRunAction_h
#define BucatiniRunAction_h 1

#include "G4UserRunAction.hh"

class HistoManager;
class G4Run;

class BucatiniRunAction : public G4UserRunAction {

public:
  BucatiniRunAction(HistoManager*);
  virtual ~BucatiniRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  HistoManager* fHistoManager;
};

#endif
