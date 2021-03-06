#include <stdio.h>
#include <string>
#include <fstream>

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
#include "G4Trd.hh"
#include "G4GenericMessenger.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UImanager.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4FieldManager.hh"
#include "G4AutoDelete.hh"
#include "G4SDManager.hh"

#include "ESDetectorConstruction.hpp"
#include "ESConstants.hpp"
#include "ESSD.hpp"
#include "ESMagneticField.hpp"


ESDetectorConstruction::ESDetectorConstruction(MirrorState mirrorState, ColliState colliState,
                                               DetState detState, G4bool vacFlag)
   : G4VUserDetectorConstruction(),
     fWorldLV(nullptr),
     fMagneticFieldLV(nullptr),
     fVacuumMat(nullptr),
     fWindowMat(nullptr),
     fAirMat(nullptr),
     fLANEXMat(nullptr),
     fCollimatorMat(nullptr),
     fMagnetMat(nullptr),
     fMirrorGlassMat(nullptr),
     fMirrorAlMat(nullptr),
     fFoilMat(nullptr),
     fVacFlag(vacFlag),
     fMessenger(nullptr),
     fVacuumPV(nullptr),
     fWindowPV(nullptr),
     fAirPV(nullptr)
{
   fMirrorState = mirrorState;
   fColliState = colliState;
   fDetState = detState;
   
   fCheckOverlap = true;
   DefineMaterials();
   DefineGeometries();
   DefineCommands();
}

ESDetectorConstruction::~ESDetectorConstruction()
{
   delete fMessenger;

   delete fVacuumPV;
   delete fWindowPV;
   delete fAirPV;

   delete fWorldLV;
   //delete fMagneticFieldLV;// this is same as fWorldLV;
}

void ESDetectorConstruction::DefineMaterials()
{
   G4NistManager *manager = G4NistManager::Instance();

   // NIST database materials
   fVacuumMat = manager->FindOrBuildMaterial("G4_Galactic");
   if(fVacFlag)
      fWindowMat = fAirMat = fCollimatorMat = fLANEXMat = fFoilMat
                 = fMirrorGlassMat = fMirrorAlMat = fVacuumMat;
   else{
      fWindowMat = manager->FindOrBuildMaterial("G4_POLYCARBONATE");
      fAirMat = manager->FindOrBuildMaterial("G4_AIR");
      fCollimatorMat = manager->FindOrBuildMaterial("G4_Pb");
      fMirrorGlassMat = manager->FindOrBuildMaterial("G4_GLASS_PLATE");
      fMirrorAlMat = manager->FindOrBuildMaterial("G4_Al");
      fLANEXMat = manager->FindOrBuildMaterial("G4_GADOLINIUM_OXYSULFIDE");
      fFoilMat = manager->FindOrBuildMaterial("G4_Al");
   }
   fMagnetMat = manager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
   fFoilMat = manager->FindOrBuildMaterial("G4_Al");
   
   /*
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
   */
}

void ESDetectorConstruction::DefineGeometries()
{
   fAirT = 3000.*mm;
   fWindowT = 3.*mm;
   fVacuumT = 5000.*mm;

   fWindowZPos = kSourceZPos + kSourceToWindow - fWindowT / 2.;

   fColliT = 100.*mm;
   fColliHole = 3.*mm;
   if(fColliState == ColliState::InVac) fColliHole = 3.5*mm;

   fMagnetH = 381.*mm;  // along Y axis
   fMagnetW = 392.*mm;   // along X axis
   fMagnetL = 1000.*mm; // along Z
   fMagnetGap = kMagnetGap;      // ... X 
   fMagnetDepth = 70.*mm;   // ... Y 

   fLANEXT = 305.*um;
   fLANEXW = 100.*mm;
}

