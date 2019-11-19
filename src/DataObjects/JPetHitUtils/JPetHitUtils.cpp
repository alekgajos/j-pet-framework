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
 *  @file JPetHitUtils.cpp
 */

#include "JPetHitUtils/JPetHitUtils.h"
#include <limits>

const double JPetHitUtils::kUnset = -std::numeric_limits<float>::infinity();

double JPetHitUtils::getTimeDiffAtThr(const JPetHit& hit, int thr) {
  auto lead_times_A = hit.getSignalA().getRecoSignal().getRawSignal()
    .getTimesVsThresholdNumber(JPetSigCh::Leading);
  auto lead_times_B = hit.getSignalB().getRecoSignal().getRawSignal()
    .getTimesVsThresholdNumber(JPetSigCh::Leading);
  if(lead_times_B.count(thr) > 0 && lead_times_A.count(thr) > 0) {
    return lead_times_A[thr] - lead_times_B[thr];
  }
  return kUnset;
}

double JPetHitUtils::getTimeAtThr(const JPetHit& hit, int thr) {
  auto lead_times_A = hit.getSignalA().getRecoSignal().getRawSignal()
    .getTimesVsThresholdNumber(JPetSigCh::Leading);
  auto lead_times_B = hit.getSignalB().getRecoSignal().getRawSignal()
    .getTimesVsThresholdNumber(JPetSigCh::Leading);
  if(lead_times_B.count(thr) > 0 && lead_times_A.count(thr) > 0) {
    return 0.5 * (lead_times_A[thr] + lead_times_B[thr]);
  }
  return kUnset;
}