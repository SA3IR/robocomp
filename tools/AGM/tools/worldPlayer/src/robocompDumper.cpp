#include "robocompDumper.h"


// Force generate specific template implementations for RoboCompAGMWorldModel objects
template bool RoboCompDumper::writeDump<RoboCompAGMWorldModel::World>(const RoboCompAGMWorldModel::World &object);
template bool RoboCompDumper::writeDump<RoboCompAGMWorldModel::Node>(const RoboCompAGMWorldModel::Node &object);
template bool RoboCompDumper::writeDump<RoboCompAGMWorldModel::Edge>(const RoboCompAGMWorldModel::Edge &object);
template bool RoboCompDumper::writeDump<RoboCompAGMWorldModel::NodeSequence>(const RoboCompAGMWorldModel::NodeSequence &object);
template bool RoboCompDumper::writeDump<RoboCompAGMWorldModel::EdgeSequence>(const RoboCompAGMWorldModel::EdgeSequence &object);


// Single dump functions
template<typename T> bool RoboCompDumper::writeDump(const T &object) {
	// Get object type
	const short type=getType(object);
	
	// Get object data
	Ice::OutputStream outStream;
	outStream.write(object);
	const std::pair<const Byte*,const Byte*> data=outStream.finished();
	
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "Writing RoboComp dump: " << std::endl;
		std::cout << LOGTAG << " - Object: " << getTypeName(type) << " (type " << type << ")" << std::endl;
		printObject(object,false,"   - ");
	}
	
	// Write RoboComp dump
	return GenericDumper::writeDump(type,data.first,data.second-data.first);
}
bool RoboCompDumper::readDump(Dump &dump) {
	// Read binary dump from file
	if (!GenericDumper::readDump(dump,&regenerateObject)) {
		return false;
	}
	
	// Debug messages
	if (DEBUG) {
		std::cout << LOGTAG << "RoboComp dump read: " << std::endl;
		std::cout << LOGTAG << " - Object: " << getTypeName(dump.type) << " (type " << dump.type << ")" << std::endl;
		printObject(dump.type,dump.object,false,"   - ");
	}
	
	return true;
}

// Full file functions
std::vector<RoboCompDumper::Dump> RoboCompDumper::readFullDumpFile(const std::string &filename) {
	return GenericDumper::readFullDumpFile(filename,&regenerateObject);
}


// Additional functions
template<typename T> short RoboCompDumper::getType(const T &object) {
	if (typeid(T)==typeid(RoboCompAGMWorldModel::World)) {					return WORLD_TYPE;
	} else if (typeid(T)==typeid(RoboCompAGMWorldModel::Node)) {			return NODE_TYPE;
	} else if (typeid(T)==typeid(RoboCompAGMWorldModel::Edge)) {			return EDGE_TYPE;
	} else if (typeid(T)==typeid(RoboCompAGMWorldModel::NodeSequence)) {	return NODE_SEQUENCE_TYPE;
	} else if (typeid(T)==typeid(RoboCompAGMWorldModel::EdgeSequence)) {	return EDGE_SEQUENCE_TYPE;
	} else {
		std::cout << "WARNING: Unknown object type" << std::endl;
		return UNKNOWN_TYPE;
	}
}
std::string RoboCompDumper::getTypeName(const short &type) {
	switch (type) {
		case WORLD_TYPE:			return "RoboCompAGMWorldModel::World";
		case NODE_TYPE:				return "RoboCompAGMWorldModel::Node";
		case EDGE_TYPE:				return "RoboCompAGMWorldModel::Edge";
		case NODE_SEQUENCE_TYPE:	return "RoboCompAGMWorldModel::NodeSequence";
		case EDGE_SEQUENCE_TYPE:	return "RoboCompAGMWorldModel::EdgeSequence";
		case UNKNOWN_TYPE:			return "Unknown type";
		default:
			std::cout << "WARNING: Object type " << type << " not recognized" << std::endl;
			return "Type not recognized";
	}
}

