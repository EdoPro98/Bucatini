//**************************************************
// \file BucatiniDetectorConstruction.cc
// \brief: Implementation of BucatiniDetectorConstruction class
// \author: Edoardo Proserpio edoardo.proserpio@gmail.com
// \start date: 7 July 2021
//**************************************************

#include "BucatiniDetectorConstruction.hh"
#include "BucatiniSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"

#include "G4SDManager.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "FastFiberModel.hh"

BucatiniDetectorConstruction::BucatiniDetectorConstruction()
    : G4VUserDetectorConstruction(), fCheckOverlaps(false) {
  DefineMaterials();
}

BucatiniDetectorConstruction::~BucatiniDetectorConstruction() {}

void BucatiniDetectorConstruction::DefineMaterials() {
  fMaterials = BucatiniMaterials::GetInstance();
}

G4VPhysicalVolume *BucatiniDetectorConstruction::Construct() {
  // Assign material to the calorimeter volumes
  G4Material *airMaterial = FindMaterial("G4_AIR");
  G4Material *siliconMaterial = FindMaterial("G4_Si");
  G4Material *teflonMaterial = FindMaterial("G4_TEFLON");
  G4Material *tubesMaterial = FindMaterial("brass");
  G4Material *scintillatingCoreMaterial = FindMaterial("polystyrene");
  G4Material *cherenkovCoreMaterial = FindMaterial("pmmaCher");
  G4Material *scintillatingCladMaterial = FindMaterial("pmmaScint");
  G4Material *cherenkovCladMaterial = FindMaterial("fluorinatedPolymer");
  G4Material *glassMaterial = FindMaterial("glass");

  // ------------------- //
  // Building geometries //
  // ------------------- //

  // World
  G4VSolid *worldS = new G4Box("World", worldX / 2, worldY / 2, worldZ / 2);
  G4LogicalVolume *worldLV = new G4LogicalVolume(worldS, airMaterial, "world");
  G4VPhysicalVolume *worldPV = new G4PVPlacement(
      0, G4ThreeVector(0, 0, 0), worldLV, "world", 0, false, 0, fCheckOverlaps);

  // Absorber to calculate leakage
  G4VSolid *leakageAbsorberS =
      new G4Sphere("leakageAbsorber", 2000., 2100., 0. * deg, 360. * deg,
                   0. * deg, 180. * deg);
  G4LogicalVolume *leakageAbsorberLV =
      new G4LogicalVolume(leakageAbsorberS, airMaterial, "leakageAbsorber");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 0), leakageAbsorberLV,
                    "leakageAbsorber", worldLV, false, 0, fCheckOverlaps);

  // Single calorimeter module 64x80 fibers
  G4VSolid *moduleS =
      new G4Box("module", moduleX / 2, moduleY / 2, moduleZ / 2 + sipmZ / 2);
  G4LogicalVolume *moduleLV =
      new G4LogicalVolume(moduleS, airMaterial, "module");

  // Light shield is on back of module in between sipm
  G4VSolid *shieldS =
      new G4Box("shield", moduleX / 2, moduleY / 2, sipmZ / 2 + 1 * mm);
  G4LogicalVolume *shieldLV =
      new G4LogicalVolume(shieldS, teflonMaterial, "shield");
  const G4ThreeVector shieldPos =
      G4ThreeVector(0, 0, moduleZ / 2 - sipmZ / 2 + 1 * mm);
  new G4PVPlacement(0, shieldPos, shieldLV, "shield", moduleLV, false, 0,
                    fCheckOverlaps);

  // Scield visual
  G4VisAttributes *shieldVisAttr =
      new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.25)); // green
  shieldVisAttr->SetVisibility(true);
  shieldVisAttr->SetForceWireframe(true);
  shieldVisAttr->SetForceSolid(true);
  shieldLV->SetVisAttributes(shieldVisAttr);

  // Calorimeter (matrix of modules)
  G4VSolid *calorimeterS =
      new G4Box("calorimeter", calorimeterX / 2, calorimeterY / 2,
                calorimeterZ / 2 + sipmZ / 2);
  G4LogicalVolume *calorimeterLV =
      new G4LogicalVolume(calorimeterS, airMaterial, "calorimeter");

  // Modules equipped placement
  int moduleCopyNumber = 0;
  for (int row = 0; row < nModuleRows; row++) {
    for (int col = 0; col < nModulesInRow[row]; col++) {

      const double modulePositionX =
          -(nModulesInRow[row] - 1) * moduleX / 2 + moduleX * col;
      const double modulePositionY =
          -(nModuleRows - 1) * moduleY / 2 + moduleY * row;

      G4ThreeVector modulePosition(modulePositionX, modulePositionY, 0.);

      new G4PVPlacement(0, modulePosition, moduleLV,
                        "module" + std::to_string(moduleCopyNumber),
                        calorimeterLV, false, moduleCopyNumber);
      moduleCopyNumber++;
    };
  };

  // Calorimeter placement (with rotation wrt beam axis)
  G4RotationMatrix calorimeterRotiationMatrix = G4RotationMatrix();
  calorimeterRotiationMatrix.rotateY(angleX);
  calorimeterRotiationMatrix.rotateX(angleY);
  G4ThreeVector calorimeterPosition(0, 0, 0);
  G4Transform3D calorimeterTransform =
      G4Transform3D(calorimeterRotiationMatrix, calorimeterPosition);

  new G4PVPlacement(calorimeterTransform, calorimeterLV, "calorimeter", worldLV,
                    false, 0, fCheckOverlaps);

  // Single sipm volume
  G4VSolid *sipmS = new G4Box("sipm", sipmX / 2, sipmY / 2, sipmZ / 2);
  G4LogicalVolume *sipmLV = new G4LogicalVolume(sipmS, glassMaterial, "sipm");

  // Silicon inside sipm
  G4VSolid *siliconS = new G4Box("sipmSilicon", sipmSiliconX / 2,
                                 sipmSiliconY / 2, sipmSiliconZ / 2);
  G4LogicalVolume *siliconLV =
      new G4LogicalVolume(siliconS, siliconMaterial, "sipmSilicon");

  const G4ThreeVector siliconPosition(0, 0, sipmZ / 2 - sipmSiliconZ / 2);
  new G4PVPlacement(0, siliconPosition, siliconLV, "sipmSilicon", sipmLV, false,
                    0, fCheckOverlaps);

  // SiPM silicon visual
  G4VisAttributes *siliconVisAttr =
      new G4VisAttributes(G4Colour(0.0, 0.8, 0.0)); // green
  siliconVisAttr->SetVisibility(true);
  siliconVisAttr->SetForceWireframe(true);
  siliconVisAttr->SetForceSolid(true);
  siliconLV->SetVisAttributes(siliconVisAttr);

  // Tube
  G4Tubs *tubeS = new G4Tubs("tube", tubeInnerRadius, tubeOuterRadius,
                             fiberZ / 2, 0. * deg, 360 * deg);
  G4LogicalVolume *tubeLV = new G4LogicalVolume(tubeS, tubesMaterial, "tube");

  // Clad and core solid
  G4Tubs *fiberCladS = new G4Tubs("fiberClad", fiberCoreRadius, tubeInnerRadius,
                                  fiberZ / 2, 0 * deg, 360 * deg);
  G4Tubs *fiberCoreS = new G4Tubs("fiberClad", 0, fiberCoreRadius, fiberZ / 2,
                                  0 * deg, 360 * deg);

  // S and C clad logical
  G4LogicalVolume *fiberScinCladLV =
      new G4LogicalVolume(fiberCladS, scintillatingCladMaterial, "fiberCladS");
  G4LogicalVolume *fiberCherCladLV =
      new G4LogicalVolume(fiberCladS, cherenkovCladMaterial, "fiberCladC");

  // S and C core logical
  G4LogicalVolume *fiberScinCoreLV =
      new G4LogicalVolume(fiberCoreS, scintillatingCoreMaterial, "fiberCoreS");
  G4LogicalVolume *fiberCherCoreLV =
      new G4LogicalVolume(fiberCoreS, cherenkovCoreMaterial, "fiberCoreC");

  // FastFiber setup
  fRegion = new G4Region("fiberRegion");
  fiberScinCoreLV->SetRegion(fRegion);
  fiberCherCoreLV->SetRegion(fRegion);
  fRegion->AddRootLogicalVolume(fiberScinCoreLV);
  fRegion->AddRootLogicalVolume(fiberCherCoreLV);

  // Vis attributes
  G4VisAttributes *tubeVisAtt = new G4VisAttributes(G4Colour(0.6, 0.3, 0.3));
  tubeVisAtt->SetVisibility(true);
  tubeVisAtt->SetForceWireframe(true);
  tubeVisAtt->SetForceSolid(true);
  tubeLV->SetVisAttributes(tubeVisAtt);

  G4VisAttributes *ScincoreVisAtt =
      new G4VisAttributes(G4Colour(0.0, 0.0, 0.8));
  ScincoreVisAtt->SetVisibility(true);
  ScincoreVisAtt->SetForceWireframe(true);
  ScincoreVisAtt->SetForceSolid(true);
  fiberScinCoreLV->SetVisAttributes(ScincoreVisAtt);

  G4VisAttributes *scinCladVisAtt =
      new G4VisAttributes(G4Colour(0.7, 0.85, 0.9));
  scinCladVisAtt->SetVisibility(true);
  scinCladVisAtt->SetForceWireframe(true);
  scinCladVisAtt->SetForceSolid(true);
  fiberScinCladLV->SetVisAttributes(scinCladVisAtt);

  G4VisAttributes *cherCoreVisAtt =
      new G4VisAttributes(G4Colour(0.8, 0.0, 0.0));
  cherCoreVisAtt->SetVisibility(true);
  cherCoreVisAtt->SetForceWireframe(true);
  cherCoreVisAtt->SetForceSolid(true);
  fiberCherCoreLV->SetVisAttributes(cherCoreVisAtt);

  G4VisAttributes *cherCladVisAtt =
      new G4VisAttributes(G4Colour(1.0, 0.4, 0.3));
  cherCladVisAtt->SetVisibility(true);
  cherCladVisAtt->SetForceWireframe(true);
  cherCladVisAtt->SetForceSolid(true);
  fiberCherCladLV->SetVisAttributes(cherCladVisAtt);

  // Fibers and sipm positioning in modules
  int fiberCopyNumber = 0;
  for (int row = 0; row < nFibersRows; row++) {
    for (int col = 0; col < nFibersCols; col++) {

      if (row % 2 != 0) {
        // S fiber
        const std::string fiberName =
            "S_row" + std::to_string(row) + "_col_" + std::to_string(col);
        const double fiberPositionX =
            -moduleX / 2 + 2 * tubeOuterRadius + 2 * tubeOuterRadius * col;
        const double fiberPositionY =
            -moduleY / 2 + tubeOuterRadius + 1.732 * tubeOuterRadius * row;

        const G4ThreeVector fiberPosition(fiberPositionX, fiberPositionY,
                                          -sipmZ / 2);
        const G4ThreeVector sipmPosition(fiberPositionX, fiberPositionY,
                                         fiberZ / 2);

        new G4PVPlacement(0, fiberPosition, tubeLV, fiberName, moduleLV, false,
                          fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, fiberPosition, fiberScinCladLV, "fiberCladS",
                          moduleLV, false, fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, fiberPosition, fiberScinCoreLV, "fiberCoreS",
                          moduleLV, false, fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, sipmPosition, sipmLV, "S_SiPM", moduleLV, false,
                          fiberCopyNumber, fCheckOverlaps);

        // C fiber
      } else {
        const std::string fiberName =
            "C_row" + std::to_string(row) + "_column_" + std::to_string(col);

        const double fiberPositionX =
            -moduleX / 2 + tubeOuterRadius + 2 * tubeOuterRadius * col;
        const double fiberPositionY =
            -moduleY / 2 + tubeOuterRadius + 1.732 * tubeOuterRadius * row;

        const G4ThreeVector fiberPosition(fiberPositionX, fiberPositionY,
                                          -sipmZ / 2);
        const G4ThreeVector sipmPosition(fiberPositionX, fiberPositionY,
                                         fiberZ / 2);

        new G4PVPlacement(0, fiberPosition, tubeLV, fiberName, moduleLV, false,
                          fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, fiberPosition, fiberCherCladLV, "fiberCladC",
                          moduleLV, false, fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, fiberPosition, fiberCherCoreLV, "fiberCoreC",
                          moduleLV, false, fiberCopyNumber, fCheckOverlaps);
        new G4PVPlacement(0, sipmPosition, sipmLV, "S_SiPM", moduleLV, false,
                          fiberCopyNumber, fCheckOverlaps);
      }
      fiberCopyNumber++;
    }
  }

  return worldPV;
}

