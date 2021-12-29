#ifndef BucatiniHit_h
#define BucatiniHit_h 1

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"
#include <vector>

class BucatiniHit : public G4VHit {
public:
  BucatiniHit();
  BucatiniHit(const BucatiniHit&);
  virtual ~BucatiniHit();

  // operators
  bool operator==(const BucatiniHit&) const;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  virtual void Draw(){};
  virtual void Print();

  void add(const double, const double);
  void digitize(const double, const double, const int);
  void setIsScint(const bool);

  const std::vector<double>& times() const;
  const std::vector<double>& wavelengths() const;
  int nPhotons() const { return fTimes.size(); }
  int nPhotoelectrons() const { return fNPhotoelectrons; }
  double integral() const { return fIntegral; }
  double toa() const { return fToa; }
  bool isScint() const { return fIsScint; }

private:
  std::vector<double> fTimes;       // Photon hit time
  std::vector<double> fWavelengths; // Photon wlen
  int fNPhotoelectrons = 0;         // Photons detected
  double fIntegral = 0;
  double fToa = 0;
  bool fIsScint = false;
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

inline void BucatiniHit::add(const double t, const double w) {
  fTimes.emplace_back(t);
  fWavelengths.emplace_back(w);
}

inline void BucatiniHit::setIsScint(const bool isScint) { fIsScint = isScint; }

inline const std::vector<double>& BucatiniHit::times() const { return fTimes; }

inline const std::vector<double>& BucatiniHit::wavelengths() const {
  return fWavelengths;
}

inline void BucatiniHit::digitize(const double integral, const double toa,
                                  const int nphe) {
  fIntegral = integral;
  fToa = toa;
  fNPhotoelectrons = nphe;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
