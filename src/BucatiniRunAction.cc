#include "BucatiniRunAction.hh"
#include "BucatiniHistoManager.hh"

#include "G4Run.hh"

class HistoManager;
BucatiniRunAction::BucatiniRunAction(HistoManager* histoMgr) : G4UserRunAction(), fHistoManager(histoMgr) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  // Only merge in MT mode to avoid warning when running in Sequential mode
#ifdef G4MULTITHREADED
  analysisManager->SetNtupleMerging(true);
#endif
  // Create directories
  analysisManager->SetHistoDirectoryName("Histograms");
  analysisManager->SetNtupleDirectoryName("Ntuples");
}

BucatiniRunAction::~BucatiniRunAction() {}

void BucatiniRunAction::BeginOfRunAction(const G4Run*) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  analysisManager->OpenFile();
  fHistoManager->Book();
}

void BucatiniRunAction::EndOfRunAction(const G4Run*) {
  fHistoManager->PrintStatistic();
  fHistoManager->Save();
  G4AnalysisManager::Instance()->CloseFile();
  G4AnalysisManager::Instance()->Clear();
}