std::shared_ptr<void> RoboCompDumper::regenerateObject(const short &type,const std::vector<Byte> &data) {
	switch (type) {
		case WORLD_TYPE:			return regenerateObjectT<RoboCompAGMWorldModel::World>(data);
		case NODE_TYPE:				return regenerateObjectT<RoboCompAGMWorldModel::Node>(data);
		case EDGE_TYPE:				return regenerateObjectT<RoboCompAGMWorldModel::Edge>(data);
		case NODE_SEQUENCE_TYPE:	return regenerateObjectT<RoboCompAGMWorldModel::NodeSequence>(data);
		case EDGE_SEQUENCE_TYPE:	return regenerateObjectT<RoboCompAGMWorldModel::EdgeSequence>(data);
		case UNKNOWN_TYPE:
			std::cout << "WARNING: Object not regenerated, unknown type" << std::endl;
			return nullptr;
		default:
			std::cout << "ERROR: Could not regenerate object, type " << type << " not recognized" << std::endl;
			return nullptr;
	}
}
template<typename T> std::shared_ptr<T> RoboCompDumper::regenerateObjectT(const std::vector<Byte> &data) {
	std::shared_ptr<T> object=std::make_shared<T>();
	Ice::InputStream inStream(data);
	try {
		inStream.read(*object);
	} catch (Ice::UnmarshalOutOfBoundsException &e) {
		std::cout << "ERROR: Could not regenerate object, corrupted data" << std::endl;
		return nullptr;
	}
	return object;
}


// Debug functions
bool RoboCompDumper::printObject(const short &type,const std::shared_ptr<void> &object,const bool &includeAttributes,const std::string &prefix) {
	if (object!=nullptr) {
		switch (type) {
			case WORLD_TYPE:			printObject(*std::static_pointer_cast<RoboCompAGMWorldModel::World>(object),includeAttributes,prefix); break;
			case NODE_TYPE:				printObject(*std::static_pointer_cast<RoboCompAGMWorldModel::Node>(object),includeAttributes,prefix); break;
			case EDGE_TYPE:				printObject(*std::static_pointer_cast<RoboCompAGMWorldModel::Edge>(object),includeAttributes,prefix); break;
			case NODE_SEQUENCE_TYPE:	printObject(*std::static_pointer_cast<RoboCompAGMWorldModel::NodeSequence>(object),includeAttributes,prefix); break;
			case EDGE_SEQUENCE_TYPE:	printObject(*std::static_pointer_cast<RoboCompAGMWorldModel::EdgeSequence>(object),includeAttributes,prefix); break;
			case UNKNOWN_TYPE:
				std::cout << "WARNING: Could not print object, unknown type" << std::endl;
				break;
			default:
				std::cout << "WARNING: Could not print object, type " << type << " not recognized" << std::endl;
				return false;
		}
	} else {
		std::cout << LOGTAG << "Object not available (nullptr)" << std::endl;
	}
	return true;
}
void RoboCompDumper::printObject(const RoboCompAGMWorldModel::World &world,const bool &includeAttributes,const std::string &prefix) {
	if (DEBUG) std::cout << LOGTAG;
	std::cout << prefix << "World v" << world.version << ": " << world.nodes.size() << " nodes, " << world.edges.size() << " edges" << std::endl;
	printObject(world.nodes,includeAttributes,"  "+prefix);
	printObject(world.edges,includeAttributes,"  "+prefix);
}
void RoboCompDumper::printObject(const RoboCompAGMWorldModel::Node &node,const bool &includeAttributes,const std::string &prefix) {
	if (DEBUG) std::cout << LOGTAG;
	std::cout << prefix << "Node id " << node.nodeIdentifier << ", type " << node.nodeType << std::endl;
	if (includeAttributes) {
		printAttributes(node.attributes,"  "+prefix);
	}
}
void RoboCompDumper::printObject(const RoboCompAGMWorldModel::Edge &edge,const bool &includeAttributes,const std::string &prefix) {
	if (DEBUG) std::cout << LOGTAG;
	std::cout << prefix << "Edge link " << edge.a << " -> " << edge.b << std::endl;
	if (includeAttributes) {
		printAttributes(edge.attributes,"  "+prefix);
	}
}
void RoboCompDumper::printObject(const RoboCompAGMWorldModel::NodeSequence &nodeSequence,const bool &includeAttributes,const std::string &prefix) {
	for (const RoboCompAGMWorldModel::Node &node: nodeSequence) {
		if (DEBUG) std::cout << LOGTAG;
		std::cout << prefix << "Node id " << node.nodeIdentifier << ", type " << node.nodeType << std::endl;
		if (includeAttributes) {
			printAttributes(node.attributes,"  "+prefix);
		}
	}
}
void RoboCompDumper::printObject(const RoboCompAGMWorldModel::EdgeSequence &edgeSequence,const bool &includeAttributes,const std::string &prefix) {
	for (const RoboCompAGMWorldModel::Edge &edge: edgeSequence) {
		if (DEBUG) std::cout << LOGTAG;
		std::cout << prefix << "Edge link " << edge.a << " -> " << edge.b << std::endl;
		if (includeAttributes) {
			printAttributes(edge.attributes,"  "+prefix);
		}
	}
}
void RoboCompDumper::printAttributes(const RoboCompAGMWorldModel::StringDictionary &attributes,const std::string &prefix) {
	for (const std::pair<std::string,std::string> attribute: attributes) {
		if (DEBUG) std::cout << LOGTAG;
		std::cout << prefix << attribute.first << ": " << attribute.second << std::endl;
	}
}




