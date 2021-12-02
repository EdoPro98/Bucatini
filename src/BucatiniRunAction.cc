//**************************************************
// \file BucatiniRunAction.cc
// \brief: Implementation of BucatiniRunAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

// Includers from project files
//
#include "BucatiniRunAction.hh"
#include "BucatiniEventAction.hh"

// Includers from Geant4
//
#include "BucatiniHistoManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "g4root.hh"

#include <string>

class HistoManager;
BucatiniRunAction::BucatiniRunAction(HistoManager *histoMgr)
    : G4UserRunAction(), fHistoManager(histoMgr) {
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  // Only merge in MT mode to avoid warning when running in Sequential mode
#ifdef G4MULTITHREADED
  analysisManager->SetNtupleMerging(true);
#endif
  // Create directories
  analysisManager->SetHistoDirectoryName("Histograms");
  analysisManager->SetNtupleDirectoryName("Ntuples");
}

BucatiniRunAction::~BucatiniRunAction() {}

void BucatiniRunAction::BeginOfRunAction(const G4Run *) {
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  analysisManager->OpenFile();
  fHistoManager->Book();
}

void BucatiniRunAction::EndOfRunAction(const G4Run *) {
  fHistoManager->PrintStatistic();
  fHistoManager->Save();
}
