//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: CAIHMCEchord.idl
//  Source: CAIHMCEchord.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPCAIHMCECHORD_ICE
#define ROBOCOMPCAIHMCECHORD_ICE


module RoboCompCAIHMCEchord{

	interface AgentToHMCComp{
		void  searchPatientSeated();
		void  restart();
		void  processData();
		void  setPersonID(int personid);
	};
	interface HMCCompToAgent{
		void  patientIsSeated();
		void  patientIsStanding();
		void  testIsFinished();
		void  patientIsFar();
		void  processDataIsFinished(string result);
	};
};
  
  
#endif
