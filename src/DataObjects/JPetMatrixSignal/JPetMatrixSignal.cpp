/**
 *  @copyright Copyright 2019 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file JPetMatrixSignal.cpp
 */

#include "JPetMatrixSignal/JPetMatrixSignal.h"
#include <vector>

ClassImp(JPetMatrixSignal);

JPetMatrixSignal::JPetMatrixSignal(): fTime(0), fTimeStdDev(0), fTOT(0) {}

JPetMatrixSignal::JPetMatrixSignal(JPetPM::Side side, int scinID):
  fTime(0.0), fTimeStdDev(0.0), fTOT(0.0), fMtxSide(side), fScinID(scinID) {}

JPetMatrixSignal::~JPetMatrixSignal() {}

/**
 * Constructor with is null setting
 */
JPetMatrixSignal::JPetMatrixSignal(bool isNull): fIsNullObject(isNull) {}

bool JPetMatrixSignal::isNullObject() const
{
  return fIsNullObject;
}

float JPetMatrixSignal::getTime() const
{
  return fTime;
}

float JPetMatrixSignal::getTimeStdDev() const
{
  return fTimeStdDev;
}

float JPetMatrixSignal::getTOT() const
{
  return fTOT;
}

bool JPetMatrixSignal::addRawSignal(const JPetRawSignal& rawSignal)
{
  int mtxPos = rawSignal.getPM().getMatrixPosition();
  auto search = fRawSignalsMap.find(mtxPos);
  if(search == fRawSignalsMap.end()){
    fRawSignalsMap[mtxPos] = rawSignal;
    setSignalTimeAndTOT();
    return true;
  } else {
    // There is already a signal from this SiPM in this matrix, not adding
    return false;
  }
}

std::map<int, JPetRawSignal> JPetMatrixSignal::getRawSignals()
{
  return fRawSignalsMap;
}

/**
 * @brief Obtain a reference to the PhotoMultiplier parametric object
 */
int JPetMatrixSignal::getScinID()
{
    return fScinID;
}

void JPetMatrixSignal::setScinID(int scinID)
{
  fScinID = scinID;
}

JPetPM::Side JPetMatrixSignal::getMtxSide()
{
  return fMtxSide;
}

void JPetMatrixSignal::setMtxSide(JPetPM::Side side)
{
  fMtxSide = side;
}

/**
 * Clear the signals values (set all to zero/null)
 */
void JPetMatrixSignal::Clear(Option_t *)
{
  fTime = 0.0;
  fTimeStdDev = 0.0;
  fTOT = 0.0;
  fScinID = -1;
}

/**
 * Setting time of this Matrix Signal as average time of leading SigSch
 * on THR 1 from all Raw Sigs added so far
 */
void JPetMatrixSignal::setSignalTimeAndTOT()
{
  std::vector<double> leadThr1Times;

  // Calculating TOT - sum of all THR in all RawSignals
  // and time as average of times of lead thr1 SigCh
  double tot = 0.0;
  double time = 0.0;
  for(auto element : fRawSignalsMap){
    auto leads = element.second.getPoints(JPetSigCh::Leading, JPetRawSignal::ByThrValue);
    auto trails = element.second.getPoints(JPetSigCh::Trailing, JPetRawSignal::ByThrValue);
    time += leads.at(0).getTime();
    leadThr1Times.push_back(leads.at(0).getTime());
    if(leads.size()==trails.size()){
      for(uint i = 0; i<leads.size(); i++){
        tot += trails.at(i).getTime()-leads.at(i).getTime();
      }
    }
  }
  fTOT = tot;
  fTime = time/((double)fRawSignalsMap.size());

  // Calculating standard deviation of average time
  double timeStdDev = 0;
  for(auto tn : leadThr1Times){
    timeStdDev += TMath::Power(tn-fTime, 2);
  }
  fTimeStdDev = TMath::Sqrt(timeStdDev/((double)leadThr1Times.size()));
}