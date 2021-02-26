/*
 *    Copyright (C)2019 by YOUR NAME HERE
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
#ifndef GENERICWORKER_H
#define GENERICWORKER_H

#include "config.h"
#include <stdint.h>
#include <qlog/qlog.h>

#if Qt5_FOUND
	#include <QtWidgets>
#else
	#include <QtGui>
#endif
#include <ui_mainUI.h>
#include <CommonBehavior.h>

#include <Planning.h>
#include <AGMCommonBehavior.h>
#include <AGMExecutive.h>


#define CHECK_PERIOD 5000
#define BASIC_PERIOD 100

using namespace std;
using namespace RoboCompPlanning;
using namespace RoboCompAGMCommonBehavior;
using namespace RoboCompAGMExecutive;

typedef map <string,::IceProxy::Ice::Object*> MapPrx;


class GenericWorker :
#ifdef USE_QTGUI
	public QWidget, public Ui_guiDlg
#else
	public QObject
 #endif
{
Q_OBJECT
public:
	GenericWorker(MapPrx& mprx);
	virtual ~GenericWorker();
	virtual void killYourSelf();
	virtual void setPeriod(int p);

	virtual bool setParams(RoboCompCommonBehavior::ParameterList params) = 0;
	QMutex *mutex;


	AGMExecutivePrx agmexecutive_proxy;

	virtual bool AGMCommonBehavior_activateAgent(const ParameterMap &prs) = 0;
	virtual bool AGMCommonBehavior_deactivateAgent() = 0;
	virtual StateStruct AGMCommonBehavior_getAgentState() = 0;
	virtual ParameterMap AGMCommonBehavior_getAgentParameters() = 0;
	virtual bool AGMCommonBehavior_setAgentParameters(const ParameterMap &prs) = 0;
	virtual void AGMCommonBehavior_killAgent() = 0;
	virtual int AGMCommonBehavior_uptimeAgent() = 0;
	virtual bool AGMCommonBehavior_reloadConfigAgent() = 0;
	virtual void AGMExecutiveTopic_structuralChange(const RoboCompAGMWorldModel::World &w) = 0;
	virtual void AGMExecutiveTopic_symbolUpdated(const RoboCompAGMWorldModel::Node &modification) = 0;
	virtual void AGMExecutiveTopic_symbolsUpdated(const RoboCompAGMWorldModel::NodeSequence &modifications) = 0;
	virtual void AGMExecutiveTopic_edgeUpdated(const RoboCompAGMWorldModel::Edge &modification) = 0;
	virtual void AGMExecutiveTopic_edgesUpdated(const RoboCompAGMWorldModel::EdgeSequence &modifications) = 0;
    virtual void AGMExecutiveTopic_selfEdgeAdded(const int nodeid, const string &edgeType, const RoboCompAGMWorldModel::StringDictionary &attributes) = 0;
    virtual void AGMExecutiveTopic_selfEdgeDeleted(const int nodeid, const string &edgeType) = 0;
	virtual void AGMExecutiveVisualizationTopic_update(const RoboCompAGMWorldModel::World &world, const string &target, const RoboCompPlanning::Plan &plan) = 0;
protected:

	QTimer timer;
	int Period;

private:


public slots:
	virtual void compute() = 0;
    virtual void initialize(int period) = 0;
	
signals:
	void kill();
};

#endif