//####################################################################################################################################################
// DumpPlayer functions
//####################################################################################################################################################


size_t RoboCompDumper::findNextWorld(const size_t &fromIndex) {
	if (DEBUG) std::cout << LOGTAG << "Looking for the next world from index " << fromIndex << std::endl;
	return findNext(static_cast<short>(WORLD_TYPE),fromIndex,true);
}
size_t RoboCompDumper::findPreviousWorld(const size_t &fromIndex) {
	if (DEBUG) std::cout << LOGTAG << "Looking for the previous world from index " << fromIndex << std::endl;
	return findNext(static_cast<short>(WORLD_TYPE),fromIndex,false);
}

// DumpPlayer callback helpers
bool RoboCompDumper::setDumpEventCallback(GenericWorker *worker) {
	// Choose specific event callback
	return DumpPlayer::setDumpEventCallback(std::bind(&handleToWorker,worker,std::placeholders::_1));
}
bool RoboCompDumper::setDumpEventCallback(const RoboCompAGMExecutive::AGMExecutivePrx &agmexecutive_proxy) {
	// Choose specific event callback
	return DumpPlayer::setDumpEventCallback(std::bind(&sendToExecutive,agmexecutive_proxy,std::placeholders::_1));
}

void RoboCompDumper::handleToWorker(GenericWorker *worker,const Dump &dump) {
	if (DEBUG) {
		std::cout << LOGTAG << "Handling dump object to worker (" << getTypeName(dump.type) << ")" << std::endl;
		printObject(dump.type,dump.object,false," - ");
	}
	switch (dump.type) {
		case RoboCompDumper::WORLD_TYPE:			worker->structuralChange(*std::static_pointer_cast<RoboCompAGMWorldModel::World>(dump.object)); break;
		case RoboCompDumper::NODE_TYPE:				worker->symbolUpdated(*std::static_pointer_cast<RoboCompAGMWorldModel::Node>(dump.object)); break;
		case RoboCompDumper::EDGE_TYPE:				worker->edgeUpdated(*std::static_pointer_cast<RoboCompAGMWorldModel::Edge>(dump.object)); break;
		case RoboCompDumper::NODE_SEQUENCE_TYPE:	worker->symbolsUpdated(*std::static_pointer_cast<RoboCompAGMWorldModel::NodeSequence>(dump.object)); break;
		case RoboCompDumper::EDGE_SEQUENCE_TYPE:	worker->edgesUpdated(*std::static_pointer_cast<RoboCompAGMWorldModel::EdgeSequence>(dump.object)); break;
		case RoboCompDumper::UNKNOWN_TYPE:
		default:
			std::cout << "WARNING: Could not handle dump object: Unknown type" << std::endl;
	}
}
void RoboCompDumper::sendToExecutive(const RoboCompAGMExecutive::AGMExecutivePrx &agmexecutive_proxy,const Dump &dump) {
	if (DEBUG) {
		std::cout << LOGTAG << "Sending dump object to AGMExecutive (" << getTypeName(dump.type) << ")" << std::endl;
		printObject(dump.type,dump.object,false," - ");
	}
	switch (dump.type) {
		case RoboCompDumper::WORLD_TYPE:			agmexecutive_proxy->structuralChangeProposal(*std::static_pointer_cast<RoboCompAGMWorldModel::World>(dump.object),"DumpPlayer",""); break;
		case RoboCompDumper::NODE_TYPE:				agmexecutive_proxy->symbolUpdate(*std::static_pointer_cast<RoboCompAGMWorldModel::Node>(dump.object)); break;
		case RoboCompDumper::EDGE_TYPE:				agmexecutive_proxy->edgeUpdate(*std::static_pointer_cast<RoboCompAGMWorldModel::Edge>(dump.object)); break;
		case RoboCompDumper::NODE_SEQUENCE_TYPE:	agmexecutive_proxy->symbolsUpdate(*std::static_pointer_cast<RoboCompAGMWorldModel::NodeSequence>(dump.object)); break;
		case RoboCompDumper::EDGE_SEQUENCE_TYPE:	agmexecutive_proxy->edgesUpdate(*std::static_pointer_cast<RoboCompAGMWorldModel::EdgeSequence>(dump.object)); break;
		case RoboCompDumper::UNKNOWN_TYPE:
		default:
			std::cout << "WARNING: Could not send dump object: Unknown type" << std::endl;
	}
}
