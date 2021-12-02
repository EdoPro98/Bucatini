//**************************************************
// \file BucatiniActionInitialization.hh
// \brief: Definition of BucatiniActionInitialization class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 2021
//**************************************************

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
