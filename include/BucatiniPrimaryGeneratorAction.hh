//**************************************************
// \file BucatiniPrimaryGeneratorAction.hh
// \brief: Definition of BucatiniPrimaryGeneratorAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniPrimaryGeneratorAction_h
#define BucatiniPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;

class BucatiniPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

public:
  BucatiniPrimaryGeneratorAction();
  virtual ~BucatiniPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *event);

private:
  G4GeneralParticleSource *fGeneralParticleSource;
};

#endif

//**************************************************