G4VPhysicalVolume *ESDetectorConstruction::Construct()
{
   // world volume
   G4double worldX = 2.*m;
   G4double worldY = 2.*m;
   G4double worldZ = 7.*m;

   G4Box *worldS = new G4Box("World", worldX / 2., worldY / 2., worldZ / 2.);
   fWorldLV = new G4LogicalVolume(worldS, fVacuumMat, "World");

   G4VisAttributes *visAttributes = new G4VisAttributes(G4Colour::White());
   visAttributes->SetVisibility(false);
   fWorldLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4VPhysicalVolume *worldPV
      = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLV, "World", 0,
                          false, 0, fCheckOverlap);

   // Air layer
   G4double airW = worldX;
   G4double airH = worldY;

   G4Box *airS = new G4Box("Air", airW / 2., airH / 2., fAirT / 2.);
   G4LogicalVolume *airLV = new G4LogicalVolume(airS, fAirMat, "Air");
   visAttributes = new G4VisAttributes(G4Colour::Magenta());
   airLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4double airZPos = fAirT / 2. + fWindowZPos + fWindowT / 2.;
   G4ThreeVector airPos = G4ThreeVector(0., 0., airZPos);
   fAirPV = new G4PVPlacement(nullptr, airPos, airLV, "Air", fWorldLV,
                              false, 0, fCheckOverlap);

   // Magnet
   G4LogicalVolume *magnetLV = ConstructMagnet();
   visAttributes = new G4VisAttributes(G4Colour::Blue());
   magnetLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4double magnetZPos = -airZPos + fMagnetL / 2.;
   // The Y center of magnet is center of baseBox
   G4double magnetYPos = (fMagnetH) / 4. - fMagnetDepth / 2.;
   //magnetYPos -= 25.*mm;
   G4ThreeVector magnetPos = G4ThreeVector(0., magnetYPos, magnetZPos);
   new G4PVPlacement(nullptr, magnetPos, magnetLV, "Magnet", airLV,
                     false, 0, fCheckOverlap);   
   
   // Window layer
   G4double windowW = airW;
   G4double windowH = airH;

   G4Box *windowS = new G4Box("Window", windowW / 2., windowH / 2., fWindowT / 2.);
   G4LogicalVolume *windowLV = new G4LogicalVolume(windowS, fWindowMat, "Window");
   visAttributes = new G4VisAttributes(G4Colour::Green());
   windowLV->SetVisAttributes(visAttributes);
   fVisAttributes.push_back(visAttributes);

   G4ThreeVector windowPos = G4ThreeVector(0., 0., fWindowZPos);
   fWindowPV = new G4PVPlacement(nullptr, windowPos, windowLV, "Window", fWorldLV,
                                 false, 0, fCheckOverlap);

   // Collimator layer
   if(fColliState != ColliState::No){
      //G4double colliW = airW;
      //G4double colliH = airH;
      G4double colliW = 10.*cm;
      G4double colliH = 10.*cm;

      G4Box *plateS = new G4Box("Plate", colliW / 2., colliH / 2., fColliT / 2.);
      G4Tubs *holeS = new G4Tubs("Hole", 0., fColliHole / 2., fColliT / 2., 0., 2*CLHEP::pi);
      G4SubtractionSolid* colliS = new G4SubtractionSolid("Collimator" , plateS, holeS);      
      G4LogicalVolume *colliLV = new G4LogicalVolume(colliS, fCollimatorMat, "Collimator");
      visAttributes = new G4VisAttributes(G4Colour::Gray());
      colliLV->SetVisAttributes(visAttributes);
      fVisAttributes.push_back(visAttributes);

      G4double colliZPos{0.};
      if(fColliState == ColliState::InAir) colliZPos = -airZPos - fColliT / 2.;
      else if(fColliState == ColliState::InVac) colliZPos = kSourceZPos + kSourceToColliVac;
      G4ThreeVector colliPos = G4ThreeVector(0., 0., colliZPos);
      
      G4LogicalVolume *colliMotherLV{nullptr};
      if(fColliState == ColliState::InAir) colliMotherLV = airLV;
      else if(fColliState == ColliState::InVac) colliMotherLV = fWorldLV;
      new G4PVPlacement(nullptr, colliPos, colliLV, "Collimator", colliMotherLV,
                        false, 0, fCheckOverlap);
   }

   if(fDetState == DetState::Real || fDetState == DetState::NoFront){
      ConstructBothDetectors();
   }
   else if(fDetState == DetState::Vertical)
      ConstructVerticalDetectors();
   else if(fDetState == DetState::Horizontal)
      ConstructHorizontalDetectors();

   if(fMirrorState != MirrorState::No) ConstructMirror();

   if(fDetState != DetState::NoFront) ConstructFront();

   ConstructConverter();

   //ConstructFoil();
   
   fMagneticFieldLV = fWorldLV;
   
   return worldPV;
}

