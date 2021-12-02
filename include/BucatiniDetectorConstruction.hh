//**************************************************
// \file BucatiniDetectorConstruction.hh
// \brief: Definition of BucatiniDetectorConstruction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#ifndef BucatiniDetectorConstruction_h
#define BucatiniDetectorConstruction_h 1

// Includers from Geant4
#include "BucatiniMaterials.hh"
#include "G4Region.hh"
#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include <array>

class G4Material;
class G4VPhysicalVolume;
// ----------------------------------------------------- //
// Geometrical constants that can be used in other files //
// ----------------------------------------------------- //

// Geometry parameters of a single tower/module
static constexpr int nModuleRows = 10;
static constexpr int nModuleCols = 24;
static constexpr int nFibersRows = 32;
static constexpr int nFibersCols = 16;
static constexpr std::array<int, nModuleRows> nModulesInRow = {10, 10, 20, 20, 24, 24, 20, 20, 10, 10};
static constexpr int nTotalModules = 168;
static constexpr double tubeOuterRadius = 1 * mm;
static constexpr double tubeInnerRadius = 0.5 * mm;
static constexpr double moduleX = 2 * tubeOuterRadius * nFibersCols;
static constexpr double moduleY = tubeOuterRadius * 1.732 * nFibersRows;
static constexpr double moduleZ = 2000. * mm;
static constexpr int nSensor = nFibersCols * nFibersRows * nTotalModules;

// Size of full calorimeter
static constexpr double calorimeterX = nModuleCols * moduleX;
static constexpr double calorimeterY = nModuleRows * moduleY;
static constexpr double calorimeterZ = moduleZ;
static constexpr double angleX = 1.0 * deg;
static constexpr double angleY = 0.0 * deg;

// Geometry parameters of the world, world is a G4Box
static constexpr double worldX = 2 * calorimeterX;
static constexpr double worldY = 2 * calorimeterY;
static constexpr double worldZ = 5 * calorimeterZ;

// Geometry parameters of the fiber
static constexpr double fiberCoreRadius = 0.485 * mm;
static constexpr double fiberZ = moduleZ;

// Geometry parameters of the SiPM
static constexpr double sipmX = 1.3 * mm;
static constexpr double sipmY = sipmX;
static constexpr double sipmZ = 0.36 * mm;

// Geometry parameters of the SiPM, active silicon layer
static constexpr double sipmSiliconX = 1. * mm;
static constexpr double sipmSiliconY = sipmSiliconX;
static constexpr double sipmSiliconZ = 0.05 * mm;

class BucatiniDetectorConstruction : public G4VUserDetectorConstruction {

public:
  BucatiniDetectorConstruction();
  virtual ~BucatiniDetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void ConstructMaterials();

  G4LogicalVolume* createScintillatingFiberLV(const double, G4Material*, const double, const double, G4Material*,
                                              const double, G4Material*);

  G4LogicalVolume* createCherenkovFiberLV(const double, G4Material*, const double, const double, G4Material*,
                                          const double, G4Material*);

private:
  void DefineMaterials();
  G4Material* FindMaterial(G4String matName) { return fMaterials->GetMaterial(matName); }

  bool fCheckOverlaps; // option for checking volumes overlaps
  G4Region* fRegion;
  BucatiniMaterials* fMaterials;
};

#endif
