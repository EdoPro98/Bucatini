#ifndef BucatiniActionInitialization_h
#define BucatiniActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class BucatiniActionInitialization : public G4VUserActionInitialization {

public:
  BucatiniActionInitialization();
  virtual ~BucatiniActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

#endif

//**************************************************
