//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: FaceEchord.idl
//  Source: FaceEchord.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPFACEECHORD_ICE
#define ROBOCOMPFACEECHORD_ICE

#include <RGBDEchord.ice>

module RoboCompFaceEchord{
	["cpp:comparable"]
	struct Point3D{
		float x;
					float y;
					float z;
				};
	["cpp:comparable"]
	struct DetailedFace{
		Point3D nose;
		Point3D leftEar;
		Point3D rightEar;
		int left;
		int right;
		int top;
		int bottom;
		int identifier;
		bool rightEyeClosed;
		bool leftEyeClosed;
		float yaw;
					float pitch;
					float roll;
					RoboCompRGBDEchord::TRGBImage faceImage;
	};
	dictionary<int, DetailedFace>DetailedFaceMap;

	interface MSKFaceEvent{
		void  newFaceAvailable(DetailedFaceMap face, long timestamp);
	};
};
  
#endif