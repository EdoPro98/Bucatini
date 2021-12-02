#include "BucatiniHit.hh"
#include "G4Box.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<BucatiniHit>* BucatiniHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BucatiniHit::BucatiniHit() : G4VHit() {
  fTimes.reserve(1000);
  fWavelengths.reserve(1000);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BucatiniHit::~BucatiniHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool BucatiniHit::operator==(const BucatiniHit& right) const { return (this == &right) ? true : false; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BucatiniHit::Print() {
  if (fTimes.size() > 0) {
    G4cout << "========================================" << G4endl;
    G4cout << "Integral: " << std::setw(7) << fIntegral << " Toa: " << std::setw(7) << fToa << G4endl;
  }
  for (int i = 0; i < fTimes.size(); ++i) {
    G4cout << "Time: " << std::setw(7) << G4BestUnit(fTimes[i], "Time") << " Wavelength: " << std::setw(7)
           << G4BestUnit(fWavelengths[i], "Length") << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
