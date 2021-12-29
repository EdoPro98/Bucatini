#include "BucatiniMaterials.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <cmath>
#include <stdio.h>

BucatiniMaterials* BucatiniMaterials::fInstance = 0;

BucatiniMaterials::BucatiniMaterials() {
  fNistMan = G4NistManager::Instance();
  CreateMaterials();
}

BucatiniMaterials::~BucatiniMaterials() {
  if (fInstance != 0)
    delete fInstance;
  fInstance = 0;
}

BucatiniMaterials* BucatiniMaterials::GetInstance() {
  if (fInstance == 0)
    fInstance = new BucatiniMaterials();

  return fInstance;
}

G4Material* BucatiniMaterials::GetMaterial(const G4String matName) {
  G4Material* mat = fNistMan->FindOrBuildMaterial(matName);

  if (!mat)
    mat = G4Material::GetMaterial(matName);
  if (!mat) {
    std::ostringstream o;
    o << "Material " << matName << " not found!";
    G4Exception("BucatiniMaterials::GetMaterial", "", FatalException,
                o.str().c_str());
  }

  return mat;
}

void BucatiniMaterials::CreateMaterials() {

  // --------------------------------------- //
  // Define Elements, Mixtures and Materials //
  // --------------------------------------- //

  // Elements //
  std::string name, symbol;
  double a, z;

  a = 1.01 * g / mole;
  G4Element* elementH =
      new G4Element(name = "Hydrogen", symbol = "H", z = 1., a); // Hidrogen

  a = 12.01 * g / mole;
  G4Element* elementC =
      new G4Element(name = "Carbon", symbol = "C", z = 6., a); // Carbon

  a = 16.00 * g / mole;
  G4Element* elementO =
      new G4Element(name = "Oxygen", symbol = "O", z = 8., a); // Oxygen

  a = 28.09 * g / mole;
  G4Element* elementSi =
      new G4Element(name = "Silicon", symbol = "Si", z = 14., a); // Silicon

  a = 18.9984 * g / mole;
  G4Element* elementF =
      new G4Element(name = "Fluorine", symbol = "F", z = 9., a); // Fluorine

  a = 63.546 * g / mole;
  G4Element* elementCu =
      new G4Element(name = "Copper", symbol = "Cu", z = 29., a); // Copper

  a = 65.38 * g / mole;
  G4Element* elementZn =
      new G4Element(name = "Zinc", symbol = "Zn", z = 30., a); // Zinc

  // Materials //
  fNistMan = G4NistManager::Instance();
  fNistMan->FindOrBuildMaterial("G4_Cu");
  fNistMan->FindOrBuildMaterial("G4_Si");
  fNistMan->FindOrBuildMaterial("G4_AIR");
  fNistMan->FindOrBuildMaterial("G4_TEFLON");

  fAir = G4Material::GetMaterial("G4_AIR");
  fTeflon = G4Material::GetMaterial("G4_TEFLON");
  fSilicon = G4Material::GetMaterial("G4_Si");

  // Polystyrene from elements (C5H5)
  fPolystyrene = new G4Material(name = "polystyrene", 1.05 * g / cm3, 2);
  fPolystyrene->AddElement(elementC, 8);
  fPolystyrene->AddElement(elementH, 8);

  // PMMA material from elements (C5O2H8)
  fPmmaS = new G4Material(name = "pmmaScint", 1.19 * g / cm3, 3);
  fPmmaS->AddElement(elementC, 5);
  fPmmaS->AddElement(elementO, 2);
  fPmmaS->AddElement(elementH, 8);

  // PMMA material from elements (C5O2H8)
  fPmmaC = new G4Material(name = "pmmaCher", 1.19 * g / cm3, 3);
  fPmmaC->AddElement(elementC, 5);
  fPmmaC->AddElement(elementO, 2);
  fPmmaC->AddElement(elementH, 8);

  // Fluorinated Polymer material from elements (C2F2)
  // material for the cladding of the Cherenkov fibers
  fFluorinatedPoly =
      new G4Material(name = "fluorinatedPolymer", 1.43 * g / cm3, 2);
  fFluorinatedPoly->AddElement(elementC, 2);
  fFluorinatedPoly->AddElement(elementF, 2);

  // Glass material from elements (SiO2)
  fGlass = new G4Material(name = "glass", 2.4 * g / cm3, 2);
  fGlass->AddElement(elementSi, 1);
  fGlass->AddElement(elementO, 2);

  // Mixtures //

  // Brass (Copper260)
  fBrass = new G4Material(name = "brass", 8.53 * g / cm3, 2);
  fBrass->AddElement(elementCu, 70 * perCent);
  fBrass->AddElement(elementZn, 30 * perCent);

  G4MaterialPropertiesTable* MPTAir = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTSilicon = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTPMMAScint = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTPMMACher = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTFluoPoly = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTGlass = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable* MPTPolystyrene = new G4MaterialPropertiesTable();
  // 600., 590., 580., 570., 560., 550., 540., 530.,
  // 520., 510., 500., 490., 480., 470., 460., 450.,
  // 440., 430., 420., 400., 350., 300.
  std::vector<double> photonEnergy = {
      2.06640 * eV, 2.10143 * eV, 2.13766 * eV, 2.17516 * eV, 2.21400 * eV,
      2.25426 * eV, 2.29600 * eV, 2.33932 * eV, 2.38431 * eV, 2.43106 * eV,
      2.47968 * eV, 2.53029 * eV, 2.58300 * eV, 2.63796 * eV, 2.69531 * eV,
      2.75520 * eV, 2.81782 * eV, 2.88335 * eV, 2.95200 * eV, 3.09960 * eV,
      3.54241 * eV, 4.13281 * eV};

  const int nEntries = photonEnergy.size();
  std::vector<double> refractiveindexAir(nEntries, 1.0);
  MPTAir->AddProperty("RINDEX", photonEnergy, refractiveindexAir)
      ->SetSpline(true);
  fAir->SetMaterialPropertiesTable(MPTAir);

  // ---------------------------------------------- //
  // Optical properties of Scintillating fiber core //
  // ---------------------------------------------- //
  // https://refractiveindex.info
  std::vector<double> refractiveindexPolystyrene = {
      1.5882,  1.58898, 1.58996, 1.59084, 1.59217, 1.59338, 1.59443, 1.59587,
      1.59721, 1.59862, 1.60021, 1.60201, 1.60372, 1.60594, 1.60797, 1.61037,
      1.61279, 1.61546, 1.61834, 1.62552, 1.63,    1.63};
  std::vector<double> absLenPolystyrene = {
      5.429 * m, 13.00 * m, 14.50 * m, 16.00 * m, 18.00 * m, 16.50 * m,
      17.00 * m, 14.00 * m, 16.00 * m, 15.00 * m, 14.50 * m, 13.00 * m,
      12.00 * m, 10.00 * m, 8.000 * m, 7.238 * m, 4.000 * m, 1.200 * m,
      0.500 * m, 0.200 * m, 0.200 * m, 0.100 * m};
  // Saint Gobain BCF-10 product catalog
  std::vector<double> emissionPolystyrene = {
      0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.02, 0.04, 0.06, 0.09, 0.10,
      0.12, 0.20, 0.30, 0.40, 0.60, 0.80, 1.00, 0.70, 0.0,  0.0,  0.0};
  MPTPolystyrene
      ->AddProperty("RINDEX", photonEnergy, refractiveindexPolystyrene)
      ->SetSpline(true);
  MPTPolystyrene->AddProperty("ABSLENGTH", photonEnergy, absLenPolystyrene)
      ->SetSpline(true);
  MPTPolystyrene
      ->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy,
                    emissionPolystyrene)
      ->SetSpline(true);
  MPTPolystyrene->AddConstProperty("SCINTILLATIONYIELD", 3200. / MeV);
  MPTPolystyrene->AddConstProperty("RESOLUTIONSCALE", 1.0);
  MPTPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.7 * ns);

  fPolystyrene->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
  fPolystyrene->SetMaterialPropertiesTable(MPTPolystyrene);

  // -------------------------------------------- //
  // Optical properties of Scintillating cladding //
  // -------------------------------------------- //
  // https://refractiveindex.info
  std::vector<double> refractiveIndexPmma = {
      1.49216, 1.4927,  1.49324, 1.4937,  1.49405, 1.49463, 1.49521, 1.49594,
      1.49639, 1.49722, 1.49795, 1.49873, 1.49955, 1.50056, 1.50114, 1.50227,
      1.50342, 1.50426, 1.50539, 1.50818, 1.51,    1.52};
  std::vector<double> absLenPmma = {
      39.48 * m, 48.25 * m, 54.29 * m, 57.91 * m, 54.29 * m, 33.40 * m,
      31.02 * m, 43.43 * m, 43.43 * m, 41.36 * m, 39.48 * m, 37.76 * m,
      36.19 * m, 36.19 * m, 33.40 * m, 31.02 * m, 28.95 * m, 25.55 * m,
      24.13 * m, 21.71 * m, 2.171 * m, 0.434 * m};
  MPTPMMAScint->AddProperty("RINDEX", photonEnergy, refractiveIndexPmma)
      ->SetSpline(true);
  MPTPMMAScint->AddProperty("ABSLENGTH", photonEnergy, absLenPmma);
  fPmmaS->SetMaterialPropertiesTable(MPTPMMAScint);

  // ------------------------------------------ //
  // Optical properties of Cherenkov fiber core //
  // ------------------------------------------ //
  // https://refractiveindex.info
  MPTPMMACher->AddProperty("RINDEX", photonEnergy, refractiveIndexPmma)
      ->SetSpline(true);
  MPTPMMACher->AddProperty("ABSLENGTH", photonEnergy, absLenPmma);
  fPmmaC->SetMaterialPropertiesTable(MPTPMMACher);

  // ---------------------------------------------- //
  // Optical properties of Cherenkov fiber cladding //
  // ---------------------------------------------- //
  std::vector<double> refractiveIndexFluoPoly(nEntries, 1.42);
  MPTFluoPoly->AddProperty("RINDEX", photonEnergy, refractiveIndexFluoPoly);
  fFluorinatedPoly->SetMaterialPropertiesTable(MPTFluoPoly);

  // --------------------------- //
  // Optical properties of Glass //
  // --------------------------- //
  std::vector<double> refractiveIndexGlass(nEntries, 1.51);
  MPTGlass->AddProperty("RINDEX", photonEnergy, refractiveIndexGlass);
  fGlass->SetMaterialPropertiesTable(MPTGlass);

  // ------------------------------ //
  // Optical properties of Silicon  //
  // ------------------------------ //
  std::vector<double> refractiveIndexSilicon = {
      3.94,  3.963, 3.988, 4.015, 4.045, 4.077, 4.112, 4.151,
      4.193, 4.241, 4.294, 4.35,  4.419, 4.497, 4.587, 4.691,
      4.812, 4.949, 5.119, 5.613, 5.494, 4.976};

  std::vector<double> absLenSilicon(nEntries, 0.001 * mm);

  MPTSilicon->AddProperty("RINDEX", photonEnergy, refractiveIndexSilicon)
      ->SetSpline(true);
  MPTSilicon->AddProperty("ABSLENGTH", photonEnergy, absLenSilicon);
  fSilicon->SetMaterialPropertiesTable(MPTSilicon);
}
