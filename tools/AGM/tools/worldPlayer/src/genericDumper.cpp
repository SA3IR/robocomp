#include "genericDumper.h"
#include <iomanip>
#include <sstream>
#include <cstring>


// Functions
GenericDumper::GenericDumper() {
}
GenericDumper::~GenericDumper() {
	closeDumpFile();
}


// Manage dump file
bool GenericDumper::createDumpFile(const std::string &filename) {
	// Create file
	dumpFile.open(filename,std::ios::out|std::ios::binary|std::ios::trunc);
	if (dumpFile.fail()) {
		std::cout << "ERROR: Could not create dump file " << filename.c_str() << std::endl;
		return false;
	}
	
	// Set time reference
	referenceClockTime=std::chrono::system_clock::now();
	referenceTime=std::chrono::steady_clock::now();
	
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "Created dump file " << filename << std::endl;
		std::cout << LOGTAG << " - Time reference: " << getClockTimeString(referenceClockTime) << std::endl;
	}
	
	// Write header: head signature and time reference (ns since epoch)
	dumpFile.write(HEAD,sizeof(Head));
	const ClockTimeReference nanosSinceEpoch=std::chrono::nanoseconds(referenceClockTime.time_since_epoch()).count();
	dumpFile.write(reinterpret_cast<const char*>(&nanosSinceEpoch),sizeof(ClockTimeReference));
	dumpFile.flush();
	return dumpFile.good();
}
bool GenericDumper::openDumpFile(const std::string &filename) {
	// Open file
	dumpFile.open(filename,std::ios::in|std::ios::binary);
	if (dumpFile.fail()) {
		std::cout << "ERROR: Could not open dump file " << filename.c_str() << std::endl;
		return false;
	}
	
	// Read header: head signature and time reference (ns since epoch)
	Head head;
	dumpFile.read(head,sizeof(Head));
	ClockTimeReference nanosSinceEpoch;
	dumpFile.read(reinterpret_cast<char*>(&nanosSinceEpoch),sizeof(ClockTimeReference));
	if (!dumpFile.good()) {
		std::cout << "ERROR: Could not read dump file header" << std::endl;
		dumpFile.close();
		return false;
	}
	
	// Check header values for errors
	if (std::memcmp(head,HEAD,sizeof(Head))!=0) {
		std::cout << "ERROR: Wrong dump file. Bad head signature" << std::endl;
		dumpFile.close();
		return false;
	}
	
	// Set time reference
	referenceClockTime=std::chrono::system_clock::time_point(std::chrono::nanoseconds(nanosSinceEpoch));
	
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "Open dump file " << filename.c_str() << std::endl;
		std::cout << LOGTAG << " - Time reference: " << getClockTimeString(referenceClockTime) << std::endl;
	}
	
	return true;
}
bool GenericDumper::flushDumpFile() {
	dumpFile.flush();
	return dumpFile.good();
}
void GenericDumper::closeDumpFile() {
	if (dumpFile.is_open()) {
		dumpFile.close();
		if (DEBUG) std::cout << LOGTAG << "Dump file closed" << std::endl;
	}
}

std::chrono::system_clock::time_point GenericDumper::getDumpFileTime() {
	return referenceClockTime;
}


// Single dump functions
bool GenericDumper::writeDump(const DumpTimestamp &timestamp,const DumpType &type,const Byte* data,const DumpSize &size) {
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "Writing dump: " << std::endl;
		std::cout << LOGTAG << " - Time since reference: " << timestamp << " (ns)" << std::endl;
		std::cout << LOGTAG << " - Type: " << type << std::endl;
		std::cout << LOGTAG << " - Size: " << size << " bytes" << std::endl;
	}
	
	// Write to dump file: timestamp, type, size and data
	dumpFile.write(reinterpret_cast<const char*>(&timestamp),sizeof(DumpTimestamp));
	dumpFile.write(reinterpret_cast<const char*>(&type),sizeof(DumpType));
	dumpFile.write(reinterpret_cast<const char*>(&size),sizeof(DumpSize));
	dumpFile.write(reinterpret_cast<const char*>(data),size);
	return dumpFile.good();
}
bool GenericDumper::writeDump(const DumpType &type,const Byte* data,const DumpSize &size) {
	// Calculate timestamp (ns since time reference)
	const DumpTimestamp timestamp=std::chrono::nanoseconds(std::chrono::steady_clock::now()-referenceTime).count();
	return writeDump(timestamp,type,data,size);
}
bool GenericDumper::writeDump(const DumpType &type,const std::vector<Byte> &data) {
	return writeDump(type,data.data(),data.size());
}

