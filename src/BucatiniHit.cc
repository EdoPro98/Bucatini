#include "BucatiniHit.hh"
#include "G4UnitsTable.hh"

#include <iostream>

G4ThreadLocal G4Allocator<BucatiniHit>* BucatiniHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BucatiniHit::BucatiniHit() : G4VHit() {}

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
  for (size_t i = 0; i < fTimes.size(); ++i) {
    G4cout << "Time: " << std::setw(7) << G4BestUnit(fTimes[i], "Time") << " Wavelength: " << std::setw(7)
           << G4BestUnit(fWavelengths[i], "Length") << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
