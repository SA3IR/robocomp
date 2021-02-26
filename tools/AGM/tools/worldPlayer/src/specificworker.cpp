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

#include <agm_modelConverter.h>


/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx) {
	// Initialize graphical components
	selectComboBox->setEditable(true);
	selectComboBox->setInsertPolicy(QComboBox::NoInsert);
	selectComboBox->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	
	speedSpinBox->setDecimals(3);
	speedSpinBox->setMinimum(0.001);		// Min speed = 10^-3 = 0.001x
	speedSpinBox->setMaximum(1000);			// Max speed = 10^3 = 1000x
	speedSpinBox->setSuffix("x");
	speedSlider->setMinimum(-30);			// Min speed = 10^-3 = 0.001x
	speedSlider->setMaximum(30);			// Max speed = 10^3 = 1000x
	speedSlider->setSingleStep(1);
	
	jumpPastButton->setToolTip(QString("Jump -%1 s").arg(JUMP_SECONDS));
	jumpFutureButton->setToolTip(QString("Jump +%1 s").arg(JUMP_SECONDS));
	jumpPreviousButton->setToolTip("Previous structural change");
	jumpNextButton->setToolTip("Next structural change");
	modelSlider->setTracking(false);
	
	QPalette palette=modelWidget->palette();
	palette.setColor(backgroundRole(),QColor(255,255,255));		// White background
	modelWidget->setPalette(palette);
	modelDrawer=new AGMModelDrawer(new RCDraw(modelWidget),attributesTableWidget);
	
	
	// Graphical events
	connect(selectComboBox,SIGNAL(activated(QString)),this,SLOT(itemSelected(QString)));
	connect(saveModelButton,SIGNAL(clicked()),this,SLOT(saveModel()));
	
	connect(loadFileButton,SIGNAL(clicked()),this,SLOT(loadDumpFileDialog()));
	
	connect(speedSpinBox,SIGNAL(editingFinished()),this,SLOT(speedSpinBoxChange()));
	connect(speedSlider,SIGNAL(valueChanged(int)),this,SLOT(speedSliderChange(int)));
	connect(reverseCheckBox,SIGNAL(stateChanged(int)),this,SLOT(directionChange(int)));
	
	connect(playButton,SIGNAL(clicked()),this,SLOT(play()));
	connect(pauseButton,SIGNAL(clicked()),this,SLOT(pause()));
	connect(stopButton,SIGNAL(clicked()),this,SLOT(stop()));
	
	connect(jumpPastButton,SIGNAL(clicked()),this,SLOT(jumpPast()));
	connect(jumpFutureButton,SIGNAL(clicked()),this,SLOT(jumpFuture()));
	connect(jumpPreviousButton,SIGNAL(clicked()),this,SLOT(jumpPrevious()));
	connect(jumpNextButton,SIGNAL(clicked()),this,SLOT(jumpNext()));
	connect(modelSlider,SIGNAL(actionTriggered(int)),this,SLOT(modelSliderAction(int)));
	
	connect(redrawButton,SIGNAL(clicked()),this,SLOT(redrawModel()));
	
	
	// Initialize variables
	dumpPlayer.setDumpEventCallback(std::bind(&SpecificWorker::onDumpEvent,this,std::placeholders::_1,std::placeholders::_2));
	isSpinBoxChange=false;
	
	// Default values
	speedSpinBox->setValue(1);		// Default speed = 1x
	speedSpinBoxChange();
	reverseCheckBox->setChecked(!dumpPlayer.isDirectionForward());
	updateDumpFileInfo("No file","---",0);
	resetModel();
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker() {
	std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params) {
	// Load dump file
	const std::string defaultDumpFile=getPathFromEnvVar(RC_LOG_PATH_ENV)+"/"+DEFAULT_DUMP_FILENAME;
	loadDumpFile(defaultDumpFile);
	
// Uncomment to enable "compute()" function
	Period=20;
	timer.start(Period);
	return true;
}

void SpecificWorker::compute() {
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
	
	// Update model information
	QMutexLocker locker(mutex);
	modelDrawer->update(worldModel);
	if (updateModelInfo) {
		modelTimeLabel->setText(QString::fromStdString(dumpPlayer.getReplayClockTimeString(timestamp)));
		currentLabel->setText(QString::number(current));
		if (!modelSlider->isSliderDown()) {		// Do not override user action: dragging the slider
			modelSlider->setValue(current);
		}
		if (updateSelectList) {
			fillSelectList();
			updateSelectList=false;
		}
		updateModelInfo=false;
	}
}


// Functions
void SpecificWorker::resetKnownItems() {
	QMutexLocker locker(mutex);
	if (DEBUG) std::cout << LOGTAG << "Clearing symbols and edges lists" << std::endl;
	knownSymbols.clear();
	knownEdges.clear();
	updateSelectList=true;
}
void SpecificWorker::updateKnownItems() {
	bool updateSymbols=false;
	bool updateEdges=false;
	for (auto symbol: worldModel->symbols) {
		const QString str=QString::fromStdString(symbol->toString());
		if (!knownSymbols.contains(str)) {
			if (DEBUG) std::cout << LOGTAG << "Adding to symbols list \"" << str.toStdString() << "\"" << std::endl;
			knownSymbols.append(str);
			updateSymbols=true;
		}
	}
	for (auto edge: worldModel->edges) {
		const QString str=QString::fromStdString(edge->toString(worldModel));
		if (!knownEdges.contains(str)) {
			if (DEBUG) std::cout << LOGTAG << "Adding to edges list \"" << str.toStdString() << "\"" << std::endl;
			knownEdges.append(str);
			updateEdges=true;
		}
	}
	if (updateSymbols) {
		knownSymbols.sort();
	}
	if (updateEdges) {
		knownEdges.sort();
	}
	if (updateSymbols||updateEdges) {
		updateSelectList=true;
	}
}
void SpecificWorker::fillSelectList() {
	if (DEBUG) std::cout << LOGTAG << "Updating select item's list" << std::endl;
	const QString text=selectComboBox->currentText();
	selectComboBox->clear();
	selectComboBox->addItems(knownSymbols);
	selectComboBox->addItems(knownEdges);
	selectComboBox->setEditText(text);
}
void SpecificWorker::itemSelected(QString itemName) {
	modelDrawer->setInterest(itemName.toStdString());
	selectComboBox->setCurrentIndex(-1);
}

void SpecificWorker::saveModel() {
	const QString filename=QFileDialog::getSaveFileName(this,"Choose a file to save the current model");
	if (filename.isEmpty()) {
		return;
	}
	QMutexLocker locker(mutex);
	if (DEBUG) std::cout << LOGTAG << "Saving model to \"" << filename.toStdString() << "\"" << std::endl;
	worldModel->save(filename.toStdString());
}

void SpecificWorker::loadDumpFileDialog() {
	const QString filename=QFileDialog::getOpenFileName(this,"Choose a dump file to load");
	if (!filename.isEmpty()) {
		// Stop previous reproduction (if any)
		stop();
		loadDumpFile(filename.toStdString());
	}
}
void SpecificWorker::loadDumpFile(const std::string &filename) {
	std::cout << "Loading dump file \"" << filename.c_str() << "\"" << std::endl;
	const std::vector<RoboCompDumper::Dump> dumps=dumpPlayer.readFullDumpFile(filename);
	const std::chrono::system_clock::time_point timeReference=dumpPlayer.getDumpFileTime();
	dumpPlayer.setReplayTimeReference(timeReference);
	dumpPlayer.setReplayContent(dumps);
	
	// Update dump file information
	if (DEBUG) std::cout << LOGTAG << "Read " << dumps.size() << " dumps from file" << std::endl;
	updateDumpFileInfo(filename,RoboCompDumper::getClockTimeString(timeReference,false),dumps.size());
	resetKnownItems();
	// Load current model and go to the first world
	const size_t index=dumpPlayer.getIndex();
	if (!goToDump(dumpPlayer.findNextWorld(index))) {
		resetModel();
	}
}
void SpecificWorker::updateDumpFileInfo(const std::string &filename,const std::string &date,const size_t &nDumps) {
	fileLabel->setText(QString::fromStdString(filename));
	fileLabel->setToolTip(QString::fromStdString(filename));
	dateLabel->setText(QString::fromStdString(date));
	totalLabel->setText(QString("%1 (%2 dumps)").arg(QString::fromStdString(dumpPlayer.getReplayDurationString())).arg(nDumps));
	
	if (nDumps>0) {
		maxLabel->setText(QString::number(nDumps-1));
		modelSlider->setMaximum(nDumps-1);
		modelSlider->setPageStep(std::ceil(static_cast<double>(nDumps)/10));
	} else {
		maxLabel->setText(QString::number(-1));
		modelSlider->setMaximum(0);
		modelSlider->setPageStep(1);
	}
}

void SpecificWorker::speedSpinBoxChange() {
	isSpinBoxChange=true;
	const double speed=speedSpinBox->value();
	speedSlider->setValue(std::round(10*std::log10(speed)));
	isSpinBoxChange=false;
	if (DEBUG) std::cout << LOGTAG << "Setting player speed to " << speed << std::endl;
	dumpPlayer.setSpeed(speed);
}
void SpecificWorker::speedSliderChange(int value) {
	// Do nothing if change comes from spinBox
	if (isSpinBoxChange) {
		return;
	}
	
	const double speed=std::pow(10,static_cast<double>(value)/10);
	speedSpinBox->setValue(speed);
	if (DEBUG) std::cout << LOGTAG << "Setting player speed to " << speed << std::endl;
	dumpPlayer.setSpeed(speed);
}
void SpecificWorker::directionChange(int state) {
	if (state==Qt::Checked) {
		if (DEBUG) std::cout << LOGTAG << "Setting player direction to backwards" << std::endl;
		dumpPlayer.setDirectionBackwards(false);
	} else {
		if (DEBUG) std::cout << LOGTAG << "Setting player direction to forward" << std::endl;
		dumpPlayer.setDirectionForward(false);
	}
}

void SpecificWorker::play() {
	if (DEBUG) std::cout << LOGTAG << "Starting reproduction" << std::endl;
	dumpPlayer.startReplay();
}
void SpecificWorker::pause() {
	if (DEBUG) std::cout << LOGTAG << "Pausing reproduction" << std::endl;
	dumpPlayer.stopReplay(false);
}
void SpecificWorker::stop() {
	if (DEBUG) std::cout << LOGTAG << "Stopping reproduction" << std::endl;
	dumpPlayer.stopReplay();
	// Load current model and go to the first world
	const size_t index=dumpPlayer.getIndex();
	if (!goToDump(dumpPlayer.findNextWorld(index))) {
		resetModel();
	}
}

void SpecificWorker::jumpPast() {
	if (DEBUG) std::cout << LOGTAG << "Jumping to dump at " << -JUMP_SECONDS << " seconds" << std::endl;
	goToDump(dumpPlayer.jumpTo(std::chrono::seconds(-JUMP_SECONDS)));
}
void SpecificWorker::jumpFuture() {
	if (DEBUG) std::cout << LOGTAG << "Jumping to dump at " << JUMP_SECONDS << " seconds" << std::endl;
	goToDump(dumpPlayer.jumpTo(std::chrono::seconds(JUMP_SECONDS)));
}
void SpecificWorker::jumpPrevious() {
	const size_t index=dumpPlayer.getIndex();
	if (index>0) {
		goToDump(dumpPlayer.findPreviousWorld(index-1));
	}
}
void SpecificWorker::jumpNext() {
	const size_t index=dumpPlayer.getIndex();
	goToDump(dumpPlayer.findNextWorld(index+1));
}
void SpecificWorker::modelSliderAction(int action) {
	int position=modelSlider->sliderPosition();
	if (position<0) {
		position=0;
	}
	goToDump(position);
}
bool SpecificWorker::goToDump(const size_t &next) {
	if (!dumpPlayer.setIndex(next)) {
		return false;
	}
	
	if (DEBUG) std::cout << LOGTAG << "Jumping to dump " << next << std::endl;
	const size_t first=dumpPlayer.findPreviousWorld(next);
	QMutexLocker locker(mutex);
	if (DEBUG) std::cout << LOGTAG << "Getting updates since previous world: " << first << " to " << next << std::endl;
	worldModel=AGMModel::SPtr(new AGMModel());
	RoboCompDumper::Dump dump;
	for (size_t i=first;i<next;i++) {
		dumpPlayer.getDump(dump,i);
		RoboCompDumper::handleToWorker(this,dump);
	}
	dumpPlayer.getDump(dump,next);
	onDumpEvent(dump,next);
	return true;
}
void SpecificWorker::resetModel() {
	QMutexLocker locker(mutex);
	if (DEBUG) std::cout << LOGTAG << "Reset model" << std::endl;
	worldModel=AGMModel::SPtr(new AGMModel());
	current=dumpPlayer.getIndex();
	timestamp=timestamp.zero();
	updateModelInfo=true;
}
void SpecificWorker::redrawModel() {
	if (DEBUG) std::cout << LOGTAG << "Redrawing model" << std::endl;
	// Clean model
	modelDrawer->update(AGMModel::SPtr(new AGMModel()));
	// Update model information
	compute();
}


// Dump events
void SpecificWorker::onDumpEvent(const RoboCompDumper::Dump &dump,const size_t &index) {
	QMutexLocker locker(mutex);
	current=index;
	timestamp=dump.timestamp;
	RoboCompDumper::handleToWorker(this,dump);
	updateModelInfo=true;
}

void SpecificWorker::structuralChange(const RoboCompAGMWorldModel::World &w) {
	if (DEBUG) std::cout << LOGTAG << "World v" << w.version << std::endl;
	AGMModelConverter::fromIceToInternal(w,worldModel);
	updateKnownItems();
}
void SpecificWorker::symbolUpdated(const RoboCompAGMWorldModel::Node &modification) {
	if (DEBUG) std::cout << LOGTAG << "Symbol update" << std::endl;
	AGMModelConverter::includeIceModificationInInternalModel(modification,worldModel);
}
void SpecificWorker::edgeUpdated(const RoboCompAGMWorldModel::Edge &modification) {
	if (DEBUG) std::cout << LOGTAG << "Edge update" << std::endl;
	AGMModelConverter::includeIceModificationInInternalModel(modification,worldModel);
}
void SpecificWorker::symbolsUpdated(const RoboCompAGMWorldModel::NodeSequence &modifications) {
	if (DEBUG) std::cout << LOGTAG << "Symbols (" << modifications.size() << ") update" << std::endl;
	for (auto modification: modifications) {
		AGMModelConverter::includeIceModificationInInternalModel(modification,worldModel);
	}
}
void SpecificWorker::edgesUpdated(const RoboCompAGMWorldModel::EdgeSequence &modifications) {
	if (DEBUG) std::cout << LOGTAG << "Edges (" << modifications.size() << ") update" << std::endl;
	for (auto modification: modifications) {
		AGMModelConverter::includeIceModificationInInternalModel(modification,worldModel);
	}
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
	}
	return path;
}


