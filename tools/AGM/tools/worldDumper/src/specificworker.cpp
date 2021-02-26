/*
 *    Copyright (C)2020 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx)
{

}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//       THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = new InnerModel(innermodel_path);
//	}
//	catch(std::exception e) { qFatal("Error reading config params"); }


	// Create dump file
	const std::string dumpFile=getPathFromEnvVar(RC_LOG_PATH_ENV)+"/"+DUMP_FILENAME;
	if (dumper.createDumpFile(dumpFile)) {
		std::cout << "Using dump file \"" << dumpFile.c_str() << "\"" << std::endl;
	} else {
		std::cout << "ERROR: Could not create dump file \"" << dumpFile.c_str() << "\"" << std::endl;
		return false;
	}


// Uncomment to enable "compute()" function
//	timer.start(Period);

	return true;
}

void SpecificWorker::compute()
{
	QMutexLocker locker(mutex);
	//computeCODE
// 	try
// 	{
// 		camera_proxy->getYImage(0,img, cState, bState);
// 		memcpy(image_gray.data, &img[0], m_width*m_height*sizeof(uchar));
// 		searchTags(image_gray);
// 	}
// 	catch(const Ice::Exception &e)
// 	{
// 		std::cout << "Error reading from Camera" << e << std::endl;
// 	}
}


void SpecificWorker::structuralChange(const RoboCompAGMWorldModel::World &w)
{
//subscribesToCODE
	if (DEBUG) std::cout << LOGTAG << "Received world v" << w.version << std::endl;
	dumper.writeDump(w);
	dumper.flushDumpFile();
}

void SpecificWorker::edgesUpdated(const RoboCompAGMWorldModel::EdgeSequence &modifications)
{
//subscribesToCODE
	if (DEBUG) std::cout << LOGTAG << "Received edges update (" << modifications.size() << ")" << std::endl;
	dumper.writeDump(modifications);
}

void SpecificWorker::edgeUpdated(const RoboCompAGMWorldModel::Edge &modification)
{
//subscribesToCODE
	if (DEBUG) std::cout << LOGTAG << "Received edge update" << std::endl;
	dumper.writeDump(modification);
}

void SpecificWorker::symbolUpdated(const RoboCompAGMWorldModel::Node &modification)
{
//subscribesToCODE
	if (DEBUG) std::cout << LOGTAG << "Received symbol update" << std::endl;
	dumper.writeDump(modification);
}

void SpecificWorker::symbolsUpdated(const RoboCompAGMWorldModel::NodeSequence &modifications)
{
//subscribesToCODE
	if (DEBUG) std::cout << LOGTAG << "Received symbols update (" << modifications.size() << ")" << std::endl;
	dumper.writeDump(modifications);
}

// Helper functions
std::string SpecificWorker::getPathFromEnvVar(const std::string &envVarName) {
	std::string path;
	const char* envContent=std::getenv(envVarName.c_str());
	if (envContent!=NULL) {
		path=envContent;
	}
	// Set local directory if path is empty (environment variable does not exist OR it exists but is blank)
	if (path.empty()) {
		std::cout << "WARNING: Environment variable \"" << envVarName.c_str() << "\" does not exist or is blank, using local folder" << std::endl;
		path="./";
//	} else {
//		// Create directory if does not exist
//		std::system(("mkdir -p \""+path+"\"").c_str());												// TODO: make dir using "stat" or sanitize input (content from envVarName)
	}
	return path;
}
