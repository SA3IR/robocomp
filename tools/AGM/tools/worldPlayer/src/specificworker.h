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

/**
       \brief
       @author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <innermodel/innermodel.h>

#include "robocompDumper.h"
#include <agm_modelDrawer.h>


class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(MapPrx& mprx);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);
	
	
public slots:
	void compute();
	
	// Graphical events
	void itemSelected(QString itemName);
	void saveModel();
	
	void loadDumpFileDialog();
	
	void speedSpinBoxChange();
	void speedSliderChange(int value);
	void directionChange(int state);
	
	void play();
	void pause();
	void stop();
	
	void jumpPast();
	void jumpFuture();
	void jumpPrevious();
	void jumpNext();
	void modelSliderAction(int action);
	
	void redrawModel();
	
	
private:
//	InnerModel *innerModel;
	
	
	// Debug
	static const bool DEBUG=false;										///< Constant value that allows the compiler to exclude log code.
	static constexpr const char* LOGTAG="SpecificWorker: ";				///< Defines a log tag with a different name per class.
	
	
	// Constants
	const std::string RC_LOG_PATH_ENV="ROBOCOMP_LOGS";
	const std::string DEFAULT_DUMP_FILENAME="worlds.dump";
	const int JUMP_SECONDS=60;
	
	
	// Variables
	RoboCompDumper dumpPlayer;
	AGMModel::SPtr worldModel;
	size_t current;
	std::chrono::nanoseconds timestamp;
	bool updateModelInfo;
	bool updateSelectList;
	QStringList knownSymbols;
	QStringList knownEdges;
	
	AGMModelDrawer *modelDrawer;
	bool isSpinBoxChange;
	
	
	// Functions
	void resetKnownItems();
	void updateKnownItems();
	void fillSelectList();
	
	void loadDumpFile(const std::string &filename);
	void updateDumpFileInfo(const std::string &filename,const std::string &date,const size_t &nDumps);
	
	bool goToDump(const size_t &n);
	void resetModel();
	
	
	// Dump events
	void onDumpEvent(const RoboCompDumper::Dump &dump,const size_t &index);
	
	void structuralChange(const RoboCompAGMWorldModel::World &w);
	void symbolUpdated(const RoboCompAGMWorldModel::Node &modification);
	void edgeUpdated(const RoboCompAGMWorldModel::Edge &modification);
	void symbolsUpdated(const RoboCompAGMWorldModel::NodeSequence &modifications);
	void edgesUpdated(const RoboCompAGMWorldModel::EdgeSequence &modifications);
	
	
	// Helper functions
	static std::string getPathFromEnvVar(const std::string &envVarName);
	
	
};

#endif
