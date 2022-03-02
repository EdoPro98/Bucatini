#include "BucatiniSD.hh"
#ifndef BucatiniHit_h
#define BucatiniHit_h 1

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"
#include "G4VHit.hh"
#include <vector>

class BucatiniHit : public G4VHit {
public:
  BucatiniHit();
  virtual ~BucatiniHit();


  inline void* operator new(size_t);
  inline void operator delete(void*);

  virtual void Print();

private:
  friend class BucatiniSD;
  friend class BucatiniEventAction;

  std::vector<double> fTimes;       // Photon hit time
  std::vector<double> fWavelengths; // Photon wlen
  
  int fNPhotoelectrons = 0;         // Photons detected
  double fIntegral = 0;
  double fToa = 0;
  bool fIsScint = false;

  int fRow,fCol,fModule;
  double fXPos;
  double fYPos;
};

using BucatiniHitsCollection = G4THitsCollection<BucatiniHit>;

extern G4ThreadLocal G4Allocator<BucatiniHit>* BucatiniHitAllocator;

inline void* BucatiniHit::operator new(size_t) {
  if (!BucatiniHitAllocator) {
    BucatiniHitAllocator = new G4Allocator<BucatiniHit>;
  }
  return (void*)BucatiniHitAllocator->MallocSingle();
}

inline void BucatiniHit::operator delete(void* hit) {
  if (!BucatiniHitAllocator) {
    BucatiniHitAllocator = new G4Allocator<BucatiniHit>;
  }
  BucatiniHitAllocator->FreeSingle((BucatiniHit*)hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