void BucatiniDetectorConstruction::ConstructSDandField() {
  sipm::SiPMProperties sipmProperties;
  sipm::SiPMProperties sipmPropertiesScint;
  sipm::SiPMProperties sipmPropertiesCher;
  sipmProperties.setDcr(250e3);
  sipmProperties.setXt(0.01);
  sipmProperties.setAp(0.01);
  sipmProperties.setSnr(25);
  sipmProperties.setSignalLength(500);
  sipmProperties.setSampling(0.1);
  sipmProperties.setPitch(15);
  sipmProperties.setRiseTime(1);
  sipmProperties.setFallTimeFast(25);
  sipmProperties.setRecoveryTime(25);

  const std::vector<double> wlen = {280, 300, 325, 350, 375, 400, 425,
                                    450, 475, 500, 525, 550, 575, 600,
                                    625, 650, 675, 700, 800, 900, 1000};
  const std::vector<double> sipmpde = {
      0,    0.12, 0.16, 0.21, 0.22, 0.27, 0.30, 0.32, 0.32, 0.31, 0.30,
      0.27, 0.25, 0.23, 0.21, 0.20, 0.17, 0.14, 0.07, 0.03, 0.01};
  const std::vector<double> wratten3 = {
      1.,    1.,    1.,    1.,    1.,    1.,    1.,
      0.5,   0.067, 0.033, 0.025, 0.02,  0.017, 0.017,
      0.017, 0.017, 0.017, 0.017, 0.017, 0.017, 0.017};

  std::vector<double> pdeS(sipmpde.size());
  std::vector<double> pdeC(sipmpde.size());

  for (int i = 0; i < sipmpde.size(); ++i) {
    pdeS[i] = sipmpde[i] * wratten3[i];
    pdeC[i] = 0.63 * sipmpde[i];
  }

  sipmPropertiesScint = sipmProperties;
  sipmPropertiesCher = sipmProperties;
  sipmPropertiesScint.setPdeSpectrum(wlen, pdeS);
  sipmPropertiesCher.setPdeSpectrum(wlen, pdeC);
  // Pointer is deallocated in BucatiniSD~
  sipm::SiPMSensor *sipmS = new sipm::SiPMSensor(sipmPropertiesScint);
  sipm::SiPMSensor *sipmC = new sipm::SiPMSensor(sipmPropertiesCher);

  BucatiniSD *sipmSD =
      new BucatiniSD("sipmSD", "sipmHitsCollection", nSensor, sipmS, sipmC);
  G4SDManager::GetSDMpointer()->AddNewDetector(sipmSD);
  SetSensitiveDetector("sipmSilicon", sipmSD);

  new FastFiberModel("FastFiberModel", fRegion);
}
