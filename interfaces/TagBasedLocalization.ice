//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: TagBasedLocalization.idl
//  Source: TagBasedLocalization.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPTAGBASEDLOCALIZATION_ICE
#define ROBOCOMPTAGBASEDLOCALIZATION_ICE

module RoboCompTagBasedLocalization{
	struct LineEstimation {
		string id;
		double distance;
		double angle;
	};
	sequence<LineEstimation> TrolleyRectangle;

	interface TagBasedLocalization{
		void newTagBasedPose(float x, float z, float alpha);
		void trolleyPoseInfo(double x, double z, double alpha, TrolleyRectangle lines);
		void pickUpPoseInfo(double x, double z, double alpha);
		void deliverFinishedPoseInfo(double x, double z, double alpha);			
	};
};
  
#endif