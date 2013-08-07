#ifndef _JPETSIGCH_H_
#define _JPETSIGCH_H_

#include <vector>
#include <map>

#include "../JPetPM/JPetPM.h"

using namespace std;

class JPetSigCh {
public:
	JPetSigCh();
	JPetSigCh(float EdgeTime, float FallEdgeTime = 0);
	
	typedef enum EdgeType { kEdge, kFalling };
	
	float getTime(EdgeType Type) { return fChannels[0][Type]; }
	float getTime(int ChNo, EdgeType Type) { return fChannels[ChNo][Type]; }
	void addCh(float EdgeTime, float FallEdgeTime);
	void setPM(JPetPM* PM) { fPM = PM; }
private:
	typedef map < EdgeType, float > SingleCh;
	typedef vector < SingleCh > ChSet;
	//float fAmpl;
	 
	ChSet fChannels;
	bool fIsSlow;
	JPetPM* fPM;
};

#endif