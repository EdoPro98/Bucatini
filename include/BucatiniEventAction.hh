//**************************************************
// \file BucatiniEventAction.hh
// \brief: Definition of BucatiniEventAction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniEventAction_h
#define BucatiniEventAction_h 1

#include "BucatiniDetectorConstruction.hh"
#include "BucatiniHit.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

#include <array>

class HistoManager;

class BucatiniEventAction : public G4UserEventAction {

public:
  BucatiniEventAction(HistoManager*);
  virtual ~BucatiniEventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  void addEnergyEM(double);                 // Add em component
  void addEnergyScinFibers(double);         // Add energy in scintillating fibers
  void addEnergyCherFibers(double);         // Add energy in Cherenkov fibers
  void addCherenkovPhotons(int);            // Add cherenkov photoelectron
  void addScintillatingPhotons(int);        // Add scintillating photoelectron
  void addScintillatingPhotoelectrons(int); // Add scintillating photoelectron
  void addCherenkovPhotoelectrons(int);     // Add scintillating photoelectron
  void addEnergyDeposited(double);          // Add all energy deposited
  void savePrimaryPDGID(int);
  void savePrimaryEnergy(double);
  void addEnergyLeaked(double);

  void addSiPMHit(int, double);

private:
  BucatiniHitsCollection* GetHitsCollection(const int hcID, const G4Event* event) const;

  HistoManager* fHistoManager;

  double fEnergyEM;          // Energy of em component
  double fEnergyScinFibers;  // Energy in scintillating fibers
  double fEnergyCherFibers;  // Energy in Cherenkov fibers
  int fNPhotonsScint;        // Number of Cherenkov photons detected
  int fNPhotonsCher;         // Number of Scintillating photons detected
  int fNPhotoelectronsScint; // Number of Cherenkov photons detected
  int fNPhotoelectronsCher;  // Number of Scintillating photons detected
  double fEnergyDeposited;   // Total energy deposited (does not count energy)
  int fPrimaryPDGID;         // PDGID of primary particle
  double fEnergyPrimary;     // Primary particle energy
  double fEnergyLeaked;

  int fSipmHCID = -1;
};

// Inline functions definition
inline void BucatiniEventAction::addEnergyLeaked(const double e) { fEnergyLeaked += e; }

inline void BucatiniEventAction::savePrimaryPDGID(const int pdgid) { fPrimaryPDGID = pdgid; }

inline void BucatiniEventAction::savePrimaryEnergy(const double e) { fEnergyPrimary = e; }

inline void BucatiniEventAction::addEnergyEM(const double e) { fEnergyEM += e; }

inline void BucatiniEventAction::addEnergyScinFibers(const double e) { fEnergyScinFibers += e; }

inline void BucatiniEventAction::addEnergyCherFibers(const double e) { fEnergyCherFibers += e; }

inline void BucatiniEventAction::addScintillatingPhotons(const int n) { fNPhotonsScint += n; }

inline void BucatiniEventAction::addCherenkovPhotons(const int n) { fNPhotonsCher += n; }

inline void BucatiniEventAction::addScintillatingPhotoelectrons(const int n) { fNPhotoelectronsScint += n; }

inline void BucatiniEventAction::addCherenkovPhotoelectrons(const int n) { fNPhotoelectronsCher += n; }

inline void BucatiniEventAction::addEnergyDeposited(const double e) { fEnergyDeposited += e; }
#endif
