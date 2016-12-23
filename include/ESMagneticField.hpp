#ifndef ESMagneticField_h
#define ESMagneticField_h 1

#include "G4MagneticField.hh"
#include "G4FieldManager.hh"

#include "G4SystemOfUnits.hh"

// For avoiding the CLHEP::s warning messages
// ROOT classes have to be include .cpp file.
class TFile;
class TH2D;

class ESMagneticField : public G4MagneticField
{
public:
   // For converting local corrdinate, need offset (center of LV in the world LV)
   // Not yet implemented
   ESMagneticField(G4ThreeVector offset);   

   ESMagneticField();   
   virtual ~ESMagneticField();  
      
   virtual void GetFieldValue(const G4double point[4], G4double* bfield) const;
      
   // Get the field manager
   G4FieldManager *GetFieldManager() const; 
    
private:
   // Field manager 
   G4FieldManager *fFieldManager;
   
   // Magnetic Field information
   TH2D *fHisBx;
   
   G4ThreeVector fOffset;
   
   // Magnetic field region used with global cordinates
   // It is used only for world logical volume
   // In case of you use for daughter volume of the world,
   // Please don't forget to convert to local cordinates.
   G4double fXMax;
   G4double fXMin;
   G4double fYMax;
   G4double fYMin;
   G4double fZMax;
   G4double fZMin;
};

#endif
