//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: personFollower.idl
//  Source: personFollower.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPPERSONFOLLOWER_ICE
#define ROBOCOMPPERSONFOLLOWER_ICE

module RoboComppersonFollower{
	["cpp:comparable"]
	struct Pose{
		float x;
		float y;
		float z;
		float orientation;
	};
	interface personFollower{
		void updatePersonPose(Pose p);
		void personLost();	
	};
};
  
#endif
