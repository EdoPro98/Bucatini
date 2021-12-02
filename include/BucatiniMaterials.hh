#ifndef BucatiniMaterials_h
#define BucatiniMaterials_h 1

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

class BucatiniMaterials {
public:
  virtual ~BucatiniMaterials();
  static BucatiniMaterials* GetInstance();
  G4Material* GetMaterial(const G4String);

private:
  BucatiniMaterials();
  void CreateMaterials();

  static BucatiniMaterials* fInstance;
  G4NistManager* fNistMan;
  G4Material* fAir;
  G4Material* fSilicon;
  G4Material* fTeflon;
  G4Material* fBrass;
  G4Material* fPolystyrene;
  G4Material* fPmmaS;
  G4Material* fPmmaC;
  G4Material* fFluorinatedPoly;
  G4Material* fGlass;
};

#endif
