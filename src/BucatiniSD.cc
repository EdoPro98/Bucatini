#include "BucatiniSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "BucatiniDetectorConstruction.hh"
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
  for (G4int i = 0; i < fNofCells; i++) {
    auto hit = new BucatiniHit();
    fHitsCollection->insert(hit);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool BucatiniSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if (step->GetTrack()->GetParticleDefinition() != G4OpticalPhoton::Definition()) {
    return false;
  }
  // GetCopyNumber(1) == sipmColCopyNumber  (aka col in row)
  // GetCopyNumber(2) == sipmRowCopyNumber  (aka row in module)
  // GetCopyNumber(3) == moduleCopyNumber   (aka module id in calo)
  const auto touchable = step->GetPreStepPoint()->GetTouchable();
  const int sipmCol = touchable->GetCopyNumber(1);
  const int sipmRow = touchable->GetCopyNumber(2);
  const int module = touchable->GetCopyNumber(3);
  const int sipmId = module * nFibersRows * nFibersCols + sipmRow * nFibersCols + sipmCol; 
  BucatiniHit* hit = static_cast<BucatiniHit*>(fHitsCollection->GetHit(sipmId));

  hit->fTimes.push_back(step->GetTrack()->GetGlobalTime());
  hit->fWavelengths.push_back(1239.84193 / step->GetTrack()->GetKineticEnergy() * eV);
  hit->fModule = module;
  hit->fRow = sipmRow;
  hit->fCol = sipmCol;
  hit->fIsScint = (sipmRow % 2 == 0);

  step->GetTrack()->SetTrackStatus(fStopAndKill);
  return true;
}

void BucatiniSD::Digitize(BucatiniHit* hit) {
  sipm::SiPMSensor* lSensor;
  if (hit->fIsScint) {
    lSensor = fSiPMScint;
  } else {
    lSensor = fSiPMCher;
  }
  
  lSensor->resetState();
  lSensor->addPhotons(hit->fTimes, hit->fWavelengths);
  lSensor->runEvent();
  const double integral = lSensor->signal().integral(0, 250, 1.5);
  const double toa = lSensor->signal().toa(0, 250, 1.5);
  const int nPhe = lSensor->debug().nPhotoelectrons;
  hit->fIntegral = integral;
  hit->fToa = toa;
  hit->fNPhotoelectrons = nPhe;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BucatiniSD::EndOfEvent(G4HCofThisEvent* hce) {
  auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  auto hc = hce->GetHC(hcID);
  for (int i = 0; i < nSensor; ++i) {
    BucatiniHit* hit = static_cast<BucatiniHit*>(hc->GetHit(i));
    // Digitise only if at least 2 photon hit sipm
    if(hit->fTimes.size() > 2)
      Digitize(hit);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