void ESDetectorConstruction::ConstructFoil()
{
   // Foil after source
   G4double foilSize = 100.*mm;
   G4double foilT = 0.1*mm;
   G4Box *foilS = new G4Box("Foil", foilSize / 2., foilSize / 2., foilT / 2.);
   G4LogicalVolume *foilLV = new G4LogicalVolume(foilS, fFoilMat, "Foil");
   foilLV->SetVisAttributes(G4Colour::White());

   G4double foilZPos = kSourceZPos + 1.*m;
   G4ThreeVector foilPos = G4ThreeVector(0., 0., foilZPos);
   G4RotationMatrix *foilRot = new G4RotationMatrix();
   foilRot->rotateY(45.0*deg);

   new G4PVPlacement(foilRot, foilPos, foilLV, "Foil", fWorldLV,
                     false, 0, fCheckOverlap);

}

void ESDetectorConstruction::ConstructFront()
{
   // Front LANEX
   G4double frontSize = 50.*mm;
   G4Box *frontS = new G4Box("FrontLANEX", frontSize / 2., frontSize / 2., fLANEXT / 2.);
   G4LogicalVolume *frontLV = new G4LogicalVolume(frontS, fLANEXMat, "FrontLANEX");
   frontLV->SetVisAttributes(G4Colour::Yellow());

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double frontZPos = -airZPos - 120.*mm;
   G4ThreeVector frontPos = G4ThreeVector(0., 0., frontZPos);
   G4RotationMatrix *frontRot = new G4RotationMatrix();
   frontRot->rotateY(45.0*deg);

   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   new G4PVPlacement(frontRot, frontPos, frontLV, "FrontLANEX", motherLV,
                     false, 0, fCheckOverlap);

}

void ESDetectorConstruction::ConstructConverter()
{
   // Gamma converter
   G4double size = 10.*mm;
   G4Box *converterS = new G4Box("Converter", size / 2., size / 2., size / 2.);
   G4LogicalVolume *converterLV = new G4LogicalVolume(converterS, fWindowMat, "Converter");
   converterLV->SetVisAttributes(G4Colour::Green());

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double converterZPos = -airZPos + 2000.*mm;
   G4ThreeVector converterPos = G4ThreeVector(0., 0., converterZPos);

   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   new G4PVPlacement(nullptr, converterPos, converterLV, "Converter", motherLV,
                     false, 0, fCheckOverlap);
}

G4LogicalVolume *ESDetectorConstruction::ConstructMagnet()
{
   G4double sidePlateT = 30.*mm;	  // thickness of the side-plates of the magnet (it was not provided)  
   G4double sidePlateH = fMagnetH - 70.;   // 70 is deduced from the drawing (it was not provided) 

// Construction of the magnet components... 
   G4double baseW = fMagnetW - 2. * sidePlateT;  // not provided, but ESTIMATED from drawing 
   G4double baseH = fMagnetH / 2.;   // ESTIMATED... 
   G4double topW = (fMagnetW - 2. * sidePlateT - fMagnetGap) / 2.;  // ESTIMATED... 

   G4Box *boxS = new G4Box ("box", baseW / 2., baseH / 2., fMagnetL / 2.);
   G4Box *gapS = new G4Box ("gap", fMagnetGap / 2., (fMagnetDepth + 1.) / 2., fMagnetL / 2.);
   G4SubtractionSolid *baseBoxS = new G4SubtractionSolid("baseBox", boxS, gapS, 0,
                                                         G4ThreeVector(0., -(baseH - (fMagnetDepth + 1.)) / 2., 0.)); 

// upper trapezoid of the magnet... 
   G4RotationMatrix *xRot90deg = new G4RotationMatrix();
   xRot90deg->rotateX(90.0*deg);

   G4Trd *upperBoxS = new G4Trd ("upperBox", baseW / 2., topW / 2., fMagnetL / 2., fMagnetL / 2., baseH / 2.); 
   G4UnionSolid *magnetS = new G4UnionSolid("Magnet" , baseBoxS, upperBoxS, xRot90deg,
                                            G4ThreeVector(0., baseH, 0.)); 

// side plates of the magnet... 
   G4Box *sidePlateS = new G4Box ("sidePlate", sidePlateT / 2.,  sidePlateH / 2., fMagnetL / 2. );
   G4double dx = (baseW + sidePlateT) / 2.; 
   G4double dy = (sidePlateH - baseH) / 2.; 
   magnetS = new G4UnionSolid("Magnet", magnetS, sidePlateS, 0, G4ThreeVector( dx, dy, 0.)); 
   magnetS = new G4UnionSolid("Magnet", magnetS, sidePlateS, 0, G4ThreeVector( -dx, dy, 0.)); 

   G4LogicalVolume *magnetLV = new G4LogicalVolume(magnetS, fMagnetMat, "Magnet"); 

   return magnetLV;
}

