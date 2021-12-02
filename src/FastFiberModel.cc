#include "FastFiberModel.hh"

#include "G4GeometryTolerance.hh"
#include "G4OpProcessSubType.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"

FastFiberData::FastFiberData(G4int id, G4double en, G4double globTime,
                             G4double path, G4ThreeVector pos,
                             G4ThreeVector mom, G4ThreeVector pol,
                             G4int status) {
  trackID = id;
  kineticEnergy = en;
  globalTime = globTime;
  pathLength = path;
  globalPosition = pos;
  momentumDirection = mom;
  polarization = pol;
  mOpBoundaryStatus = status;
  mOpAbsorptionNumIntLenLeft = DBL_MAX;
  mOpWLSNumIntLenLeft = DBL_MAX;
  mStepLengthInterval = 0.;
}

// FastFiberData& FastFiberData::operator=(const FastFiberData& right) {
//   trackID = right.trackID;
//   kineticEnergy = right.kineticEnergy;
//   globalTime = right.globalTime;
//   pathLength = right.pathLength;
//   globalPosition = right.globalPosition;
//   momentumDirection = right.momentumDirection;
//   polarization = right.polarization;
//   mOpBoundaryStatus = right.mOpBoundaryStatus;
//   mOpAbsorptionNumIntLenLeft = right.mOpAbsorptionNumIntLenLeft;
//   mOpWLSNumIntLenLeft = right.mOpWLSNumIntLenLeft;
//   mStepLengthInterval = right.mStepLengthInterval;
//
//   return *this;
// }

G4bool FastFiberData::checkRepetitive(const FastFiberData theData,
                                      G4bool checkInterval) {
  if (this->trackID != theData.trackID)
    return false;
  if (this->mOpBoundaryStatus != theData.mOpBoundaryStatus)
    return false;
  if (checkInterval &&
      std::abs(this->mStepLengthInterval - theData.mStepLengthInterval) >
          G4GeometryTolerance::GetInstance()->GetSurfaceTolerance())
    return false;
  return true;
}

FastFiberModel::FastFiberModel(G4String name, G4Region *envelope)
    : G4VFastSimulationModel(name, envelope),
      mDataPrevious(0, 0., 0., 0., G4ThreeVector(0), G4ThreeVector(0),
                    G4ThreeVector(0)),
      mDataCurrent(0, 0., 0., 0., G4ThreeVector(0), G4ThreeVector(0),
                   G4ThreeVector(0)) {
  pOpBoundaryProc = nullptr;
  pOpAbsorption = nullptr;
  pOpWLS = nullptr;
  fProcAssigned = false;
  fSafety = 2;
  mNtransport = 0.;
  mTransportUnit = 0.;
  mFiberPos = G4ThreeVector(0);
  mFiberAxis = G4ThreeVector(0);
  fKill = false;
  fTransported = false;
  fSwitch = true;
  fVerbose = 0;

  DefineCommands();
}

FastFiberModel::~FastFiberModel() {}

