#include "BucatiniEventAction.hh"
#include "BucatiniHit.hh"

#include "BucatiniHistoManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"

BucatiniEventAction::BucatiniEventAction(HistoManager* histoMgr) : G4UserEventAction(), fHistoManager(histoMgr) {}

BucatiniEventAction::~BucatiniEventAction() {}

BucatiniHitsCollection* BucatiniEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const {

  BucatiniHitsCollection* hitsCollection = static_cast<BucatiniHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
  return hitsCollection;
}

void BucatiniEventAction::BeginOfEventAction(const G4Event*) {
  fEnergyEM = 0.;
  fEnergyScinFibers = 0.;
  fEnergyCherFibers = 0.;
  fNPhotonsScint = 0;
  fNPhotonsCher = 0;
  fNPhotoelectronsScint = 0;
  fNPhotoelectronsCher = 0;
  fEnergyLeaked = 0;
  fEnergyDeposited = 0;
  fEnergyPrimary = 0;
}

void BucatiniEventAction::EndOfEventAction(const G4Event* event) {
  std::vector<double> sipmIntegral(nSensor);
  std::vector<double> sipmToa(nSensor);
  std::vector<int> sipmNphe(nSensor);
  std::vector<int> sipmNph(nSensor);

  fSipmHCID = G4SDManager::GetSDMpointer()->GetCollectionID("sipmHitsCollection");

  BucatiniHitsCollection* hitsCollection = GetHitsCollection(fSipmHCID, event);

  for (int i = 0; i < nSensor; ++i) {
    const BucatiniHit* hit = static_cast<BucatiniHit*>(hitsCollection->GetHit(i));
    if (hit->fIsScint) {
      // Scint
      fNPhotoelectronsScint += hit->fNPhotoelectrons;
      fNPhotonsScint += hit->fTimes.size();
    } else {
      // Cher
      fNPhotoelectronsCher += hit->fNPhotoelectrons;
      fNPhotonsCher += hit->fTimes.size();
    }
    sipmIntegral[i] = hit->fIntegral;
    sipmToa[i] = hit->fToa;
    sipmNph[i] = hit->fTimes.size();
    sipmNphe[i] = hit->fNPhotoelectrons;
  }

  fHistoManager->FillHisto(0, fEnergyDeposited * MeV);
  fHistoManager->FillHisto(1, fEnergyEM * MeV);
  fHistoManager->FillHisto(2, fEnergyScinFibers * MeV);
  fHistoManager->FillHisto(3, fEnergyCherFibers * MeV);
  fHistoManager->FillHisto(4, fEnergyLeaked * MeV);
  fHistoManager->FillHisto(5, fEnergyPrimary * MeV);
  fHistoManager->FillHisto(6, fNPhotoelectronsScint);
  fHistoManager->FillHisto(7, fNPhotoelectronsCher);
  fHistoManager->FillHisto(8, fNPhotonsScint);
  fHistoManager->FillHisto(9, fNPhotonsCher);

  fHistoManager->FillNtuple(fEnergyDeposited, fEnergyEM, fEnergyScinFibers, fEnergyCherFibers, fEnergyLeaked,
                            fEnergyPrimary, fNPhotoelectronsScint, fNPhotoelectronsCher, fNPhotonsScint, fNPhotonsCher,
                            sipmIntegral, sipmToa, sipmNphe, sipmNph);
}
