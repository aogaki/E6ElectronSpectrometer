#ifndef ESPrimaryGeneratorAction_h
#define ESPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"


class G4Event;
class G4ParticleGun;
class G4GenericMessenger;
class TF1;
class TH2D;
class TFile;

class ESPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
   ESPrimaryGeneratorAction();
   virtual ~ESPrimaryGeneratorAction();

   virtual void GeneratePrimaries(G4Event *);

private:
   G4ParticleGun *fParticleGun;
   
   G4double fZPosition;
   G4double fThetaMax;
   G4double fCosMax;
};

#endif
