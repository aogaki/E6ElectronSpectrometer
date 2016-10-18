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
   ESPrimaryGeneratorAction(G4bool useMonoEne, G4double beamEne);
   virtual ~ESPrimaryGeneratorAction();

   virtual void GeneratePrimaries(G4Event *);

private:
   G4ParticleGun *fParticleGun;

   void MonoEneGun();
   void UniformGun();
   void (ESPrimaryGeneratorAction::*GunFuncPointer)();
   G4bool fUseMonoEne;
   G4double fBeamEne;
   G4ThreeVector fParVec;
   
   G4double fZPosition;
   G4double fThetaMax;
   G4double fCosMax;
};

#endif