bool GenericDumper::readDump(Dump &dump,const std::function<std::shared_ptr<void>(const short &,const std::vector<Byte> &)> &regenerateObject) {
	// Read dump from file: timestamp, type, size and data
	DumpTimestamp timestamp;
	dumpFile.read(reinterpret_cast<char*>(&timestamp),sizeof(DumpTimestamp));
	const bool isEmpty=(dumpFile.gcount()==0);
	DumpType type;
	dumpFile.read(reinterpret_cast<char*>(&type),sizeof(DumpType));
	DumpSize size=0;
	dumpFile.read(reinterpret_cast<char*>(&size),sizeof(DumpSize));
	std::vector<Byte> data(size);
	dumpFile.read(reinterpret_cast<char*>(data.data()),size);
	if (dumpFile.fail()) {
		if (!dumpFile.eof()) {
			std::cout << "ERROR: Fail while reading dump file" << std::endl;
		}
		if (!isEmpty) {
			std::cout << "WARNING: Ignoring incomplete dump" << std::endl;
		}
		return false;
	}
	
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "Dump read: " << std::endl;
		std::cout << LOGTAG << " - Time since reference: " << timestamp << " (ns)" << std::endl;
		std::cout << LOGTAG << " - Estimated time: " << getClockTimeString(referenceClockTime+dump.timestamp) << std::endl;
		std::cout << LOGTAG << " - Type: " << type << std::endl;
		std::cout << LOGTAG << " - Size: " << size << " bytes" << std::endl;
	}
	
	// Get timestamp (ns since time reference)
	const std::chrono::nanoseconds elapsed(timestamp);
	
	// Save dump
	dump.timestamp=elapsed;
	dump.type=type;
	dump.data=data;
	
	// Get object from dump content (if possible) and free resources
	if (regenerateObject!=nullptr) {
		dump.object=regenerateObject(dump.type,dump.data);
		if (dump.object!=nullptr) {
			dump.data.clear();
			dump.data.shrink_to_fit();
		}
	} else {
		dump.object=nullptr;
	}
	
	return true;
}


// Full file functions
std::vector<GenericDumper::Dump> GenericDumper::readFullDumpFile(const std::string &filename,const std::function<std::shared_ptr<void>(const short &,const std::vector<Byte> &)> &regenerateObject) {
	if (DEBUG) std::cout << LOGTAG << "Reading all dumps from file" << std::endl;
	
	std::vector<Dump> dumps;
	if (!openDumpFile(filename)) {
		// Could not read file, set null time reference
		referenceClockTime=std::chrono::system_clock::time_point();
		return dumps;
	}
	
	Dump dump;
	while (readDump(dump,regenerateObject)) {
		dumps.push_back(dump);
	}
	if (DEBUG) std::cout << LOGTAG << "Read " << dumps.size() << " dumps from file" << std::endl;
	closeDumpFile();
	
	return dumps;
}


// Debug functions
std::string GenericDumper::getClockTimeString(const std::chrono::system_clock::time_point &time,const bool &subseconds) {
	// Print cTime (seconds) and add subseconds
	const std::chrono::system_clock::time_point timeSeconds=std::chrono::time_point_cast<std::chrono::seconds>(time);
	const std::time_t cTime=std::chrono::system_clock::to_time_t(timeSeconds);
	std::ostringstream out;
	out << std::put_time(std::localtime(&cTime),"%F %T");
	
	// Add subseconds
	if (subseconds) {
		const std::chrono::microseconds micros=std::chrono::duration_cast<std::chrono::microseconds>(time-timeSeconds);
		out << "." << std::setfill('0') << std::setw(6) << micros.count();
	}
	return out.str();
}




//####################################################################################################################################################
// DumpPlayer functions
//####################################################################################################################################################

// Constructor
DumpPlayer::DumpPlayer() {
	// Initialize variables
	isRunning.store(false);
	wakeUp.store(false);
	
	stop.store(false);
	setDirectionForward();
	setSpeed(1);
}
DumpPlayer::~DumpPlayer() {
	stopReplay(false);
}


