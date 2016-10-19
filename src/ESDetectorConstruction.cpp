#include <stdio.h>
#include <string>
#include <fstream>

#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4CutTubs.hh"
#include "G4GenericMessenger.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UImanager.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "ESDetectorConstruction.hpp"
#include "ESExitSD.hpp"


ESDetectorConstruction::ESDetectorConstruction()
   : G4VUserDetectorConstruction(),
     fVacuumMat(nullptr),
     fWindowMat(nullptr),
     fAirMat(nullptr),
     fMessenger(nullptr),
     fVacuumPV(nullptr),
     fWindowPV(nullptr),
     fAirPV(nullptr)
{
   fCheckOverlap = true;
   DefineMaterials();
   DefineGeometries();
   DefineCommands();
}

ESDetectorConstruction::~ESDetectorConstruction()
{
   delete fVacuumMat;
   delete fWindowMat;
   delete fAirMat;
   delete fMessenger;
   delete fVacuumPV;
   delete fWindowPV;
   delete fAirPV;
}

void ESDetectorConstruction::DefineMaterials()
{
   G4NistManager *manager = G4NistManager::Instance();

   // NIST database materials
   fVacuumMat = manager->FindOrBuildMaterial("G4_Galactic");
   fAirMat = manager->FindOrBuildMaterial("G4_AIR");
   fWindowMat = manager->FindOrBuildMaterial("G4_POLYCARBONATE");

   // Acrylic C5O2H8
   G4Element *eleH  = manager->FindOrBuildElement("H");
   G4Element *eleC  = manager->FindOrBuildElement("C");
   G4Element *eleO  = manager->FindOrBuildElement("O");
   if(eleH == nullptr || eleC == nullptr || eleO == nullptr){
      G4cout << "Check the elements definition" << G4endl;
      exit(0);
   }
   G4double acrylicRho = 1.18*g/cm3;
   G4Material *acrylic = new G4Material("Acrylic", acrylicRho, 3);
   acrylic->AddElement(eleC, 5);
   acrylic->AddElement(eleO, 2);
   acrylic->AddElement(eleH, 8);

   //fWindowMat = acrylic;
}

void ESDetectorConstruction::DefineGeometries()
{
   fAirT = 1.*m;
   fWindowT = 3.*mm;
   fVacuumT = 2.*m;
}

G4VPhysicalVolume *ESDetectorConstruction::Construct()
{
   // world volume
   G4double worldX = 6.*m;
   G4double worldY = 6.*m;
   G4double worldZ = 8.*m;

   G4Box *worldS = new G4Box("World", worldX / 2., worldY / 2., worldZ / 2.);
   G4LogicalVolume *worldLV = new G4LogicalVolume(worldS, fVacuumMat, "World");

   G4VisAttributes *visAttributes = new G4VisAttributes(G4Colour::White());
   visAttributes->SetVisibility(false);
   worldLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4VPhysicalVolume *worldPV
      = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", 0,
                          false, 0, fCheckOverlap);

   // Air layer
   G4double airW = worldX;
   G4double airH = worldY;

   G4Box *airS = new G4Box("Air", airW / 2., airH / 2., fAirT / 2.);
   G4LogicalVolume *airLV = new G4LogicalVolume(airS, fAirMat, "Air");
   visAttributes = new G4VisAttributes(G4Colour::Cyan());
   airLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4ThreeVector airPos = G4ThreeVector(0., 0., -fAirT / 2.);
   fAirPV = new G4PVPlacement(nullptr, airPos, airLV, "Air", worldLV,
                              false, 0, fCheckOverlap);
      
   // window layer
   G4double windowW = worldX;
   G4double windowH = worldY;

   G4Box *windowS = new G4Box("Window", windowW / 2., windowH / 2., fWindowT / 2.);
   G4LogicalVolume *windowLV = new G4LogicalVolume(windowS, fWindowMat, "Window");
   visAttributes = new G4VisAttributes(G4Colour::Green());
   windowLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4ThreeVector windowPos = G4ThreeVector(0., 0., (-fWindowT / 2.) - fAirT);
   fWindowPV = new G4PVPlacement(nullptr, windowPos, windowLV, "Window", worldLV,
                                 false, 0, fCheckOverlap);
      
   // vacuum layer
   G4double vacuumW = worldX;
   G4double vacuumH = worldY;

   G4Box *vacuumS = new G4Box("Vacuum", vacuumW / 2., vacuumH / 2., fVacuumT / 2.);
   G4LogicalVolume *vacuumLV = new G4LogicalVolume(vacuumS, fVacuumMat, "Vacuum");
   visAttributes = new G4VisAttributes(G4Colour::White());
   vacuumLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4ThreeVector vacuumPos = G4ThreeVector(0., 0., (-fVacuumT / 2.) - fAirT - fWindowT);
   fVacuumPV = new G4PVPlacement(nullptr, vacuumPos, vacuumLV, "Vacuum", worldLV,
                                 false, 0, fCheckOverlap);
      
   return worldPV;
}

