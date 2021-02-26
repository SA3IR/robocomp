#ifndef RC_WORLD_DUMPER_H
#define RC_WORLD_DUMPER_H


#include "genericDumper.h"
#include <AGMWorldModel.h>
#include <genericworker.h>
#include <AGMExecutive.h>

#include <Ice/OutputStream.h>
#include <Ice/InputStream.h>
#include <Ice/LocalException.h>


class RoboCompDumper: public GenericDumper, public DumpPlayer {

public:
	using DumpPlayer::setDumpEventCallback;
	
	// Single dump functions
	template<typename T> bool writeDump(const T &object);
	bool readDump(Dump &dump);
	
	// Full file functions
	std::vector<Dump> readFullDumpFile(const std::string &filename);
	
	
	// DumpPlayer functions
	size_t findNextWorld(const size_t &fromIndex);
	size_t findPreviousWorld(const size_t &fromIndex);
	
	// DumpPlayer callback helpers
	bool setDumpEventCallback(GenericWorker *worker);
	bool setDumpEventCallback(const RoboCompAGMExecutive::AGMExecutivePrx &agmexecutive_proxy);
	
	static void handleToWorker(GenericWorker *worker,const Dump &dump);
	static void sendToExecutive(const RoboCompAGMExecutive::AGMExecutivePrx &agmexecutive_proxy,const Dump &dump);
	
	
private:
	
	// Debug
	static const bool DEBUG=false;										///< Constant value that allows the compiler to exclude log code.
	static constexpr const char* LOGTAG="RoboCompDumper: ";				///< Defines a log tag with a different name per class.
	
	
	// Constants
	static const short UNKNOWN_TYPE=0;
	static const short WORLD_TYPE=1;
	static const short NODE_TYPE=2;
	static const short EDGE_TYPE=3;
	static const short NODE_SEQUENCE_TYPE=4;
	static const short EDGE_SEQUENCE_TYPE=5;
	
	
	// Additional functions
	template<typename T> static short getType(const T &object);
	static std::string getTypeName(const short &type);
	
	std::shared_ptr<void> static regenerateObject(const short &type,const std::vector<Byte> &data);
	template<typename T> static std::shared_ptr<T> regenerateObjectT(const std::vector<Byte> &data);
	
	
	// Debug functions
	static bool printObject(const short &type,const std::shared_ptr<void> &sptr,const bool &includeAttributes,const std::string &prefix="");
	static void printObject(const RoboCompAGMWorldModel::World &world,const bool &includeAttributes,const std::string &prefix="");
	static void printObject(const RoboCompAGMWorldModel::Node &node,const bool &includeAttributes,const std::string &prefix="");
	static void printObject(const RoboCompAGMWorldModel::Edge &edge,const bool &includeAttributes,const std::string &prefix="");
	static void printObject(const RoboCompAGMWorldModel::NodeSequence &nodeSequence,const bool &includeAttributes,const std::string &prefix="");
	static void printObject(const RoboCompAGMWorldModel::EdgeSequence &edgeSequence,const bool &includeAttributes,const std::string &prefix="");
	static void printAttributes(const RoboCompAGMWorldModel::StringDictionary &attributes,const std::string &prefix="");
	
	
};
#endif