// Player functions
void DumpPlayer::startReplaySync() {
	// Check for available data
	const size_t nDumps=dumps.size();
	if (nDumps==0) {
		std::cout << "WARNING: There are no dumps to replay" << std::endl;
		isRunning.store(false);
		return;
	}
	if (DEBUG) std::cout << LOGTAG << "Starting replay" << std::endl;
	
	// DEBUG_REPLAY_TIME: Check replay time (1/2). Variable definitions
	const size_t firstDump=index.load();
	const std::chrono::steady_clock::time_point startReplayTime=std::chrono::steady_clock::now();
	// DEBUG_REPLAY_TIME: Check replay time (1/2). End variable definitions
	
	std::chrono::steady_clock::time_point lastEventTime;
	std::chrono::duration<double,std::nano> eventTimeDiff;
	std::unique_lock<std::mutex> lock(replayMutex);
	while (!stop.load()) {
		// Dump event
		lastEventTime=std::chrono::steady_clock::now();
		size_t current=index.load();
		if (DEBUG) std::cout << LOGTAG << "Calling event handler for dump " << current << std::endl;
		onDumpEvent(dumps[current],current);
		
		// Locate next dump
		size_t next;
		if (forward.load()) {
			if (current>=(nDumps-1)) break;
			next=current+1;
		} else {
			if (current<=0) break;
			next=current-1;
		}
		
		// Sleep until next event time
		eventTimeDiff=(dumps[next].timestamp-dumps[current].timestamp)*timeFactor.load();
		if (next<current) {		// To get proper sign. Note that std::chrono::abs() was not available until C++17
			eventTimeDiff=-eventTimeDiff;
		}
		if (DEBUG) std::cout << LOGTAG << "Waiting for " << eventTimeDiff.count() << " ns until next event" << std::endl;
		if (conditionVariable.wait_until(lock,lastEventTime+eventTimeDiff,[this](){return wakeUp.load();})) {
			if (DEBUG) std::cout << LOGTAG << "Replay thread has been waked up" << std::endl;
			// Reset wake up flag and go to next iteration without update index
			wakeUp.store(false);
			continue;
		}
		
		// Update next dump index if was not modified (atomically)
		index.compare_exchange_strong(current,next);
		// Be careful using "current" value after call compare_exchange_*(). It may be modified:
		// - If index==current (not modified) => return true; and index=next;
		// - If index!=current (was modified) => return false; and current=index;		<--- Look out!
	}
	
	if (DEBUG_REPLAY_TIME) {		// DEBUG_REPLAY_TIME: Check replay time (2/2). Results
		const size_t lastDump=index.load();
		const std::chrono::duration<double,std::nano> elapsed=lastEventTime-startReplayTime;
		eventTimeDiff=(dumps[lastDump].timestamp-dumps[firstDump].timestamp)*timeFactor.load();
		if (lastDump<firstDump) {		// To get proper sign. Note that std::chrono::abs() was not available until C++17
			eventTimeDiff=-eventTimeDiff;
		}
		std::cout << LOGTAG << "Dump replay from " << firstDump << " to " << lastDump << " (" << lastDump-firstDump+1 << "/" << nDumps << " dumps) at " << 1/timeFactor << "x" << std::endl;
		std::cout << LOGTAG << " - Timestamps diff : " << std::setw(15) << eventTimeDiff.count() << " ns" << std::endl;
		std::cout << LOGTAG << " - Elapsed time    : " << std::setw(15) << elapsed.count() << " ns" << std::endl;
		std::cout << LOGTAG << " - Difference      : " << std::setw(15) << (elapsed-eventTimeDiff).count() << " ns" << std::endl;
	}
	
	// Call replay finishing event
	if (onReplayFinishing) {
		if (DEBUG) std::cout << LOGTAG << "Calling replay finishing event handler" << std::endl;
		onReplayFinishing();
	}
	if (DEBUG) std::cout << LOGTAG << "Replay finished" << std::endl;
	isRunning.store(false);
}
bool DumpPlayer::startReplay() {
	// Check for requirements
	if (onDumpEvent==nullptr) {
		std::cout << "WARNING: Dump event callback should be set to start replay thread" << std::endl;
		return false;
	}
	
	// Check if replay thread is running
	if (isRunning) {
		std::cout << "WARNING: Replay thread is already running" << std::endl;
		return false;
	}
	// Check if replay thread is finished
	if (replayThread.joinable()) {
		replayThread.join();
	}
	
	// Reset thread flags
	wakeUp.store(false);
	isRunning.store(true);
	if (DEBUG) std::cout << LOGTAG << "Creating replay thread" << std::endl;
	replayThread=std::thread(&DumpPlayer::startReplaySync,this);
	return true;
}
void DumpPlayer::stopReplay(const bool &reset) {
	// Check if replay thread is running or finished
	if (replayThread.joinable()) {
		if (DEBUG) std::cout << LOGTAG << "Stopping replay thread" << std::endl;
		// Activate stop flag
		stop.store(true);
		wakeUpReplayThread();
		replayThread.join();
		// Reset stop flag
		stop.store(false);
	}
	// Reset index if required
	if (reset) {
		resetIndex();
	}
}

