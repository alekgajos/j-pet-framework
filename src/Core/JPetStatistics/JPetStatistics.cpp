/**
 *  @copyright Copyright 2018 The J-PET Framework Authors. All rights reserved.
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
 *  @file JPetStatistics.cpp
 */

#include "JPetStatistics/JPetStatistics.h"

ClassImp(JPetStatistics);

JPetStatistics::JPetStatistics() { ; }
JPetStatistics::JPetStatistics(const JPetStatistics& old)
{
  fStats.AddAll(old.getStatsTable());
  fCounters = old.fCounters;
}

JPetStatistics::~JPetStatistics() { fStats.Clear("nodelete"); }

void JPetStatistics::createHistogram(TObject* object) { fStats.Add(object); }

void JPetStatistics::createObject(TObject* object) { fStats.Add(object); }

void JPetStatistics::createHistogramWithAxes(TObject* object, TString xAxisName, TString yAxisName, TString zAxisName) 
{ 
  TClass *cl = object->IsA();
  if( cl->InheritsFrom("TH1D") )
  {
    TH1D* tempHisto = dynamic_cast<TH1D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
  }
  else if( cl->InheritsFrom("TH2D") )
  {
    TH2D* tempHisto = dynamic_cast<TH2D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
  }
  else if( cl->InheritsFrom("TH3D") )
  {
    TH3D* tempHisto = dynamic_cast<TH3D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
    tempHisto->GetZaxis()->SetTitle(zAxisName);
  }
  fStats.Add(object);
}

void JPetStatistics::createHistogramWithCustomAxes(TObject* object, std::vector<std::vector<std::string>> binLabels, 
                                                   std::vector<std::vector<unsigned>> binNumbers, TString xAxisName, 
                                                   TString yAxisName, TString zAxisName)
{
  TClass *cl = object->IsA();
  if( cl->InheritsFrom("TH1D") )
  {
    TH1D* tempHisto = dynamic_cast<TH1D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
    if( binLabels.size() > 0 && binNumbers.size() > 0 )
    {
      for( unsigned i=0; i<binLabels[0].size() || i<binNumbers[0].size(); i++ )
        tempHisto->GetXaxis()->SetBinLabel(binNumbers[0][i],binLabels[0][i].c_str());
    }
    else
      writeError(tempHisto->GetName(), " had empty custom names of axis or bin numbers" );
  }
  else if( cl->InheritsFrom("TH2D") )
  {
    TH2D* tempHisto = dynamic_cast<TH2D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
    if( binLabels.size() > 0 && binNumbers.size() > 0 )
    {
      for( unsigned i=0; i<binLabels[0].size() || i<binNumbers[0].size(); i++ )
        tempHisto->GetXaxis()->SetBinLabel(binNumbers[0][i],binLabels[0][i].c_str());
      if( binLabels.size() > 1 && binNumbers.size() > 1 )
      {
        for( unsigned j=0; j<binLabels[1].size() || j<binNumbers[1].size(); j++ )
          tempHisto->GetYaxis()->SetBinLabel(binNumbers[1][j],binLabels[1][j].c_str());
      }
    }
    else
      writeError(tempHisto->GetName(), " had empty custom names of axis or bin numbers" );
  }
  else if( cl->InheritsFrom("TH3D") )
  {
    TH3D* tempHisto = dynamic_cast<TH3D*>(object);
    tempHisto->GetXaxis()->SetTitle(xAxisName);
    tempHisto->GetYaxis()->SetTitle(yAxisName);
    tempHisto->GetZaxis()->SetTitle(zAxisName);
    if( binLabels.size() > 0 && binNumbers.size() > 0 )
    {
      for( unsigned i=0; i<binLabels[0].size() || i<binNumbers[0].size(); i++ )
        tempHisto->GetXaxis()->SetBinLabel(binNumbers[0][i],binLabels[0][i].c_str());
      if( binLabels.size() > 1 && binNumbers.size() > 1 )
      {
        for( unsigned j=0; j<binLabels[1].size() || j<binNumbers[1].size(); j++ )
          tempHisto->GetYaxis()->SetBinLabel(binNumbers[1][j],binLabels[1][j].c_str());
        if( binLabels.size() > 2 && binNumbers.size() > 2 )
        {
          for( unsigned k=0; k<binLabels[2].size() || k<binNumbers[2].size(); k++ )
            tempHisto->GetZaxis()->SetBinLabel(binNumbers[2][k],binLabels[2][k].c_str());
        }
      }
    }
    else
      writeError(tempHisto->GetName(), " had empty custom names of axis or bin numbers" );
  }
  fStats.Add(object);  
}

void JPetStatistics::createGraph(TObject* object) { fStats.Add(object); }

void JPetStatistics::createCanvas(TObject* object) { fStats.Add(object); }

void JPetStatistics::fillHistogram(const char* name, double xValue, doubleCheck yValue, doubleCheck zValue)
{
  TObject *tempObject = getObject<TObject>(name);
  if( !tempObject )
  {
    writeError(name, " does not exist" );
    return;
  }
  TClass *cl = tempObject->IsA();
  if( cl->InheritsFrom("TH1D") )
  {
    TH1D* tempHisto = dynamic_cast<TH1D*>(tempObject);
    tempHisto->Fill(xValue);
  }
  else if( cl->InheritsFrom("TH2D") )
  {
    TH2D* tempHisto = dynamic_cast<TH2D*>(tempObject);
    if(yValue.isChanged)
        tempHisto->Fill(xValue, yValue.value);
    else
        writeError(name, " does not received argument for Y axis" );
  }
  else if( cl->InheritsFrom("TH3D") )
  {
    TH3D* tempHisto = dynamic_cast<TH3D*>(tempObject);
    if(zValue.isChanged)
        tempHisto->Fill(xValue, yValue.value, zValue.value);
    else if(yValue.isChanged)
        writeError(name, " does not received argument for Y and Z axis" );
    else
        writeError(name, " does not received argument for Z axis" );
  }  
}

TEfficiency* JPetStatistics::getEffiHisto(const char* name) { return getObject<TEfficiency>(name); }

TH1F* JPetStatistics::getHisto1D(const char* name) { return getObject<TH1F>(name); }

TH2F* JPetStatistics::getHisto2D(const char* name) { return getObject<TH2F>(name); }

TH3F* JPetStatistics::getHisto3D(const char* name) { return getObject<TH3F>(name); }

TGraph* JPetStatistics::getGraph(const char* name) { return getObject<TGraph>(name); }

TCanvas* JPetStatistics::getCanvas(const char* name) { return getObject<TCanvas>(name); }

void JPetStatistics::createCounter(const char* name) { fCounters[name] = 0.0; }

double& JPetStatistics::getCounter(const char* name) { return fCounters[name]; }

const THashTable* JPetStatistics::getStatsTable() const { return &fStats; }

void JPetStatistics::writeError(const char* nameOfHistogram, const char* messageEnd )
{
  ERROR(std::string("Histogram with name ") + std::string(nameOfHistogram) + std::string(messageEnd) );
}
