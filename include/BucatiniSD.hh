//**************************************************
// \file BucatiniSD.hh
// \brief: Definition of BucatiniBRunAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniSD_h
#define BucatiniSD_h 1

#include "BucatiniDetectorConstruction.hh"
#include "BucatiniHit.hh"
#include "G4VSensitiveDetector.hh"

#include "SiPM.h"

class G4Step;
class G4HCofThisEvent;

class BucatiniSD : public G4VSensitiveDetector {
public:
  BucatiniSD(const std::string&, const std::string&, const int, sipm::SiPMSensor*, sipm::SiPMSensor*);
  virtual ~BucatiniSD();

  virtual void Initialize(G4HCofThisEvent* hitCollection);
  virtual bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

private:
  void Digitize(BucatiniHitsCollection*);
  static bool isScint(const int);

  BucatiniHitsCollection* fHitsCollection;
  const int fNofCells;
  sipm::SiPMSensor* fSiPMScint;
  sipm::SiPMSensor* fSiPMCher;
};

inline bool BucatiniSD::isScint(const int copyNumber) {
  const int row = copyNumber / nFibersCols;
  return (row % 2) != 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