void DumpPlayer::wakeUpReplayThread() {
	// Check if replay thread is running
	if (isRunning) {
		if (DEBUG) std::cout << LOGTAG << "Waking up replay thread" << std::endl;
		std::lock_guard<std::mutex> lock(replayMutex);
		// Activate wake up flag
		wakeUp.store(true);
		conditionVariable.notify_all();
	}
}


// Configuration functions
bool DumpPlayer::setReplayContent(const std::vector<GenericDumper::Dump> &content) {
	// Check if replay thread is running
	if (isRunning) {
		std::cout << "WARNING: It's not allowed to update content while replay thread is running" << std::endl;
		return false;
	}
	
	if (DEBUG) std::cout << LOGTAG << "Loading new content (" << dumps.size() << " dumps)" << std::endl;
	dumps=content;
	resetIndex();
	return true;
}
bool DumpPlayer::setReplayTimeReference(const std::chrono::system_clock::time_point &time) {
	// Check if replay thread is running
	if (isRunning) {
		std::cout << "WARNING: It's not allowed to update the time reference while replay thread is running" << std::endl;
		return false;
	}
	
	if (DEBUG) std::cout << LOGTAG << "Loading new time reference" << std::endl;
	timeReference=time;
	return true;
}
bool DumpPlayer::setDumpEventCallback(const std::function<void(const GenericDumper::Dump &,const size_t &)> &callback) {
	// Check if replay thread is running
	if (isRunning) {
		std::cout << "WARNING: It's not allowed to update any callback while replay thread is running" << std::endl;
		return false;
	}
	
	if (DEBUG) std::cout << LOGTAG << "Setting new onDumpEvent callback" << std::endl;
	onDumpEvent=callback;
	return true;
}
bool DumpPlayer::setReplayFinishingCallback(const std::function<void()> &callback) {
	// Check if replay thread is running
	if (isRunning) {
		std::cout << "WARNING: It's not allowed to update any callback while replay thread is running" << std::endl;
		return false;
	}
	
	if (DEBUG) std::cout << LOGTAG << "Setting new onReplayFinishing callback" << std::endl;
	onReplayFinishing=callback;
	return true;
}


// Controls functions
void DumpPlayer::resetIndex() {
	if (DEBUG) std::cout << LOGTAG << "Setting index to its initial value" << std::endl;
	if (dumps.size()>0) {
		if (forward.load()) {
			index.store(0);
		} else {
			index.store(dumps.size()-1);
		}
	} else {
		// Default value (no data available)
		index.store(0);
	}
	wakeUpReplayThread();
}
bool DumpPlayer::setIndex(const size_t &newIndex) {
	// Check for data range
	if (newIndex>=dumps.size()) {
		return false;
	}
	if (DEBUG) std::cout << LOGTAG << "Setting index to " << newIndex << std::endl;
	index.store(newIndex);
	wakeUpReplayThread();
	return true;
}
size_t DumpPlayer::getIndex() {
	return index.load();
}

