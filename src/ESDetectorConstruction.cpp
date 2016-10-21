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
#include "ESConstants.hpp"
#include "ESExitSD.hpp"


ESDetectorConstruction::ESDetectorConstruction(ColliState colliState)
   : G4VUserDetectorConstruction(),
     fVacuumMat(nullptr),
     fWindowMat(nullptr),
     fAirMat(nullptr),
     fCollimatorMat(nullptr),
     fMessenger(nullptr),
     fVacuumPV(nullptr),
     fWindowPV(nullptr),
     fAirPV(nullptr)
{
   fColliState = colliState;
   
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
   delete fCollimatorMat;
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
   fWindowMat = manager->FindOrBuildMaterial("G4_POLYCARBONATE");
   fAirMat = manager->FindOrBuildMaterial("G4_AIR");
   fCollimatorMat = manager->FindOrBuildMaterial("G4_Pb");

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
   fAirT = 5000.*mm;
   fWindowT = 3.*mm;
   fVacuumT = 5000.*mm;

   fWindowZPos = kSourceZPos + kSourceToWindow + fWindowT / 2.;

   fColliT = 100.*mm;
   fColliHole = 4.*mm;
   if(fColliState == ColliState::InVac) fColliHole = 3.5*mm;
}

G4VPhysicalVolume *ESDetectorConstruction::Construct()
{
   // world volume
   G4double worldX = 1.*m;
   G4double worldY = 1.*m;
   G4double worldZ = 10.*m;

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
   G4double airW = 1000.*mm;
   G4double airH = 1000.*mm;

   G4Box *airS = new G4Box("Air", airW / 2., airH / 2., fAirT / 2.);
   G4LogicalVolume *airLV = new G4LogicalVolume(airS, fAirMat, "Air");
   visAttributes = new G4VisAttributes(G4Colour::Cyan());
   airLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4double airZPos = fAirT / 2.;
   G4ThreeVector airPos = G4ThreeVector(0., 0., airZPos);
   fAirPV = new G4PVPlacement(nullptr, airPos, airLV, "Air", worldLV,
                              false, 0, fCheckOverlap);
   // window layer
   G4double windowW = airW;
   G4double windowH = airH;

   G4Box *windowS = new G4Box("Window", windowW / 2., windowH / 2., fWindowT / 2.);
   G4LogicalVolume *windowLV = new G4LogicalVolume(windowS, fWindowMat, "Window");
   visAttributes = new G4VisAttributes(G4Colour::Green());
   windowLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4ThreeVector windowPos = G4ThreeVector(0., 0., fWindowZPos);
   fWindowPV = new G4PVPlacement(nullptr, windowPos, windowLV, "Window", worldLV,
                                 false, 0, fCheckOverlap);
      
   // detecting layer at entrance of magnet
   G4double entW = airW;
   G4double entH = airH;
   G4double entT = -fWindowZPos - fWindowT / 2.;
   G4Box *entS = new G4Box("Entrance", entW / 2., entH / 2., entT / 2.);
   G4LogicalVolume *entLV = new G4LogicalVolume(entS, fAirMat, "Entrance");
   visAttributes = new G4VisAttributes(G4Colour::Magenta());
   entLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4double entZPos = -entT / 2.;
   G4ThreeVector entPos = G4ThreeVector(0., 0., entZPos);
   new G4PVPlacement(nullptr, entPos, entLV, "Entrance", worldLV,
                     false, 0, fCheckOverlap);


   // detecting layer at LANEX position
   G4double lanexW = airW;
   G4double lanexH = 0.01*nm;
   G4double lanexT = fAirT;
   G4Box *lanexS = new G4Box("LANEX", lanexW / 2., lanexH / 2., lanexT / 2.);
   G4LogicalVolume *lanexLV = new G4LogicalVolume(lanexS, fAirMat, "LANEX");
   visAttributes = new G4VisAttributes(G4Colour::Red());
   lanexLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4double lanexYPos = -135.*mm;
   G4ThreeVector lanexPos = G4ThreeVector(0., lanexYPos, 0.);
   new G4PVPlacement(nullptr, lanexPos, lanexLV, "LANEX", airLV,
                     false, 0, fCheckOverlap);


   // Collimator
   if(fColliState != ColliState::No){
      G4double colliW = airW;
      G4double colliH = airH;

      G4Box *plateS = new G4Box("Plate", colliW / 2., colliH / 2., fColliT / 2.);
      G4Tubs *holeS = new G4Tubs("Hole", 0., fColliHole / 2., fColliT / 2., 0., 2*CLHEP::pi);
      G4SubtractionSolid* colliS = new G4SubtractionSolid("Collimator" , plateS, holeS);      
      G4LogicalVolume *colliLV = new G4LogicalVolume(colliS, fCollimatorMat, "Collimator");
      visAttributes = new G4VisAttributes(G4Colour::Gray());
      colliLV->SetVisAttributes(visAttributes);
      fVisAttributes.push_back(visAttributes);

      G4double colliZPos;
      if(fColliState == ColliState::InAir) colliZPos = entT / 2. - fColliT / 2.;
      else if(fColliState == ColliState::InVac) colliZPos = kSourceZPos + kSourceToColliVac;
      G4ThreeVector colliPos = G4ThreeVector(0., 0., colliZPos);
      
      G4LogicalVolume *colliMotherLV;
      if(fColliState == ColliState::InAir) colliMotherLV = entLV;
      else if(fColliState == ColliState::InVac) colliMotherLV = worldLV;
      new G4PVPlacement(nullptr, colliPos, colliLV, "Collimator", colliMotherLV,
                        false, 0, fCheckOverlap);
   }
   
   return worldPV;
}

void ESDetectorConstruction::ConstructSDandField()
{
   G4VSensitiveDetector *ExitSD = new ESExitSD("Exit",
                                               "ExitHitsCollection");
 
   G4LogicalVolumeStore *lvStore = G4LogicalVolumeStore::GetInstance();
   for(auto &&lv: *lvStore){
      //if(lv->GetName() != "World")
      if(lv->GetName() == "Entrance" || lv->GetName() == "LANEX")
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
