#ifndef GENERIC_DUMPER_H
#define GENERIC_DUMPER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <memory>
#include <functional>

#include <thread>
#include <atomic>
#include <condition_variable>


class GenericDumper {

public:
	
	// Byte definition to manage data
	using Byte=uint8_t; 
	
	// Types to code Dump File content
	using Head=char[16];												///< Header: Head signature
	using ClockTimeReference=int64_t;									///< Header: Time reference (ns since epoch)
	using DumpTimestamp=int64_t;										///< Dump: Timestamp (ns since time reference)
	using DumpType=int16_t;												///< Dump: Data type
	using DumpSize=int32_t;												///< Dump: Data size (bytes)
	
	// Dump definition
	struct Dump {
		std::chrono::nanoseconds timestamp;
		short type;
		std::vector<Byte> data;
		std::shared_ptr<void> object;
	};
	
	
	// Functions
	GenericDumper();
	~GenericDumper();
	
	
	// Manage dump file
	bool createDumpFile(const std::string &filename);
	bool openDumpFile(const std::string &filename);
	bool flushDumpFile();
	void closeDumpFile();
	
	std::chrono::system_clock::time_point getDumpFileTime();
	
	
	// Single dump functions
	bool writeDump(const DumpTimestamp &timestamp,const DumpType &type,const Byte* data,const DumpSize &size);
	bool writeDump(const DumpType &type,const Byte* data,const DumpSize &size);
	bool writeDump(const DumpType &type,const std::vector<Byte> &data);
	
	bool readDump(Dump &dump,const std::function<std::shared_ptr<void>(const short &,const std::vector<Byte> &)> &regenerateObject=nullptr);
	
	
	// Full file functions
	std::vector<Dump> readFullDumpFile(const std::string &filename,const std::function<std::shared_ptr<void>(const short &,const std::vector<Byte> &)> &regenerateObject);
	
	
	// Helper functions
	static std::string getClockTimeString(const std::chrono::system_clock::time_point &timePoint,const bool &subseconds=true);
	
	
protected:
	
	
private:
	
	// Debug
	static const bool DEBUG=false;										///< Constant value that allows the compiler to exclude log code.
	static constexpr const char* LOGTAG="GenericDumper: ";				///< Defines a log tag with a different name per class.
	
	
	// Constants
	const Head HEAD="DumpFile";
	
	
	// Variables
	std::fstream dumpFile;												///< Underlying file handler
	std::chrono::system_clock::time_point referenceClockTime;			///< Time reference (clock) for dump timestamps
	std::chrono::steady_clock::time_point referenceTime;				///< Time reference (monotonic) to calculate timestamps on write dumps
	
	
};




class DumpPlayer {

public:
	
	// Functions
	DumpPlayer();
	~DumpPlayer();
	
	
	// Player functions
	void startReplaySync();
	bool startReplay();
	void stopReplay(const bool &reset=true);
	
	
	// Configuration functions
	bool setReplayContent(const std::vector<GenericDumper::Dump> &dumps);
	bool setReplayTimeReference(const std::chrono::system_clock::time_point &timeReference);
	bool setDumpEventCallback(const std::function<void(const GenericDumper::Dump &,const size_t &)> &callback);
	bool setReplayFinishingCallback(const std::function<void()> &callback);
	
	// Controls functions
	void resetIndex();
	bool setIndex(const size_t &newIndex);
	size_t getIndex();
	
	void setDirectionForward(const bool &reset=true);
	void setDirectionBackwards(const bool &reset=true);
	bool isDirectionForward();
	
	void setSpeed(const double &speed);
	double getSpeed();
	
	// Data functions
	bool getDump(GenericDumper::Dump &dump,const size_t &n);
	std::chrono::nanoseconds getDumpTimestamp(const size_t &n);
	
	size_t findNext(const std::function<bool(const GenericDumper::Dump &)> &match,const size_t &fromIndex,const bool &forward=true);
	size_t findNext(const short &type,const size_t &fromIndex,const bool &forward);
	
	size_t jumpTo(const std::chrono::nanoseconds &diff,const size_t &fromIndex);
	size_t jumpTo(const std::chrono::nanoseconds &diff);
	
	
	// Additional functions
	std::chrono::system_clock::time_point getReplayClockTime(const std::chrono::nanoseconds &timestamp);
	std::string getReplayClockTimeString(const std::chrono::nanoseconds &timestamp);
	
	std::chrono::nanoseconds getReplayDuration();
	std::string getReplayDurationString();
	
	
private:
	
	// Debug
	static const bool DEBUG=false;										///< Constant value that allows the compiler to exclude log code.
	static const bool DEBUG_REPLAY_TIME=false;							///< Constant value that allows the compiler to exclude code to check replay time.
	static constexpr const char* LOGTAG="DumpPlayer: ";					///< Defines a log tag with a different name per class.
	
	
	// Player functions
	void wakeUpReplayThread();
	
	
	// Variables
	std::vector<GenericDumper::Dump> dumps;
	std::chrono::system_clock::time_point timeReference;
	
	std::function<void(const GenericDumper::Dump &,const size_t &)> onDumpEvent;
	std::function<void()> onReplayFinishing;
	
	std::thread replayThread;
	std::atomic<bool> isRunning;
	std::condition_variable conditionVariable;
	std::mutex replayMutex;
	std::atomic<bool> wakeUp;
	
	std::atomic<size_t> index;
	std::atomic<bool> forward;
	std::atomic<double> timeFactor;
	std::atomic<bool> stop;
	
	
};
#endif
