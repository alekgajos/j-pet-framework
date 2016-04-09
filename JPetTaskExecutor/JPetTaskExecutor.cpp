/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
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
 *  @file JPetTaskExecutor.cpp
 */

#include "JPetTaskExecutor.h"
#include <cassert>
#include "../JPetTaskInterface/JPetTaskInterface.h"
#include "../JPetScopeReader/JPetScopeReader.h"
#include "../JPetTaskLoader/JPetTaskLoader.h"


JPetTaskExecutor::JPetTaskExecutor(TaskGeneratorChain* taskGeneratorChain, int processedFileId, JPetOptions opt) :
  fProcessedFile(processedFileId),
  ftaskGeneratorChain(taskGeneratorChain),
  fOptions(opt)
{
  if (taskGeneratorChain) {
    for (auto taskGenerator : *ftaskGeneratorChain) {
      auto task = taskGenerator();
      task->setParamManager(&fParamManager); // maybe that should not be here
      fTasks.push_back(task);
    }
  } else {
    ERROR("taskGeneratorChain is null while constructing JPetTaskExecutor");
  }
}

void JPetTaskExecutor::process()
{
  processFromCmdLineArgs(fProcessedFile);
  for (auto currentTask = fTasks.begin(); currentTask != fTasks.end(); currentTask++) {

    // ignore the event range options for all but the first processed task
    if(currentTask != fTasks.begin()){
      fOptions.resetEventRange();
    }

    INFO(Form("Starting task: %s", dynamic_cast<JPetTaskLoader*>(*currentTask)->getSubTask()->GetName()));
    (*currentTask)->init(fOptions.getOptions());
    (*currentTask)->exec();
    (*currentTask)->terminate();
    INFO(Form("Finished task: %s", dynamic_cast<JPetTaskLoader*>(*currentTask)->getSubTask()->GetName()));
  }
}

void* JPetTaskExecutor::processProxy(void* runner)
{
  static_cast<JPetTaskExecutor*>(runner)->process();
}

TThread* JPetTaskExecutor::run()
{
  TThread* thread = new TThread(to_string(fProcessedFile).c_str(), processProxy, (void*)this);
  assert(thread);
  thread->Run();
  return thread;
}

void JPetTaskExecutor::processFromCmdLineArgs(int)
{
  auto runNum = fOptions.getRunNumber();
  if (runNum >= 0) {
    fParamManager.getParametersFromDatabase(runNum); /// @todo some error handling
  }
  auto inputFileType = fOptions.getInputFileType();
  auto inputFile = fOptions.getInputFile();
  if (inputFileType == JPetOptions::kScope) {
    JPetScopeReader* module = new JPetScopeReader(new JPetScopeTask("JPetScopeReader", "Process Oscilloscope ASCII data into JPetRecoSignal structures."));
    module->setFileName(inputFile);
    fTasks.push_front(module);
  } else if (inputFileType == JPetOptions::kHld) {
    fUnpacker.setParams(fOptions.getInputFile());
    unpackFile();
  }
}

void JPetTaskExecutor::unpackFile()
{
  if (fOptions.getInputFileType() == JPetOptions::kHld) {
    fUnpacker.exec();
  } else {
    WARNING("Input file is not hld and unpacker was supposed to be called!");
  }
}

JPetTaskExecutor::~JPetTaskExecutor()
{
  for (auto task : fTasks) {
    delete task;
  }
}