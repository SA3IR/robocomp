//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: AGMCommonBehavior.idl
//  Source: AGMCommonBehavior.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPLEGCONTROLLER_ICE
#define ROBOCOMPLEGCONTROLLER_ICE

module RoboCompLegController
{
	struct PoseLeg
	{
		float x;		//mm
		float y;		//mm
		float z;		//mm
		float vel;		//rad/s
		string ref;
	};
	struct PoseBody
	{
		float rx;		//rad
		float ry;		//rad
		float rz;		//rad
		float vel;		//rad/s
	};
	struct AnglesLeg
	{
		float q1; 	//rad
		float q2;		//rad
		float q3;		//rad
		float vel;		//rad/s
	};
	struct StateLeg
	{
		float posclavicula; 	//rad
		float poshombro; 	//rad
		float poscodo; 		//rad
		float x;		//mm
		float y;		//mm
		float z;		//mm
		string ref;
		bool ismoving;
	};
	
	interface LegController
	{
		void setIKLeg(PoseLeg p);
		void setIKBody(PoseBody p);
		void setFKLeg(AnglesLeg al);
		StateLeg getStateLeg();
	};
};

  
#endif
