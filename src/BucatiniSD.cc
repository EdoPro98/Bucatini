#include "BucatiniSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BucatiniSD::BucatiniSD(const std::string& name, const std::string& hitsCollectionName, const int nofCells,
                       sipm::SiPMSensor* sipmS, sipm::SiPMSensor* sipmC)
  : G4VSensitiveDetector(name), fHitsCollection(nullptr), fNofCells(nofCells), fSiPMScint(sipmS), fSiPMCher(sipmC) {
  collectionName.insert(hitsCollectionName);
}

BucatiniSD::~BucatiniSD() {}

void BucatiniSD::Initialize(G4HCofThisEvent* hce) {
  // Create hits collection
  fHitsCollection = new BucatiniHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);

  // Create hits
  // fNofCells for cells + one more for total sums
  for (G4int i = 0; i < fNofCells; i++) {
    fHitsCollection->insert(new BucatiniHit());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool BucatiniSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  const G4ParticleDefinition* particleDef = step->GetTrack()->GetParticleDefinition();

  if (particleDef == G4OpticalPhoton::Definition()) {
    const auto touchable = step->GetPreStepPoint()->GetTouchable();
    const int sipmCopyNumber = touchable->GetCopyNumber(1);
    const int moduleCopyNumber = touchable->GetCopyNumber(2);
    const int sipmId = sipmCopyNumber + (moduleCopyNumber * nFibersCols * nFibersRows);
    BucatiniHit* hit = static_cast<BucatiniHit*>(fHitsCollection->GetHit(sipmId));

    if (isScint(sipmCopyNumber)) {
      hit->setIsScint(true);
    }

    const double time = step->GetTrack()->GetGlobalTime();
    const double wlen = 1239.84193 / step->GetTrack()->GetKineticEnergy() * eV;
    hit->add(time, wlen);
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return true;
  }
  return false;
}

void BucatiniSD::Digitize(BucatiniHitsCollection* hc) {
  const int nHits = hc->entries();
  for (int i = 0; i < nHits; ++i) {
    BucatiniHit* hit = static_cast<BucatiniHit*>(hc->GetHit(i));
    const std::vector<double> times = hit->times();
    const std::vector<double> wlens = hit->wavelengths();

    // Differentiate C and S sipm
    sipm::SiPMSensor* lSensor;
    if (hit->isScint() == true) {
      lSensor = fSiPMScint;
    } else {
      lSensor = fSiPMCher;
    }
    if (hit->nPhotons() > 0) {
      lSensor->resetState();
      lSensor->addPhotons(times, wlens);
      lSensor->runEvent();
      const double integral = lSensor->signal().integral(0, 250, 0.5);
      const double toa = lSensor->signal().toa(0, 250, 0.5);
      const int nPhe = lSensor->debug().nPhotoelectrons;
      hit->digitize(integral, toa, nPhe);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BucatiniSD::EndOfEvent(G4HCofThisEvent* hce) {
  auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  Digitize(static_cast<BucatiniHitsCollection*>(hce->GetHC(hcID)));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
