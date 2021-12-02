#include "BucatiniHistoManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager() : fFactoryOn(false), fAnalysisManager(G4AnalysisManager::Instance()) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HistoManager::Book() {
  // Create histograms.
  // Histogram ids are generated automatically starting from 0.

  // id = 0
  fAnalysisManager->CreateH1("TotalEnergyDeposited", "Energy deposited in calorimeter (MeV)", 1200, 0., 120 * GeV);
  // id = 1
  fAnalysisManager->CreateH1("TotalEmEnergy", "EM energy deposited in calorimeter (MeV)", 1200, 0., 120 * GeV);
  // id = 2
  fAnalysisManager->CreateH1("TotalScintillatingEnergy", "Energy deposited in S fibers (MeV)", 1200, 0., 120 * GeV);
  // id = 3
  fAnalysisManager->CreateH1("TotalCherenkovEnergy", "Energy deposited in C fibers (MeV)", 1200, 0., 120 * GeV);
  // id = 4
  fAnalysisManager->CreateH1("LeakedEnergy", "Energy escaped from the calorimeter (MeV)", 1200, 0., 120 * GeV);
  // id = 5
  fAnalysisManager->CreateH1("PrimaryEnergy", "Energy of primary particle (MeV)", 100, 0., 120 * GeV);
  // id = 6
  fAnalysisManager->CreateH1("ScintillatingPhotoelectrons", "Number of photoelectrons in S fibers", 1000, 0., 10000);
  // id = 7
  fAnalysisManager->CreateH1("CherenkovPhotoelectrons", "Number of photoelectrons in C fibers", 1000, 0., 10000);
  // id = 8
  fAnalysisManager->CreateH1("ScintillatingPhotons", "Number of photons in S fibers", 1000, 0., 10000);
  // id = 9
  fAnalysisManager->CreateH1("CherenkovPhotons", "Number of photons in C fibers", 1000, 0., 10000);

  // Create ntuples.
  // Ntuples ids are generated automatically starting from 0.

  // Create 1st ntuple (id = 0)
  fAnalysisManager->CreateNtuple("Energy", "Energy deposited in calorimeter");
  fAnalysisManager->CreateNtupleDColumn("Total");           // column Id = 0
  fAnalysisManager->CreateNtupleDColumn("Electromagnetic"); // column Id = 1
  fAnalysisManager->CreateNtupleDColumn("Scintillating");   // column Id = 2
  fAnalysisManager->CreateNtupleDColumn("Cherenkov");       // column Id = 3
  fAnalysisManager->CreateNtupleDColumn("Leaked");          // column Id = 4
  fAnalysisManager->CreateNtupleDColumn("Primary");         // column Id = 5
  fAnalysisManager->FinishNtuple();

  // Create 2nd ntuple (id = 1)
  fAnalysisManager->CreateNtuple("Photoelectrons", "Photoelectrons statistic");
  fAnalysisManager->CreateNtupleIColumn("ScintillatingPhotons");        // column Id = 0
  fAnalysisManager->CreateNtupleIColumn("CherenkovPhotons");            // column Id = 1
  fAnalysisManager->CreateNtupleIColumn("ScintillatingPhotoelectrons"); // column Id = 2
  fAnalysisManager->CreateNtupleIColumn("CherenkovPhotoelectrons");     // column Id = 3
  fAnalysisManager->FinishNtuple();

  // Create 3nd ntuple (id = 2)
  // Store std::vector<double> / std::vector<int>
  fAnalysisManager->CreateNtuple("SiPM", "SiPM Data");
  fAnalysisManager->CreateNtupleDColumn("Integral", fIntegralSiPM);            // Column Id = 0
  fAnalysisManager->CreateNtupleDColumn("ArrivingTime", fToaSiPM);             // Column Id = 1
  fAnalysisManager->CreateNtupleIColumn("Photons", fPhotonsSiPM);              // Column id = 2
  fAnalysisManager->CreateNtupleIColumn("Photoelectrons", fPhotoelectronSiPM); // Column id = 3
  fAnalysisManager->FinishNtuple();

  fFactoryOn = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save() {
  if (!fFactoryOn) {
    return;
  }

  fAnalysisManager->Write();
  fAnalysisManager->CloseFile();
  fFactoryOn = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillHisto(const int ih, const double xbin, const double weight) {
  fAnalysisManager->FillH1(ih, xbin, weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtuple(const double totE, const double emE, const double sE, const double cE, const double leakE,
                              const double primE, const int sPhe, const int cPhe, const int sPh, const int cPh,
                              const std::vector<double>& sipmInt, const std::vector<double>& sipmToa,
                              const std::vector<int>& sipmPhe, const std::vector<int>& sipmPh) {
  // Fill 1st ntuple ( id = 0)
  fAnalysisManager->FillNtupleDColumn(0, 0, totE);
  fAnalysisManager->FillNtupleDColumn(0, 1, emE);
  fAnalysisManager->FillNtupleDColumn(0, 2, sE);
  fAnalysisManager->FillNtupleDColumn(0, 3, cE);
  fAnalysisManager->FillNtupleDColumn(0, 4, leakE);
  fAnalysisManager->FillNtupleDColumn(0, 5, primE);
  fAnalysisManager->AddNtupleRow(0);

  // Fill 2nd ntuple ( id = 1)
  fAnalysisManager->FillNtupleIColumn(1, 0, sPh);
  fAnalysisManager->FillNtupleIColumn(1, 1, cPh);
  fAnalysisManager->FillNtupleIColumn(1, 2, sPhe);
  fAnalysisManager->FillNtupleIColumn(1, 3, cPhe);
  fAnalysisManager->AddNtupleRow(1);

  // Fill 3nd ntuple ( id = 2)
  // Those are std::vector so no need to fill
  // Just update vector values in class members
  setSiPMIntegral(sipmInt);
  setSiPMToa(sipmToa);
  setSiPMPhotoelectrons(sipmPhe);
  setSiPMPhotons(sipmPh);
  // fAnalysisManager->FillNtupleDColumn();
  // fAnalysisManager->FillNtupleDColumn();
  fAnalysisManager->AddNtupleRow(2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::PrintStatistic() {
  if (!fFactoryOn) {
    return;
  }

  G4cout << "Total energy deposited"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(0)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(0)->rms(), "Energy") << G4endl;
  G4cout << "Total Em energy deposited"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(1)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(1)->rms(), "Energy") << G4endl;
  G4cout << "Total Scintillating energy deposited"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(2)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(2)->rms(), "Energy") << G4endl;
  G4cout << "Total Cherenkov energy deposited"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(3)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(3)->rms(), "Energy") << G4endl;
  G4cout << "Total leaked energy"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(4)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(4)->rms(), "Energy") << G4endl;
  G4cout << "Primary energy"
         << ": mean = " << G4BestUnit(fAnalysisManager->GetH1(5)->mean(), "Energy")
         << " rms = " << G4BestUnit(fAnalysisManager->GetH1(5)->rms(), "Energy") << G4endl;
  G4cout << "Scintillating photoelectrons"
         << ": mean = " << fAnalysisManager->GetH1(6)->mean() << " rms = " << fAnalysisManager->GetH1(6)->rms()
         << G4endl;
  G4cout << "Cherenkov photoelectrons"
         << ": mean = " << fAnalysisManager->GetH1(7)->mean() << " rms = " << fAnalysisManager->GetH1(7)->rms()
         << G4endl;
  G4cout << "======================================================================" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
