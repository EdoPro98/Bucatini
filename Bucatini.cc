//**************************************************
// \file Bucatini.cc
// \brief: main() of Bucatini project
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
// \start date: 7 July 2021
//**************************************************

// Includers from project files
//
#include "BucatiniActionInitialization.hh"
#include "BucatiniDetectorConstruction.hh"
#include "BucatiniPhysicsList.hh"

// Includers from Geant4
//
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "FTFP_BERT.hh"
#include "G4UIExecutive.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

// G4err output for usage error
//
namespace PrintUsageError {
void UsageError() {
  G4cerr << "->Bucatini usage: " << G4endl;
  G4cerr
      << "Bucatini [-m macro ] [-u UIsession] [-t nThreads] [-pl PhysicsList]"
      << G4endl;
}
} // namespace PrintUsageError

// main() function
//
int main(int argc, char** argv) {

  // Error in argument numbers
  //
  if (argc > 11) {
    PrintUsageError::UsageError();
    return 1;
  }

  // Convert arguments in G4string and G4int
  //
  G4String macro;
  G4String session;
  G4String physiscsLinstName = "FTFP_BERT"; // default physics list
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif

  for (G4int i = 1; i < argc; i = i + 2) {
    if (G4String(argv[i]) == "-m")
      macro = argv[i + 1];
    else if (G4String(argv[i]) == "-u")
      session = argv[i + 1];
    else if (G4String(argv[i]) == "-pl")
      physiscsLinstName = argv[i + 1];
#ifdef G4MULTITHREADED
    else if (G4String(argv[i]) == "-t") {
      nThreads = G4UIcommand::ConvertToInt(argv[i + 1]);
    }
#endif
    else {
      PrintUsageError::UsageError();
      return 1;
    }
  }

  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if (!macro.size()) { // if macro card is none
    ui = new G4UIExecutive(argc, argv, session);
  }

// Construct the default run manager
//
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  if (nThreads > 0) {
    runManager->SetNumberOfThreads(nThreads);
  }
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  runManager->SetUserInitialization(new BucatiniDetectorConstruction());

  runManager->SetUserInitialization(new BucatiniPhysicsList(physiscsLinstName));

  runManager->SetUserInitialization(new BucatiniActionInitialization());

  // Initialize visualization
  auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if (macro.size()) { // macro card mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macro);
  } else { // start UI session
    UImanager->ApplyCommand("/control/execute Bucatini_init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute Bucatini_gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  // Program termination (user actions deleted by run manager)
  delete visManager;
  delete runManager;
}

//**************************************************