void ESDetectorConstruction::ConstructBothDetectors()
{
   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   
   G4double LANEX_W = fLANEXW;
   G4double LANEX_L = 1.9*m;
   //G4double LANEX_H = 100.*mm + fLANEXT;
   G4double LANEX_H = 200.*mm;

   G4Box *LANEXVerS = new G4Box("LANEXDetectorV", LANEX_W / 2., LANEX_H / 2., fLANEXT / 2.);
   G4LogicalVolume *LANEXVerLV = new G4LogicalVolume(LANEXVerS, fLANEXMat, "LANEXDetectorV");
   LANEXVerLV->SetVisAttributes(G4Colour::Yellow());

   G4Box *LANEXHorS = new G4Box("LANEXDetectorH", LANEX_W / 2., fLANEXT / 2., LANEX_L / 2.);
   G4LogicalVolume *LANEXHorLV = new G4LogicalVolume(LANEXHorS, fLANEXMat, "LANEXDetectorH");
   LANEXHorLV->SetVisAttributes(G4Colour::Yellow());

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double LANEXHorZPos = -airZPos + LANEX_L / 2.;
   G4double LANEXHorYPos = -150.*mm - fLANEXT / 2.;
   G4ThreeVector LANEXHorPos = G4ThreeVector(0., LANEXHorYPos, LANEXHorZPos);
   new G4PVPlacement(nullptr, LANEXHorPos, LANEXHorLV, "LANEXDetectorH", motherLV,
                     false, 0, fCheckOverlap);

   G4double LANEXVerZPos = -airZPos + fMagnetL + 1000.*mm + fLANEXT / 2.;
   G4double LANEXVerYPos = -50.*mm - LANEX_H / 2.;
   G4ThreeVector LANEXVerPos = G4ThreeVector(0., LANEXVerYPos, LANEXVerZPos);
   new G4PVPlacement(nullptr, LANEXVerPos, LANEXVerLV, "LANEXDetectorV", motherLV,
                     false, 0, fCheckOverlap);


}

void ESDetectorConstruction::ConstructVerticalDetectors()
{
   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   G4Box *boxS = (G4Box*)motherLV->GetSolid();
   
   G4double LANEXDetT = 300.0*um * 2.;
   G4Box *LANEXDetS = new G4Box("LANEXDetectorV", boxS->GetXHalfLength(), boxS->GetYHalfLength(), LANEXDetT / 2.);
   G4LogicalVolume *LANEXDetLV = new G4LogicalVolume(LANEXDetS, fLANEXMat, "LANEXDetectorV");
   LANEXDetLV->SetVisAttributes(G4Colour::Yellow());

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double LANEXDetZPos = -airZPos + fMagnetL + 1000.*mm;
   G4ThreeVector LANEXDetPos = G4ThreeVector(0., 0., LANEXDetZPos);
   new G4PVPlacement(nullptr, LANEXDetPos, LANEXDetLV, "LANEXDetectorV", motherLV,
                     false, 0, fCheckOverlap);

   G4double airDetT = LANEXDetT / 2.;
   G4Box *airDetS = new G4Box("AirDetectorV", boxS->GetXHalfLength(), boxS->GetYHalfLength(), airDetT / 2.);
   G4LogicalVolume *airDetLV = new G4LogicalVolume(airDetS, motherLV->GetMaterial(), "AirDetectorV");
   airDetLV->SetVisAttributes(G4Colour::Yellow());

   G4double airDetZPos = -airDetT / 2.;
   G4ThreeVector airDetPos = G4ThreeVector(0., 0., airDetZPos);
   new G4PVPlacement(nullptr, airDetPos, airDetLV, "AirDetectorV", LANEXDetLV,
                     false, 0, fCheckOverlap);

}

