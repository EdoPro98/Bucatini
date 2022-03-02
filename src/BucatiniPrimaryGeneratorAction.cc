#include "BucatiniPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

BucatiniPrimaryGeneratorAction::BucatiniPrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), fGeneralParticleSource(nullptr) {

  // Particle source used is GPS
  fGeneralParticleSource = new G4GeneralParticleSource();

  // default G4GeneralParticleSource parameters (can be changed via UI)
  G4ParticleDefinition* particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fGeneralParticleSource->SetParticleDefinition(particleDefinition);
  fGeneralParticleSource->SetParticlePosition(G4ThreeVector(0., 0., 0.));
}

BucatiniPrimaryGeneratorAction::~BucatiniPrimaryGeneratorAction() {}

// GeneratePrimaries() method
//
void BucatiniPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
}

//**************************************************
