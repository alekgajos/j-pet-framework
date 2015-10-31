/**
 *  @copyright Copyright (c) 2014, Wojciech Krzemien
 *  @file JPetReaderInterface.h 
 *  @author Wojciech Krzemien, wojciech.krzemien@if.uj.edu.pl
 *  @brief
 */ 

#ifndef JPETREADERINTERFACE_H 
#define JPETREADERINTERFACE_H 

#include <TNamed.h> // for Event typedef

class JPetReaderInterface {
 public:
  typedef TObject MyEvent; 
  virtual ~JPetReaderInterface() {;}
  virtual MyEvent& getCurrentEvent()=0;   
  virtual bool nextEvent()=0;
  virtual bool firstEvent()=0;
  virtual bool lastEvent()=0;
  virtual bool nthEvent(int n)=0;
  virtual long long getCurrentEventNumber() const =0;
  virtual long long getNbOfAllEvents() const =0; 

  virtual bool openFileAndLoadData(const char* filename, const char* treename)=0;
  virtual void closeFile()=0; 
};
#endif /*  !JPETREADERINTERFACE_H */