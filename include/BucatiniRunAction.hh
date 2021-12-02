//**************************************************
// \file BucatiniRunAction.hh
// \brief: Definition of BucatiniBRunAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniRunAction_h
#define BucatiniRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

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