G4bool FastFiberModel::IsApplicable(const G4ParticleDefinition &type) {
  return &type == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FastFiberModel::ModelTrigger(const G4FastTrack &fasttrack) {
  if (!fSwitch)
    return false; // turn on/off the model

  const G4Track *track = fasttrack.GetPrimaryTrack();

  // make sure that the track does not get absorbed after transportation, as
  // number of interaction length left is reset when doing transportation
  if (!checkNILL())
    return true; // track is already transported but did not pass NILL check,
                 // attempt to reset NILL
  if (fTransported)
    return false; // track is already transported and did pass NILL check,
                  // nothing to do

  if (!checkTotalInternalReflection(track))
    return false; // nothing to do if the track has no repetitive total internal
                  // reflection

  const auto theTouchable = track->GetTouchableHandle();
  const auto solid = theTouchable->GetSolid();

  if (solid->GetEntityType() != "G4Tubs")
    return false; // only works for G4Tubs at the moment

  // if (fVerbose > 0)
  //   print(); // at this point, the track should have passed all prerequisites
  //   before entering computationally heavy
  //            // operations

  const G4Tubs *tubs = static_cast<G4Tubs *>(solid);
  const G4double fiberLen = 2. * tubs->GetZHalfLength();

  mFiberPos =
      theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(
          G4ThreeVector(0., 0., 0.));
  mFiberAxis =
      theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(
          G4ThreeVector(0., 0., 1.));

  if (mFiberAxis.dot(mDataCurrent.momentumDirection) *
          mFiberAxis.dot(mDataPrevious.momentumDirection) <
      0) { // different propagation direction (e.g. mirror)
    reset();

    return false;
  }

  const auto delta = mDataCurrent.globalPosition - mDataPrevious.globalPosition;
  mTransportUnit = delta.dot(mFiberAxis);

  // estimate the number of expected total internal reflections before reaching
  // fiber end
  const auto fiberEnd = (mTransportUnit > 0.)
                            ? mFiberPos + mFiberAxis * fiberLen / 2.
                            : mFiberPos - mFiberAxis * fiberLen / 2.;
  const auto toEnd = fiberEnd - mDataCurrent.globalPosition;
  const G4double toEndAxis = toEnd.dot(mFiberAxis);
  const G4double maxTransport = std::floor(toEndAxis / mTransportUnit);
  mNtransport = maxTransport - fSafety;

  if (mNtransport < 1.)
    return false; // require at least n = fSafety of total internal reflections
                  // at the end

  if (checkAbsorption(mDataPrevious.GetWLSNILL(), mDataCurrent.GetWLSNILL()))
    return false; // do nothing if WLS happens before reaching fiber end
  if (checkAbsorption(mDataPrevious.GetAbsorptionNILL(),
                      mDataCurrent.GetAbsorptionNILL()))
    fKill = true; // absorbed before reaching fiber end

  return true;
}

void FastFiberModel::DoIt(const G4FastTrack &fasttrack, G4FastStep &faststep) {
  const auto track = fasttrack.GetPrimaryTrack();

  if (fKill) { // absorption
    faststep.ProposeTotalEnergyDeposited(track->GetKineticEnergy());
    faststep.KillPrimaryTrack();

    return;
  }

  if (fTransported)
    return; // reset NILL if the track did not meet NILL check

  const double timeUnit = mDataCurrent.globalTime - mDataPrevious.globalTime;
  const auto posShift =
      mTransportUnit * mNtransport *
      mFiberAxis; // #TODO apply shift for xy direction as well
  const double timeShift = timeUnit * mNtransport;

  faststep.ProposePrimaryTrackFinalPosition(track->GetPosition() + posShift,
                                            false);
  faststep.ProposePrimaryTrackFinalTime(track->GetGlobalTime() + timeShift);
  faststep.ProposePrimaryTrackFinalKineticEnergy(track->GetKineticEnergy());
  faststep.ProposePrimaryTrackFinalMomentumDirection(
      track->GetMomentumDirection(), false);
  faststep.ProposePrimaryTrackFinalPolarization(track->GetPolarization(),
                                                false);
  fTransported = true;

  return;
}

G4bool FastFiberModel::checkTotalInternalReflection(const G4Track *track) {
  if (!fProcAssigned)
    setPostStepProc(track); // locate OpBoundaryProcess only once
  if (track->GetTrackStatus() == fStopButAlive ||
      track->GetTrackStatus() == fStopAndKill)
    return false;
  if (mDataCurrent.trackID != track->GetTrackID())
    reset(); // reset when moving to the next track

  // accumulate step length
  mDataCurrent.AddStepLengthInterval(track->GetStepLength());

  const G4int theStatus = pOpBoundaryProc->GetStatus();

  // if (fVerbose > 1) {
  //   G4cout << "FastFiberModel::checkTotalInternalReflection | TrackID = " <<
  //   std::setw(4) << track->GetTrackID(); G4cout << " |
  //   G4OpBoundaryProcessStatus = " << std::setw(2) << theStatus; G4cout << " |
  //   StepLength = " << std::setw(9) << track->GetStepLength() << G4endl;
  // }

  // skip exceptional iteration with FresnelReflection
  if (theStatus == G4OpBoundaryProcessStatus::FresnelReflection)
    mDataCurrent.SetOpBoundaryStatus(theStatus);

  // some cases have a status StepTooSmall when the reflection happens between
  // the boundary of cladding & outer volume (outside->cladding) since the outer
  // volume is not a G4Region
  if (theStatus == G4OpBoundaryProcessStatus::TotalInternalReflection ||
      theStatus == G4OpBoundaryProcessStatus::StepTooSmall) {
    if (theStatus !=
        G4OpBoundaryProcessStatus::
            TotalInternalReflection) { // skip StepTooSmall if the track already
                                       // has TotalInternalReflection
      if (mDataCurrent.GetOpBoundaryStatus() ==
          G4OpBoundaryProcessStatus::TotalInternalReflection)
        return false;
      if (mDataPrevious.GetOpBoundaryStatus() ==
          G4OpBoundaryProcessStatus::TotalInternalReflection)
        return false;
    }

    const G4int trackID = track->GetTrackID();
    const G4double kineticEnergy = track->GetKineticEnergy();
    const G4double globalTime = track->GetGlobalTime();
    const G4double pathLength = track->GetStepLength();
    const G4ThreeVector globalPosition = track->GetPosition();
    const G4ThreeVector momentumDirection = track->GetMomentumDirection();
    const G4ThreeVector polarization = track->GetPolarization();

    auto candidate = FastFiberData(trackID, kineticEnergy, globalTime,
                                   pathLength, globalPosition,
                                   momentumDirection, polarization, theStatus);
    if (pOpAbsorption != nullptr)
      candidate.SetAbsorptionNILL(
          pOpAbsorption->GetNumberOfInteractionLengthLeft());
    if (pOpWLS != nullptr)
      candidate.SetWLSNILL(pOpWLS->GetNumberOfInteractionLengthLeft());

    G4bool repetitive = false;
    if (candidate.checkRepetitive(mDataCurrent, false) &&
        mDataCurrent.checkRepetitive(mDataPrevious))
      repetitive = true;

    mDataPrevious = mDataCurrent;
    mDataCurrent = candidate;
    return repetitive;
  }

  return false;
}

G4bool FastFiberModel::checkAbsorption(const G4double prevNILL,
                                       const G4double currentNILL) {
  if (prevNILL < 0. || currentNILL < 0.)
    return false; // the number of interaction length left has to be reset
  if (prevNILL == currentNILL)
    return false; // no absorption
  if (prevNILL == DBL_MAX || currentNILL == DBL_MAX)
    return false; // NILL is re-initialized

  const G4double deltaNILL = prevNILL - currentNILL;

  if (currentNILL - deltaNILL * (mNtransport + fSafety) < 0.)
    return true; // absorbed before reaching fiber end

  return false;
}

G4bool FastFiberModel::checkNILL() {
  if (!fTransported)
    return true; // do nothing if the track is not already transported

  G4double wlsNILL = DBL_MAX;
  G4double absorptionNILL = DBL_MAX;

  if (pOpWLS != nullptr) {
    wlsNILL = pOpWLS->GetNumberOfInteractionLengthLeft();
    if (mDataPrevious.GetWLSNILL() == DBL_MAX ||
        mDataCurrent.GetWLSNILL() == DBL_MAX)
      return true; // NILL is re-initialized
  }

  if (pOpAbsorption != nullptr) {
    absorptionNILL = pOpAbsorption->GetNumberOfInteractionLengthLeft();
    if (mDataPrevious.GetAbsorptionNILL() == DBL_MAX ||
        mDataCurrent.GetAbsorptionNILL() == DBL_MAX)
      return true; // NILL is re-initialized
  }

  if (wlsNILL < 0. || absorptionNILL < 0.)
    return true; // let GEANT4 to reset them

  const G4double deltaWlsNILL =
      mDataPrevious.GetWLSNILL() - mDataCurrent.GetWLSNILL();
  const G4double deltaAbsorptionNILL =
      mDataPrevious.GetAbsorptionNILL() - mDataCurrent.GetAbsorptionNILL();

  const G4double finalWlsNILL = wlsNILL - deltaWlsNILL * fSafety;
  const G4double finalAbsorptionNILL =
      absorptionNILL - deltaAbsorptionNILL * fSafety;

  // prevent double counting of the probability of getting absorbed (which
  // already estimated before transportation) reset NILL again
  if (finalWlsNILL < 0. || finalAbsorptionNILL < 0.)
    return false;

  // NILL check passed (after trasportation)
  reset();

  return true;
}

G4ThreeVector FastFiberModel::getXYcomponent(const FastFiberData &data) {
  const auto relativePos = data.globalPosition - mFiberPos;
  const auto zcomponent = relativePos.dot(mFiberAxis) * mFiberAxis;
  return (relativePos - zcomponent);
}

void FastFiberModel::setPostStepProc(const G4Track *track) {
  G4ProcessManager *pm = track->GetDefinition()->GetProcessManager();
  auto postStepProcessVector = pm->GetPostStepProcessVector();

  for (int np = 0; np < postStepProcessVector->entries(); np++) {
    auto theProcess = (*postStepProcessVector)[np];

    auto theType = theProcess->GetProcessType();

    if (theType != fOptical)
      continue;

    if (theProcess->GetProcessSubType() == G4OpProcessSubType::fOpBoundary)
      pOpBoundaryProc = dynamic_cast<G4OpBoundaryProcess *>(theProcess);
    else if (theProcess->GetProcessSubType() ==
             G4OpProcessSubType::fOpAbsorption)
      pOpAbsorption = dynamic_cast<G4OpAbsorption *>(theProcess);
    else if (theProcess->GetProcessSubType() == G4OpProcessSubType::fOpWLS)
      pOpWLS = dynamic_cast<G4OpWLS *>(theProcess);
  }

  fProcAssigned = true;

  return;
}

void FastFiberModel::reset() {
  mNtransport = 0.;
  mTransportUnit = 0.;
  mFiberPos = G4ThreeVector(0);
  mFiberAxis = G4ThreeVector(0);
  fKill = false;
  fTransported = false;
  mDataCurrent.SetOpBoundaryStatus(G4OpBoundaryProcessStatus::Undefined);
  mDataPrevious.SetOpBoundaryStatus(G4OpBoundaryProcessStatus::Undefined);
  mDataCurrent.SetWLSNILL(DBL_MAX);
  mDataCurrent.SetAbsorptionNILL(DBL_MAX);
  mDataPrevious.SetWLSNILL(DBL_MAX);
  mDataPrevious.SetAbsorptionNILL(DBL_MAX);
}

void FastFiberModel::print() {
  if (fVerbose > 1) {
    G4cout << G4endl;

    G4cout << "mDataPrevious.trackID = " << mDataPrevious.trackID;
    G4cout << " | .mOpBoundaryStatus = " << std::setw(4)
           << mDataPrevious.GetOpBoundaryStatus();
    G4cout << " | .mStepLengthInterval = "
           << mDataPrevious.GetStepLengthInterval() << G4endl;

    if (fVerbose > 2) {
      G4cout << "  | globalPosition    = (" << std::setw(9)
             << mDataPrevious.globalPosition.x();
      G4cout << "," << std::setw(9) << mDataPrevious.globalPosition.y();
      G4cout << "," << std::setw(9) << mDataPrevious.globalPosition.z() << ")"
             << G4endl;

      G4cout << "  | momentumDirection = (" << std::setw(9)
             << mDataPrevious.momentumDirection.x();
      G4cout << "," << std::setw(9) << mDataPrevious.momentumDirection.y();
      G4cout << "," << std::setw(9) << mDataPrevious.momentumDirection.z()
             << ")" << G4endl;

      G4cout << "  | polarization      = (" << std::setw(9)
             << mDataPrevious.polarization.x();
      G4cout << "," << std::setw(9) << mDataPrevious.polarization.y();
      G4cout << "," << std::setw(9) << mDataPrevious.polarization.z() << ")"
             << G4endl;

      G4cout << "  | globalTime        =  " << std::setw(9)
             << mDataPrevious.globalTime << G4endl;
      G4cout << "  | WLSNILL           =  " << std::setw(9)
             << mDataPrevious.GetWLSNILL() << G4endl;
      G4cout << "  | AbsorptionNILL    =  " << std::setw(9)
             << mDataPrevious.GetAbsorptionNILL() << G4endl;
    }

    G4cout << "mDataCurrent.trackID  = " << mDataCurrent.trackID;
    G4cout << " | .mOpBoundaryStatus  = " << std::setw(4)
           << mDataCurrent.GetOpBoundaryStatus() << G4endl;

    if (fVerbose > 2) {
      G4cout << "  | globalPosition    = (" << std::setw(9)
             << mDataCurrent.globalPosition.x();
      G4cout << "," << std::setw(9) << mDataCurrent.globalPosition.y();
      G4cout << "," << std::setw(9) << mDataCurrent.globalPosition.z() << ")"
             << G4endl;

      G4cout << "  | momentumDirection = (" << std::setw(9)
             << mDataCurrent.momentumDirection.x();
      G4cout << "," << std::setw(9) << mDataCurrent.momentumDirection.y();
      G4cout << "," << std::setw(9) << mDataCurrent.momentumDirection.z() << ")"
             << G4endl;

      G4cout << "  | polarization      = (" << std::setw(9)
             << mDataCurrent.polarization.x();
      G4cout << "," << std::setw(9) << mDataCurrent.polarization.y();
      G4cout << "," << std::setw(9) << mDataCurrent.polarization.z() << ")"
             << G4endl;

      G4cout << "  | globalTime        =  " << std::setw(9)
             << mDataCurrent.globalTime << G4endl;
      G4cout << "  | WLSNILL           =  " << std::setw(9)
             << mDataCurrent.GetWLSNILL() << G4endl;
      G4cout << "  | AbsorptionNILL    =  " << std::setw(9)
             << mDataCurrent.GetAbsorptionNILL() << G4endl;
    }

    G4cout << G4endl;
  }
}

void FastFiberModel::DefineCommands() {
  mMessenger = new G4GenericMessenger(this, "/fastfiber/model/",
                                      "fastfiber model control");
  G4GenericMessenger::Command &safetyCmd = mMessenger->DeclareProperty(
      "safety", fSafety, "min number of total internal reflection");
  safetyCmd.SetParameterName("safety", true);
  safetyCmd.SetDefaultValue("2.");

  G4GenericMessenger::Command &switchCmd =
      mMessenger->DeclareProperty("on", fSwitch, "turn on fastfiber model");
  switchCmd.SetParameterName("on", true);
  switchCmd.SetDefaultValue("True");

  G4GenericMessenger::Command &verboseCmd =
      mMessenger->DeclareProperty("verbose", fVerbose, "verbose level");
  verboseCmd.SetParameterName("verbose", true);
  verboseCmd.SetDefaultValue("0");
}