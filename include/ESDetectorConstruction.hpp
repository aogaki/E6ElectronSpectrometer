#ifndef ESDetectorConstruction_h
#define ESDetectorConstruction_h 1

#ifdef NOTCPP11
#define nullptr NULL
#endif

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"

#include "ESConstants.hpp"


class ESDetectorConstruction : public G4VUserDetectorConstruction
{
public:
   ESDetectorConstruction(MirrorState mirrorState, ColliState colliState, DetState detState, G4bool vacFlag);
   virtual ~ESDetectorConstruction();

   virtual G4VPhysicalVolume *Construct();
   virtual void ConstructSDandField();

private:
   G4bool fCheckOverlap;

   G4LogicalVolume *fMagneticFieldLV;
   
   // Materials
   void DefineMaterials();
   G4Material *fVacuumMat;
   G4Material *fWindowMat;
   G4Material *fAirMat;
   G4Material *fLANEXMat;
   G4Material *fCollimatorMat;
   G4Material *fMagnetMat;
   G4Material *fMirrorGlassMat;
   G4Material *fMirrorAlMat;
   G4bool fVacFlag;
   
   // Geometry parameters
   // I have to think which variables should be defined in DefineGeometries()
   void DefineGeometries();

   G4double fVacuumT;
   G4double fWindowT;
   G4double fWindowZPos;

   G4double fAirT;
   
   ColliState fColliState;
   G4double fColliT;
   G4double fColliHole;

   MirrorState fMirrorState;
   
   G4double fMagnetH;
   G4double fMagnetW;
   G4double fMagnetL;
   G4double fMagnetGap;
   G4double fMagnetDepth;

   G4double fLANEXT;
   G4double fLANEXW;
   
   // Magnet
   G4LogicalVolume *ConstructMagnet();

   // Detectors (LANEX and Air, Air records incident LANEX)
   DetState fDetState;
   void ConstructBothDetectors();
   void ConstructHorizontalDetectors();
   void ConstructVerticalDetectors();

   void ConstructFront();

   void ConstructConverter();
   
   // Mirror
   void ConstructMirror();
   
   // For command
   void DefineCommands();
   void SetWindowT(G4double t);
   void SetWindowMat(G4String matName);
   G4GenericMessenger *fMessenger;
   G4VPhysicalVolume *fVacuumPV;
   G4VPhysicalVolume *fWindowPV;
   G4VPhysicalVolume *fAirPV;

   // For field
   void ConstructSD();
   void ConstructBField();
   
   std::vector<G4VisAttributes *> fVisAttributes;

};

#endif