void DumpPlayer::setDirectionForward(const bool &reset) {
	if (DEBUG) std::cout << LOGTAG << "Setting replay direction to forward" << std::endl;
	forward.store(true);
	if (reset) {
		resetIndex();		// wakeUpReplayThread() not called explicitly because resetIndex() calls it internally
	} else {
		wakeUpReplayThread();
	}
}
void DumpPlayer::setDirectionBackwards(const bool &reset) {
	if (DEBUG) std::cout << LOGTAG << "Setting replay direction to backwards" << std::endl;
	forward.store(false);
	if (reset) {
		resetIndex();		// wakeUpReplayThread() not called explicitly because resetIndex() calls it internally
	} else {
		wakeUpReplayThread();
	}
}
bool DumpPlayer::isDirectionForward() {
	return forward.load();
}

void DumpPlayer::setSpeed(const double &speed) {
	if (DEBUG) std::cout << LOGTAG << "Setting replay speed to " << speed << "x" << std::endl;
	timeFactor.store(1/speed);
	wakeUpReplayThread();
}
double DumpPlayer::getSpeed() {
	return 1/timeFactor.load();
}


// Data functions
bool DumpPlayer::getDump(GenericDumper::Dump &dump,const size_t &n) {
	// Check for data range
	if (n>=dumps.size()) {
		return false;
	}
	dump=dumps[n];
	return true;
}
std::chrono::nanoseconds DumpPlayer::getDumpTimestamp(const size_t &n) {
	// Check for data range
	if (n>=dumps.size()) {
		return std::chrono::nanoseconds();
	}
	return dumps[n].timestamp;
}

size_t DumpPlayer::findNext(const std::function<bool(const GenericDumper::Dump &)> &match,const size_t &fromIndex,const bool &searchForward) {
	// Check for data range
	const size_t nDumps=dumps.size();
	if (fromIndex>=nDumps) {
		return fromIndex;
	}
	
	size_t current=fromIndex;
	while (!match(dumps[current])) {
		// Locate next dump
		if (searchForward) {
			if (current>=(nDumps-1)) {
				break;
			}
			current++;
		} else {
			if (current<=0) {
				break;
			}
			current--;
		}
	}
	return current;
}
size_t DumpPlayer::findNext(const short &type,const size_t &fromIndex,const bool &searchForward) {
	if (DEBUG) std::cout << LOGTAG << "Looking for a dump of type " << type << " from index " << fromIndex << std::endl;
	return findNext([type](const GenericDumper::Dump &dump) {
		return dump.type==type;
	},fromIndex,searchForward);
}
size_t DumpPlayer::jumpTo(const std::chrono::nanoseconds &diff,const size_t &fromIndex) {
	if (DEBUG) std::cout << LOGTAG << "Looking for the first dump at " << diff.count() << " ns from index " << fromIndex << std::endl;
	
	// Check for data range
	if (fromIndex>=dumps.size()) {
		return fromIndex;
	}
	
	const bool searchForward=(diff>=diff.zero());
	const std::chrono::nanoseconds timeLimit=dumps[fromIndex].timestamp+diff;
	return findNext([timeLimit,searchForward](const GenericDumper::Dump &dump) {
		if (searchForward) {
			return dump.timestamp>=timeLimit;
		} else {
			return dump.timestamp<=timeLimit;
		}
	},fromIndex,searchForward);
}
size_t DumpPlayer::jumpTo(const std::chrono::nanoseconds &diff) {
	return jumpTo(diff,index.load());
}


// Additional functions
std::chrono::system_clock::time_point DumpPlayer::getReplayClockTime(const std::chrono::nanoseconds &timestamp) {
	return timeReference+timestamp;
}
std::string DumpPlayer::getReplayClockTimeString(const std::chrono::nanoseconds &timestamp) {
	return GenericDumper::getClockTimeString(timeReference+timestamp);
}

std::chrono::nanoseconds DumpPlayer::getReplayDuration() {
	// Check for available data
	const size_t nDumps=dumps.size();
	if (nDumps==0) {
		return std::chrono::nanoseconds();
	}
	return dumps[nDumps-1].timestamp;
}
std::string DumpPlayer::getReplayDurationString() {
	std::chrono::nanoseconds duration=getReplayDuration();
	const auto hours=std::chrono::duration_cast<std::chrono::hours>(duration);
	duration-=hours;
	const auto minutes=std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration-=minutes;
	const auto seconds=std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration-=seconds;
	
	std::ostringstream out;
	out << hours.count() << "h " << minutes.count() << "m " << seconds.count() << "s";
	return out.str();
}

