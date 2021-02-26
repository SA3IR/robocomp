#!/usr/bin/env pypy



#sudo apt-get install pypy pypy-setuptools cython
#git clone https://github.com/eleme/thriftpy.git
#cd thriftpy
#sudo pypy setup.py install
#sudo make clean
#sudo python setup.py install

#from thriftpy.protocol.binary import TBinaryProtocolFactory
#from thriftpy.transport.buffered import TBufferedTransportFactory
#from thriftpy.transport.framed import TFramedTransportFactory

import logging
logging.basicConfig(level=logging.DEBUG)

import thriftpy
agglplanner_thrift = thriftpy.load("/usr/local/share/agm/agglplanner.thrift", module_name="agglplanner_thrift")

from thriftpy.rpc import make_server


import sys, imp, traceback, thread


sys.path.append('/usr/local/share/agm/')
import agglplanner2
from agglplanningcache import *

class JobObject(object):
	def __init__(self, identifier, jobInstance, domainId, targetId):
		self.lock = thread.allocate_lock()
		self.identifier = identifier
		self.jobInstance = jobInstance
		self.domainId = domainId
		self.targetId = targetId


class Worker(object):
	mapsLock = thread.allocate_lock()
	lastUsedDomainKey = 0
	domainMap = {}
	lastUsedTargetKey = 0
	targetMap = {}
	lastUsedJobKey = 0
	jobMap = {}
	cache = PlanningCache()

	def getDomainIdentifier(self, domainText):
		print '------------------------------------------------------------------------'
		print 'getDomainIdentifier domainText(', type(domainText), ')'
		try:
			self.mapsLock.acquire()
			ret = self.lastUsedDomainKey + 1
			self.domainMap[ret] = agglplanner2.DomainInformation(ret, domainText)
			self.lastUsedDomainKey += 1
			return ret
		except:
			traceback.print_exc()
			return -1
		finally:
			self.mapsLock.release()
			print '------------------------------------------------------------------------'
		sys.exit(-1) # Should not get here

	def getTargetIdentifier(self, targetText, domainId):
		print '------------------------------------------------------------------------'
		print 'getTargetIdentifier targetText(', type(targetText), ')'
		self.mapsLock.acquire()
		try:
			ret = self.lastUsedTargetKey + 1
			print 'ret:', ret
			print 'targetText', targetText
			print 'domainId', domainId
			self.targetMap[ret] = agglplanner2.TargetInformation(ret, targetText, self.domainMap[domainId].parsed)
			print 'O', self.targetMap[ret]
			print 'TargetInformation', self.targetMap[ret]
			open("targetText.txt", 'w').write(targetText)
			open("targetCode.py", 'w').write(self.targetMap[ret].code)
			self.lastUsedTargetKey += 1
			return ret
		except:
			f = open("targetCode.py", 'w')
			f.write(targetText)
			f.close()
			print 'Wrote erroneoous code to targetCode.py'
			traceback.print_exc()
			print 'Wrote erroneoous code to targetCode.py'
			return -1
		finally:
			self.mapsLock.release()
			print '------------------------------------------------------------------------'
		sys.exit(-1) # Should not get here

	def startPlanning(self, domainId, initWorld, targetId, excludeList, awakenRules):
		print '------------------------------------------------------------------------'
		print 'startPlanning', domainId, '(', type(domainId), ')', 'initWorld(', type(initWorld), ')', targetId, '(', type(targetId), ')', excludeList, '(', type(excludeList), ')', awakenRules, '(', type(awakenRules), ')'
		try:
			self.mapsLock.acquire()
			ret = self.lastUsedJobKey + 1
			dI = self.domainMap[domainId]
			tI = self.targetMap[targetId]
			planningObject = agglplanner2.AGGLPlanner2(dI.parsed, dI.module, initWorld, (tI.module, tI.variables, tI.text), 'none')
			self.jobMap[ret] = JobObject(ret, planningObject, domainId, targetId)
			self.lastUsedJobKey += 1
			return ret
		except:
			open("error.txt", 'w').write(self.targetMap[targetId].code)
			traceback.print_exc()
			return -1
		finally:
			self.mapsLock.release()
			print '------------------------------------------------------------------------'


	def forceStopPlanning(self, jobIdentifier):
		print '------------------------------------------------------------------------'
		print 'forceStopPlanning', jobIdentifier, '(', type(jobIdentifier), ')'
		try:
			self.mapsLock.acquire()
			job = self.jobMap[jobIdentifier]
			job.jobInstance.setStopFlag()
			return 0
		except:
			traceback.print_exc()
			return 1
		finally:
			self.mapsLock.release()
			print '------------------------------------------------------------------------'
		sys.exit(-1) # Should not get here

	def getPlanningResults(self, jobIdentifier):
		print '------------------------------------------------------------------------'
		print 'getPlanningResults', jobIdentifier, '(', type(jobIdentifier), ')'
		ret = agglplanner_thrift.PlanningResults()
		try:
			self.mapsLock.acquire()
			job = self.jobMap[jobIdentifier]
			dI = self.domainMap[job.domainId]
			tI = self.targetMap[job.targetId]
		except:
			print 'ERROR AQUI 1'
			traceback.print_exc()
			raise
		finally:
			self.mapsLock.release()
		#
		# First: try with the cache
		#
		try:
			print 'e'
			cacheResult = self.cache.getPlanFromFiles(dI.text, job.jobInstance.initWorld.graph.toXMLString(), tI.code)
			print 'f', cacheResult
			if cacheResult:
				if len(cacheResult[1].strip()) == 0:
					print 'f2'
					cacheResult = False
			print 'g'
		except:
			traceback.print_exc()
			print 'h'
			print 'ERROR AQUI 2'
			cacheResult = False
		try:
			if cacheResult:
				print 'Got plan from cache'
				print '<<<'
				print cacheResult[1]
				print '>>>'
				cacheSuccess = cacheResult[0]
				cachePlan = cacheResult[1]
				# lines = cacheResult[1].split('\n')
				ret.plan = str(cacheResult[1])
				ret.cost = 1
				print 'salimos por cache'
				return ret
		except:
			print 'ERROR AQUI 234534'
		#
		# Second: try planning
		#
		try:
			plan = job.jobInstance.run()
			self.cache.includeFromFiles(dI.text, job.jobInstance.initWorld.graph.toXMLString(), tI.code, str(plan), True)
			ret.plan = str(plan)
			ret.cost = 1
			print 'PLAN:', ret.plan
			return ret
		except:
			print 'ERROR AQUI 3'
			traceback.print_exc()
			print 'ERROR AQUI 3'
			ret.plan = 'errrorr'
			ret.cost = 100
			return ret
		finally:
			print '--------------------------------------------------'
		print 'WE SHOULD NOT GET HERE'
		sys.exit(-1)
#PlanResult planHierarchical(1: i32 domainId, 2: string initWorld, 3:i32 targetId, 4: list<string> excludeList, 5: list<string> awakenRules, 6: map<string,string> symbolMapping) throws (1: string theError),




server = make_server(agglplanner_thrift.AGGLPlanner, Worker(), '127.0.0.1', 6000, client_timeout=None)
server.serve()
