#ifndef HistoManager_h
#define HistoManager_h 1

#include "g4root.hh"
#include "globals.hh"
#include <vector>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager {
public:
  HistoManager();
  ~HistoManager();

  void Book();
  void Save();

  void FillHisto(const int id, const double e, const double weight = 1.0);

  void FillNtuple(const double, const double, const double, const double,
                  const double, const double, const int, const int, const int,
                  const int, const std::vector<double>&,
                  const std::vector<double>&, const std::vector<int>&,
                  const std::vector<int>&);
  void setSiPMIntegral(const std::vector<double>&);
  void setSiPMToa(const std::vector<double>&);
  void setSiPMPhotoelectrons(const std::vector<int>&);
  void setSiPMPhotons(const std::vector<int>&);
  void PrintStatistic();

private:
  std::vector<double> fIntegralSiPM;
  std::vector<double> fToaSiPM;
  std::vector<int> fPhotoelectronSiPM;
  std::vector<int> fPhotonsSiPM;
  bool fFactoryOn;
  G4AnalysisManager* fAnalysisManager;
};

inline void HistoManager::setSiPMIntegral(const std::vector<double>& x) {
  fIntegralSiPM = x;
}
inline void HistoManager::setSiPMToa(const std::vector<double>& x) {
  fToaSiPM = x;
}
inline void HistoManager::setSiPMPhotoelectrons(const std::vector<int>& x) {
  fPhotoelectronSiPM = x;
}
inline void HistoManager::setSiPMPhotons(const std::vector<int>& x) {
  fPhotonsSiPM = x;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
