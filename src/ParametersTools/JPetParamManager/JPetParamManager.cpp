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
 *  @file JPetParamManager.cpp
 */

#include "JPetParamGetterAscii/JPetParamGetterAscii.h"
#include "JPetOptionsTools/JPetOptionsTools.h"
#include "JPetParamManager/JPetParamManager.h"
#include <boost/property_tree/xml_parser.hpp>
#include <TFile.h>

std::shared_ptr<JPetParamManager> JPetParamManager::generateParamManager(
  const std::map<std::string, boost::any>& options)
{
  using namespace jpet_options_tools;
  if (isLocalDB(options)) {
    std::set<ParamObjectType> expectMissing;
    if (FileTypeChecker::getInputFileType(options) == FileTypeChecker::kScope) {
      expectMissing.insert(ParamObjectType::kSetup);
      expectMissing.insert(ParamObjectType::kLayer);
      expectMissing.insert(ParamObjectType::kChannel);
    }
    return std::make_shared<JPetParamManager>(new JPetParamGetterAscii(getLocalDB(options)), expectMissing);
  } else {
    ERROR("No local database file found.");
    return std::make_shared<JPetParamManager>();
  }
}

JPetParamManager::~JPetParamManager()
{
  if (fBank) {
    delete fBank;
    fBank = 0;
  }
  if (fParamGetter) {
    delete fParamGetter;
    fParamGetter = 0;
  }
}

std::map<int, JPetSetup*>& JPetParamManager::getSetups(const int runID)
{
  return getSetupFactory(runID).getSetups();
}

JPetSetupFactory& JPetParamManager::getSetupFactory(const int runID)
{
  if (fSetupFactories.count(runID) == 0) {
    fSetupFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID)
    );
  }
  return fSetupFactories.at(runID);
}

std::map<int, JPetLayer*>& JPetParamManager::getLayers(const int runID)
{
  return getLayerFactory(runID).getLayers();
}

JPetLayerFactory& JPetParamManager::getLayerFactory(const int runID)
{
  if (fLayerFactories.count(runID) == 0) {
    fLayerFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID, getSetupFactory(runID))
    );
  }
  return fLayerFactories.at(runID);
}

std::map<int, JPetSlot*>& JPetParamManager::getSlots(const int runID)
{
  return getSlotFactory(runID).getSlots();
}

JPetSlotFactory& JPetParamManager::getSlotFactory(const int runID)
{
  if (fSlotFactories.count(runID) == 0) {
    fSlotFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID, getLayerFactory(runID))
    );
  }
  fSlotFactories.at(runID);
  return fSlotFactories.at(runID);
}

std::map<int, JPetScin*>& JPetParamManager::getScins(const int runID)
{
  return getScinFactory(runID).getScins();
}

JPetScinFactory& JPetParamManager::getScinFactory(const int runID)
{
  if (fScinFactories.count(runID) == 0) {
    fScinFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID, getSlotFactory(runID))
    );
  }
  return fScinFactories.at(runID);
}

std::map<int, JPetPM*>& JPetParamManager::getPMs(const int runID)
{
  return getPMFactory(runID).getPMs();
}

JPetPMFactory& JPetParamManager::getPMFactory(const int runID)
{
  if (fPMFactories.count(runID) == 0) {
    fPMFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID, getScinFactory(runID))
    );
  }
  return fPMFactories.at(runID);
}

std::map<int, JPetChannel*>& JPetParamManager::getChannels(const int runID)
{
  return getChannelFactory(runID).getChannels();
}

JPetChannelFactory& JPetParamManager::getChannelFactory(const int runID)
{
  if (fChannelFactories.count(runID) == 0) {
    fChannelFactories.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(runID),
      std::forward_as_tuple(*fParamGetter, runID, getPMFactory(runID))
    );
  }
  return fChannelFactories.at(runID);
}

void JPetParamManager::fillParameterBank(const int runID)
{
  if (fBank) {
    delete fBank;
    fBank = 0;
  }
  fBank = new JPetParamBank();
  if (!fExpectMissing.count(ParamObjectType::kSetup)) {
    for (auto& setup_p : getSetups(runID)) {
      auto& setup = *setup_p.second;
      fBank->addSetup(setup);
    }
  }
  if (!fExpectMissing.count(ParamObjectType::kLayer)) {
    for (auto& layer_p : getLayers(runID)) {
      auto& layer = *layer_p.second;
      fBank->addLayer(layer);
      fBank->getLayer(layer.getID()).setSetup(fBank->getSetup(layer.getSetup().getID()));
    }
  }
  if (!fExpectMissing.count(ParamObjectType::kSlot)) {
    for (auto& slot_p : getSlots(runID)) {
      auto& slot = *slot_p.second;
      fBank->addSlot(slot);
      fBank->getSlot(slot.getID()).setLayer(fBank->getLayer(slot.getLayer().getID()));
    }
  }
  if (!fExpectMissing.count(ParamObjectType::kScin)) {
    for (auto& scin_p : getScins(runID)) {
      auto& scin = *scin_p.second;
      fBank->addScin(scin);
      fBank->getScin(scin.getID()).setSlot(fBank->getSlot(scin.getSlot().getID()));
    }
  }
  if (!fExpectMissing.count(ParamObjectType::kPM)) {
    for (auto& pm_p : getPMs(runID)) {
      auto& pm = *pm_p.second;
      fBank->addPM(pm);
      fBank->getPM(pm.getID()).setScin(fBank->getScin(pm.getScin().getID()));
    }
  }
  if (!fExpectMissing.count(ParamObjectType::kChannel)) {
    for (auto& channel_p : getChannels(runID)) {
      auto& channel = *channel_p.second;
      fBank->addChannel(channel);
      fBank->getChannel(channel.getID()).setPM(fBank->getPM(channel.getPM().getID())
      );
    }
  }
}

bool JPetParamManager::readParametersFromFile(JPetReader* reader)
{
  assert(reader);
  if (!reader->isOpen()) {
    ERROR("Cannot read parameters from file. The provided JPetReader is closed.");
    return false;
  }
  fBank = static_cast<JPetParamBank*>(reader->getObjectFromFile("ParamBank"));
  if (!fBank) return false;
  return true;
}

bool JPetParamManager::saveParametersToFile(JPetWriter* writer)
{
  assert(writer);
  if (!writer->isOpen()) {
    ERROR("Could not write parameters to file. The provided JPetWriter is closed.");
    return false;
  }
  writer->writeObject(fBank, "ParamBank");
  return true;
}

bool JPetParamManager::readParametersFromFile(std::string filename)
{
  TFile file(filename.c_str(), "READ");
  if (!file.IsOpen()) {
    ERROR("Could not read from file.");
    return false;
  }
  fBank = static_cast<JPetParamBank*>(file.Get("ParamBank"));
  if (!fBank) return false;
  return true;
}

const JPetParamBank& JPetParamManager::getParamBank() const
{
  DEBUG("getParamBank() from JPetParamManager");
  static JPetParamBank DummyResult(true);
  if (fBank) return *fBank;
  else return DummyResult;
}

bool JPetParamManager::saveParametersToFile(std::string filename)
{
  TFile file(filename.c_str(), "UPDATE");
  if (!file.IsOpen()) {
    ERROR("Could not write to file.");
    return false;
  }
  file.cd();
  assert(fBank);
  file.WriteObject(fBank, "ParamBank;1");
  return true;
}

void JPetParamManager::clearParameters()
{
  assert(fBank);
  fBank->clear();
}