void ESDetectorConstruction::ConstructSDandField()
{
   G4VSensitiveDetector *ExitSD = new ESExitSD("Exit",
                                               "ExitHitsCollection");
 
   G4LogicalVolumeStore *lvStore = G4LogicalVolumeStore::GetInstance();
   for(auto &&lv: *lvStore){
      if(lv->GetName() != "World")
      SetSensitiveDetector(lv->GetName(), ExitSD);
   }
}

void ESDetectorConstruction::DefineCommands()
{
   fMessenger = new G4GenericMessenger(this, "/ES/Geometry/", 
                                       "For geometries");
   
   
   G4GenericMessenger::Command &windowTCmd
      = fMessenger->DeclareMethodWithUnit("WindowThickness", "mm",
                                          &ESDetectorConstruction::SetWindowT, 
                                          "Set the thickness of the window.");
   windowTCmd.SetParameterName("thickness", true);
   windowTCmd.SetRange("thickness>=0. && thickness<=2000.");
   windowTCmd.SetDefaultValue("10.0");

   G4GenericMessenger::Command &windowMatCmd
      = fMessenger->DeclareMethod("WindowMaterial",
                                  &ESDetectorConstruction::SetWindowMat, 
                                  "Set the material of the window.");
   windowMatCmd.SetParameterName("matName", true);
}

void ESDetectorConstruction::SetWindowT(G4double t)
{
   fWindowT = t;
   if(t > 0.){
      G4Box *window = (G4Box*)(fWindowPV->GetLogicalVolume()->GetSolid());
      window->SetZHalfLength(fWindowT / 2.);

      // change position
      G4ThreeVector windowPos = G4ThreeVector(0., 0., (-fWindowT / 2.) - fAirT);
      fWindowPV->SetTranslation(windowPos);
      
      G4ThreeVector vacuumPos = G4ThreeVector(0., 0., (-fVacuumT / 2.) - fAirT - fWindowT);
      fVacuumPV->SetTranslation(vacuumPos);
   }
   else{ // temporary.  exit is better?
      G4cout << "No thickness? OK, material of window is changed to vacuum." << G4endl;
      G4LogicalVolume *windowLV = fWindowPV->GetLogicalVolume();
      windowLV->SetMaterial(fVacuumMat);
   }
   
   G4RunManager::GetRunManager()->GeometryHasBeenModified();

   // Changing the beam source position
   G4UImanager *UImanager = G4UImanager::GetUIpointer();
   G4String command = "/ES/Primary/Z ";
   // source position <-> window upstream face: 1700 mm
   G4double zPosition = -(fWindowT + fAirT + 1700.*mm);
   UImanager->ApplyCommand(command + std::to_string(zPosition));
}

void ESDetectorConstruction::SetWindowMat(G4String matName)
{
   G4NistManager *manager = G4NistManager::Instance();
   G4Material *mat = nullptr;
   mat = manager->FindOrBuildMaterial(matName);
   if(mat == nullptr){
      G4cout << matName << " is not a defined material.\n"
             << "Window material is not changed." << G4endl;
      //exit(0);
   }
   else{
      G4LogicalVolume *windowLV = fWindowPV->GetLogicalVolume();
      windowLV->SetMaterial(mat);
      
      G4RunManager::GetRunManager()->GeometryHasBeenModified();
   }
}