void ESDetectorConstruction::ConstructHorizontalDetectors()
{
   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   G4Box *boxS = (G4Box*)motherLV->GetSolid();

   G4double LANEXDetT = 300.0*um * 2.;
   G4double LANEXDetZ = 8000.*mm;
   G4Box *LANEXDetS = new G4Box("LANEXDetectorH", boxS->GetXHalfLength(), LANEXDetT / 2., LANEXDetZ / 2.);
   G4LogicalVolume *LANEXDetLV = new G4LogicalVolume(LANEXDetS, fLANEXMat, "LANEXDetectorH");
   LANEXDetLV->SetVisAttributes(G4Colour::Yellow());

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double LANEXDetZPos = -airZPos + LANEXDetZ / 2.;
   G4double LANEXDetYPos = -150.*mm;
   G4ThreeVector LANEXDetPos = G4ThreeVector(0., LANEXDetYPos, LANEXDetZPos);
   new G4PVPlacement(nullptr, LANEXDetPos, LANEXDetLV, "LANEXDetectorH", motherLV,
                     false, 0, fCheckOverlap);

   G4double airDetT = LANEXDetT / 2.;
   G4Box *airDetS = new G4Box("AirDetectorH", boxS->GetXHalfLength(), airDetT / 2., LANEXDetZ / 2.);
   G4LogicalVolume *airDetLV = new G4LogicalVolume(airDetS, motherLV->GetMaterial(), "AirDetectorH");
   airDetLV->SetVisAttributes(G4Colour::Yellow());

   G4double airDetYPos = airDetT / 2.;
   G4ThreeVector airDetPos = G4ThreeVector(0., airDetYPos, 0.);
   new G4PVPlacement(nullptr, airDetPos, airDetLV, "AirDetectorH", LANEXDetLV,
                     false, 0, fCheckOverlap);

}

void ESDetectorConstruction::ConstructMirror()
{
   G4LogicalVolume *motherLV = fAirPV->GetLogicalVolume();
   
   G4double mirrorT = 4.*mm;
   G4double mirrorW = fLANEXW * sqrt(2.);
   G4double mirrorL = 1.9*m;
   G4double AlT = 0.1*mm;
   
   G4Box *mirrorS = new G4Box("Mirror", mirrorW / 2., mirrorT / 2., mirrorL / 2.);
   G4LogicalVolume *mirrorLV = new G4LogicalVolume(mirrorS, fMirrorGlassMat, "Mirror");
   mirrorLV->SetVisAttributes(G4Colour::Cyan());

   G4Box *AlS = new G4Box("MirrorAl", mirrorW / 2., AlT / 2., mirrorL / 2.);
   G4LogicalVolume *AlLV = new G4LogicalVolume(AlS, fMirrorAlMat, "MirrorAl");
   AlLV->SetVisAttributes(G4Colour::White());

   G4double AlYPos = -mirrorT / 2. + AlT / 2.;
   G4ThreeVector AlPos = G4ThreeVector(0., AlYPos, 0.);
   new G4PVPlacement(nullptr, AlPos, AlLV, "MirrorAl", mirrorLV,
                     false, 0, fCheckOverlap);

   G4double airZPos = (fAirPV->GetTranslation())[2]; // Using ObjectTranslation?
   G4double mirrorZPos = -airZPos + mirrorL / 2.;
   G4double mirrorYPos = -225.*mm ;
   G4ThreeVector mirrorPos = G4ThreeVector(0., mirrorYPos, mirrorZPos);

   G4RotationMatrix *mirrorRot = new G4RotationMatrix();
   mirrorRot->rotateZ(-45.*deg);
   
   new G4PVPlacement(mirrorRot, mirrorPos, mirrorLV, "Mirror", motherLV,
                     false, 0, fCheckOverlap);

}

void ESDetectorConstruction::ConstructSDandField()
{
   ConstructSD();
   ConstructBField();
}

void ESDetectorConstruction::ConstructSD()
{   
   // Sensitive Detectors
   G4VSensitiveDetector *SD = new ESSD("SD",
                                       "HC");
   G4SDManager::GetSDMpointer()->AddNewDetector(SD);
   
   G4LogicalVolumeStore *lvStore = G4LogicalVolumeStore::GetInstance();
   for(auto &&lv: *lvStore){
      //if(lv->GetName() != "World" && lv->GetName() != "Air")
      //if(lv->GetName() == "Window" || (lv->GetName()).contains("Detector"))
      if(lv->GetName().contains("LANEX") ||
         lv->GetName().contains("Window") ||
         lv->GetName().contains("Converter"))
         SetSensitiveDetector(lv->GetName(), SD);
   }
}

void ESDetectorConstruction::ConstructBField()
{
   // Create magnetic field and set it to Tube using the function
   ESMagneticField *magneticField = new ESMagneticField();
   fMagneticFieldLV->SetFieldManager(magneticField->GetFieldManager(), true); 

   G4FieldManager *fieldManager = new G4FieldManager();
   fieldManager->SetDetectorField(magneticField);
   fieldManager->CreateChordFinder(magneticField);
   G4bool forceToAllDaughters = true;
   fMagneticFieldLV->SetFieldManager(fieldManager, forceToAllDaughters);

   // Register the field and its manager for deleting
   G4AutoDelete::Register(magneticField);
   G4AutoDelete::Register(fieldManager);
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
