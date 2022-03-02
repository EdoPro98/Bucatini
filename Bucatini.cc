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

// Includers from Geant4
#include "G4FastSimulationPhysics.hh"
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"
#include "G4PhysListFactoryAlt.hh"
#include "G4PhysListRegistry.hh"
#include "G4PhysicsConstructorFactory.hh"
#include "G4PhysListStamper.hh"
#include "G4VModularPhysicsList.hh"
#include "G4OpticalParameters.hh"
#include "G4StepLimiter.hh"
#include "G4StepLimiterPhysics.hh"

namespace PrintUsageError {
void UsageError() {
  G4cerr << "->Bucatini usage: " << G4endl;
  G4cerr << "Bucatini [-m macro ] [-u UIsession] [-t nThreads] [-pl PhysicsList]" << G4endl;
}
} // namespace PrintUsageError

namespace{
  void PrintAvailable(G4int verbosity) {
    G4cout << G4endl;
    G4cout << "extensibleFactory: here are the available physics lists:"
           << G4endl;
    g4alt::G4PhysListFactory factory;
    factory.PrintAvailablePhysLists();

    // if user asked for extra verbosity then print physics ctors as well
    if ( verbosity > 1 ) {
      G4cout << G4endl;
      G4cout << "extensibleFactory: "
             << "here are the available physics ctors that can be added:"
             << G4endl;
      G4PhysicsConstructorRegistry* g4pctorFactory =
        G4PhysicsConstructorRegistry::Instance();
      g4pctorFactory->PrintAvailablePhysicsConstructors();
    }
  }
}

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
  G4String physListName;
  int nThreads = 0;

for (G4int i = 1; i < argc; i = i + 2) {
  if (G4String(argv[i]) == "-m")
    macro = argv[i + 1];
  else if (G4String(argv[i]) == "-u")
    session = argv[i + 1];
  else if (G4String(argv[i]) == "-pl")
    physListName = argv[i + 1];
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

#ifdef G4MULTITHREADED
G4RunManager* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default, nThreads);
#else
G4RunManager* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
#endif

G4Random::setTheEngine(new CLHEP::MTwistEngine());

// Detect interactive mode (if no macro provided) and define UI session
//
G4UIExecutive* ui = nullptr;
if (!macro.size()) { // if macro card is none
  ui = new G4UIExecutive(argc, argv, session);
}

g4alt::G4PhysListFactory factory;
G4VModularPhysicsList* physList = nullptr;

factory.SetDefaultReferencePhysList("FTFP_BERT");

G4PhysListRegistry* plreg = G4PhysListRegistry::Instance();
plreg->AddPhysicsExtension("OPTICAL","G4OpticalPhysics");
plreg->AddPhysicsExtension("STEPLIMIT","G4StepLimiterPhysics");

if ( physListName.size() ) {
  if ( G4VisExecutive::Verbosity() > 0 ) {
    G4cout << "extensibleFactory: explicitly using '"
           << physListName << "'" << G4endl;
  }
  physList = factory.GetReferencePhysList(physListName);
} else {
  if ( G4VisExecutive::Verbosity() > 0 ) {
    G4cout << "extensibleFactory: no -pl flag;"
           << " using ReferencePhysList() (FTFP_BERT)" << G4endl;
  }
  physList = factory.ReferencePhysList();
}

if ( ! physList ) {
  G4cerr << "extensibleFactory: PhysicsList '"
         << physListName
         << "' was not available in g4alt::PhysListFactory." << G4endl;
  PrintAvailable(G4VisExecutive::Verbosity());

  // if we can't get what the user asked for...
  //    don't go on to use something else, that's confusing
  G4ExceptionDescription ED;
  ED << "The factory for the physicslist ["
     << physListName
     << "] does not exist!"
     << G4endl;
  G4Exception("Bucatini", "Buactini", FatalException, ED);
  exit(42);
}

G4FastSimulationPhysics* fastSimPhysics = new G4FastSimulationPhysics();
fastSimPhysics->ActivateFastSimulation("opticalphoton");
physList->RegisterPhysics(fastSimPhysics);

G4OpticalParameters::Instance()->SetProcessActivation("Cerenkov", true);
G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", true);
G4OpticalParameters::Instance()->SetProcessActivation("OpAbsorption", true);
G4OpticalParameters::Instance()->SetProcessActivation("OpRayleigh", false);
G4OpticalParameters::Instance()->SetProcessActivation("OpMieHG", false);
G4OpticalParameters::Instance()->SetProcessActivation("OpWLS", false);
G4OpticalParameters::Instance()->SetProcessActivation("OpWLS2", false);

//G4OpticalParameters::Instance()->SetCerenkovStackPhotons(false);
//G4OpticalParameters::Instance()->SetScintStackPhotons(false);
G4OpticalParameters::Instance()->SetScintTrackInfo(false);
G4OpticalParameters::Instance()->SetScintTrackSecondariesFirst(false);  // only relevant if we actually stack and trace the optical photons
G4OpticalParameters::Instance()->SetCerenkovTrackSecondariesFirst(false);  // only relevant if we actually stack and trace the optical photons
G4OpticalParameters::Instance()->SetCerenkovMaxPhotonsPerStep(100);
G4OpticalParameters::Instance()->SetCerenkovMaxBetaChange(10.0);

G4OpticalParameters::Instance()->Dump();
physList->DumpList();  

// Set mandatory initialization classes
runManager->SetUserInitialization(new BucatiniDetectorConstruction());
runManager->SetUserInitialization(physList);
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
