#ifndef ESHit_h
#define ESHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"


class ESHit : public G4VHit
{
public:
   ESHit();
   virtual ~ESHit();
   ESHit(const ESHit &right);
   const ESHit &operator=(const ESHit &right);
   int operator==(const ESHit &right) const;

   inline void *operator new(size_t);
   inline void  operator delete(void *);

   // add setter/getter methods
   void SetTrackID(G4int id) {fTrackID = id;};
   G4int GetTrackID() {return fTrackID;};

   void SetPDGCode(G4int code) {fPDGCode = code;};
   G4int GetPDGCode() {return fPDGCode;};

   void SetKineticEnergy(G4double ene) {fKineticEnergy = ene;};
   G4double GetKineticEnergy() {return fKineticEnergy;};

   void SetDepositEnergy(G4double ene) {fDepositEnergy = ene;};
   G4double GetDepositEnergy() {return fDepositEnergy;};

   void SetPosition(G4ThreeVector pos) {fPosition = pos;};
   G4ThreeVector GetPosition() {return fPosition;};
   
   void SetMomentum(G4ThreeVector p) {fMomentum = p;};
   G4ThreeVector GetMomentum() {return fMomentum;};
   
   void SetIsLast(G4int flag) {fIsLastStep = flag;};
   G4int GetIsLast() {return fIsLastStep;};
   
   void SetVolumeName(G4String volumeName) {fVolumeName = volumeName;};
   G4String GetVolumeName() {return fVolumeName;};

   void SetVertexName(G4String vertexName) {fVertexName = vertexName;};
   G4String GetVertexName() {return fVertexName;};

private:
   G4int fTrackID;
   G4int fPDGCode;
   G4double fKineticEnergy;
   G4double fDepositEnergy;
   G4ThreeVector fPosition;
   G4ThreeVector fMomentum;
   G4int fIsLastStep;
   G4String fVolumeName;
   G4String fVertexName;
};

typedef G4THitsCollection<ESHit> ESHitsCollection;

extern G4ThreadLocal G4Allocator<ESHit> *ESHitAllocator;

inline void *ESHit::operator new(size_t)
{
   if (!ESHitAllocator)
      ESHitAllocator = new G4Allocator<ESHit>;
   return (void *)ESHitAllocator->MallocSingle();
}

inline void ESHit::operator delete(void *hit)
{
   ESHitAllocator->FreeSingle((ESHit *) hit);
}

#endif
