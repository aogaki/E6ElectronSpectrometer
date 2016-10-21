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
   ESPrimaryGeneratorAction(G4bool useMonoEne, G4double beamEne, G4bool useZeroAng);
   virtual ~ESPrimaryGeneratorAction();

   virtual void GeneratePrimaries(G4Event *);

private:
   G4ParticleGun *fParticleGun;

   void MonoEneGun();
   void UniformGun();
   void (ESPrimaryGeneratorAction::*GunPointer)();
   G4bool fUseMonoEne;
   G4double fBeamEne;

   void ZeroAng();
   void UniformAng();
   void (ESPrimaryGeneratorAction::*AngGenPointer)();
   G4bool fUseZeroAng;
   G4ThreeVector fParVec;   
   G4double fThetaMax;
   G4double fCosMax;

   // Commands
   void DefineCommands();
   G4GenericMessenger *fMessenger;
   // Changing z position of source 
   void SetSourceZ(G4double z);
   G4double fZPosition;
   // Do I have to add for theta?
   
};

#endif
