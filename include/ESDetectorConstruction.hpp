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


class ESDetectorConstruction : public G4VUserDetectorConstruction
{
public:
   ESDetectorConstruction();
   virtual ~ESDetectorConstruction();

   virtual G4VPhysicalVolume *Construct();
   virtual void ConstructSDandField();

private:
   G4bool fCheckOverlap;

   // Materials
   void DefineMaterials();
   G4Material *fVacuumMat;
   G4Material *fWindowMat;
   G4Material *fAirMat;
   
   // Geometry parameters
   void DefineGeometries();
   G4double fVacuumT;
   G4double fWindowT;
   G4double fAirT;

   // For command
   void DefineCommands();
   void SetWindowT(G4double t);
   void SetWindowMat(G4String matName);
   G4GenericMessenger *fMessenger;
   G4VPhysicalVolume *fVacuumPV;
   G4VPhysicalVolume *fWindowPV;
   G4VPhysicalVolume *fAirPV;

   
   std::vector<G4VisAttributes *> fVisAttributes;

};

#endif
