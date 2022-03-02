#ifndef BucatiniSD_h
#define BucatiniSD_h 1

#include "BucatiniDetectorConstruction.hh"
#include "G4VSensitiveDetector.hh"
#include "BucatiniHit.hh"

#include "SiPM.h"

class G4Step;
class G4HCofThisEvent;
class BucatiniHit;

class BucatiniSD : public G4VSensitiveDetector {
public:
  BucatiniSD(const std::string&, const std::string&, const int, sipm::SiPMSensor*, sipm::SiPMSensor*);
  virtual ~BucatiniSD();

  virtual void Initialize(G4HCofThisEvent* hitCollection);
  virtual bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

private:
  void Digitize(BucatiniHit*, const bool);

  BucatiniHitsCollection* fHitsCollection;
  const int fNofCells;
  sipm::SiPMSensor* fSiPMScint;
  sipm::SiPMSensor* fSiPMCher;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
