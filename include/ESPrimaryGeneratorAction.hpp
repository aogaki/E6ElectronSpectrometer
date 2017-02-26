#ifndef ESPrimaryGeneratorAction_h
#define ESPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

#include "ESConstants.hpp"


class G4Event;
class G4ParticleGun;
class G4GenericMessenger;
class TF1;
class TH1D;
class TFile;


class ESPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
   ESPrimaryGeneratorAction(BeamState beam, G4double beamEne);
   virtual ~ESPrimaryGeneratorAction();

   virtual void GeneratePrimaries(G4Event *);

private:
   G4ParticleGun *fParticleGun;

   BeamState fBeamState;
   G4double fBeamEne;

   void MonoEneGun();
   void UniformGun();
   void ReferenceGun();
   void NamGun();
   void (ESPrimaryGeneratorAction::*GunPointer)();
   TF1 *fFncNorm;
   TH1D *fHisBeam;
   
   void ZeroAng();
   void UniformAng();
   void (ESPrimaryGeneratorAction::*AngGenPointer)();
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
