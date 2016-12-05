#ifndef ESExitHit_h
#define ESExitHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"


class ESExitHit : public G4VHit
{
public:
   ESExitHit();
   virtual ~ESExitHit();
   ESExitHit(const ESExitHit &right);
   const ESExitHit &operator=(const ESExitHit &right);
   int operator==(const ESExitHit &right) const;

   inline void *operator new(size_t);
   inline void  operator delete(void *);

   // add setter/getter methods
   void SetTrackID(G4int id) {fTrackID = id;};
   G4int GetTrackID() {return fTrackID;};

   void SetPDGCode(G4int code) {fPDGCode = code;};
   G4int GetPDGCode() {return fPDGCode;};

   void SetKineticEnergy(G4double ene) {fKineticEnergy = ene;};
   G4double GetKineticEnergy() {return fKineticEnergy;};

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
   G4ThreeVector fPosition;
   G4ThreeVector fMomentum;
   G4int fIsLastStep;
   G4String fVolumeName;
   G4String fVertexName;
};

typedef G4THitsCollection<ESExitHit> ESExitHitsCollection;

extern G4ThreadLocal G4Allocator<ESExitHit> *ESExitHitAllocator;

inline void *ESExitHit::operator new(size_t)
{
   if (!ESExitHitAllocator)
      ESExitHitAllocator = new G4Allocator<ESExitHit>;
   return (void *)ESExitHitAllocator->MallocSingle();
}

inline void ESExitHit::operator delete(void *hit)
{
   ESExitHitAllocator->FreeSingle((ESExitHit *) hit);
}

#endif
