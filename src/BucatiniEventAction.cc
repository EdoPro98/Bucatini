//**************************************************
// \file BucatiniEventAction.cc
// \brief: Implementation of BucatiniEventAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "BucatiniEventAction.hh"
#include "BucatiniDetectorConstruction.hh"
#include "BucatiniHistoManager.hh"
#include "BucatiniHit.hh"
#include "BucatiniSD.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include "g4root.hh"

#include "SiPM.h"

BucatiniEventAction::BucatiniEventAction(HistoManager* histoMgr)
    : G4UserEventAction(), fHistoManager(histoMgr) {}

BucatiniEventAction::~BucatiniEventAction() {}

BucatiniHitsCollection*
BucatiniEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const {

  BucatiniHitsCollection* hitsCollection = static_cast<BucatiniHitsCollection*>(
      event->GetHCofThisEvent()->GetHC(hcID));
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
  if (fSipmHCID == -1) {
    fSipmHCID =
        G4SDManager::GetSDMpointer()->GetCollectionID("sipmHitsCollection");
  }

  BucatiniHitsCollection* hitsCollection = GetHitsCollection(fSipmHCID, event);

  std::vector<double> sipmIntegral(nSensor);
  std::vector<double> sipmToa(nSensor);
  std::vector<int> sipmNphe(nSensor);
  std::vector<int> sipmNph(nSensor);

  for (int i = 0; i < nSensor; ++i) {
    const BucatiniHit* hit = (*hitsCollection)[i];
    if (hit->isScint() == true) {
      // Scint
      fNPhotoelectronsScint += hit->nPhotoelectrons();
      fNPhotonsScint += hit->nPhotons();
    } else {
      // Cher
      fNPhotonsCher += hit->nPhotons();
      fNPhotoelectronsCher += hit->nPhotoelectrons();
    }
    sipmIntegral[i] = hit->integral();
    sipmToa[i] = hit->toa();
    sipmNph[i] = hit->nPhotons();
    sipmNphe[i] = hit->nPhotoelectrons();
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

  fHistoManager->FillNtuple(fEnergyDeposited, fEnergyEM, fEnergyScinFibers,
                            fEnergyCherFibers, fEnergyLeaked, fEnergyPrimary,
                            fNPhotoelectronsScint, fNPhotoelectronsCher,
                            fNPhotonsScint, fNPhotonsCher, sipmIntegral,
                            sipmToa, sipmNphe, sipmNph);
}
