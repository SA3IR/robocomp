import string
import md5
import sys
import traceback
import re

def digest(pp):
	md = md5.new()
	md.update(pp)
	checksum = md.hexdigest()
	return str(checksum)


class PlanningCache:
	def __init__(self):
		print 'PlanningCache::init()'
		self.data = dict()
		self.availableId = 0
		try:
			while True:
				#print 'Try including', self.availableId
				Df    = open('cache_D'+str(self.availableId)+'.py', 'r')
				D = Df.read()
				Df.close()
				If    = open('cache_I'+str(self.availableId)+'.xml', 'r')
				I = If.read()
				If.close()
				Gf    = open('cache_G'+str(self.availableId)+'.py', 'r')
				G = Gf.read()
				Gf.close()
				planf = open('cache_plan'+str(self.availableId)+'.agglp', 'r')
				plan = planf.read()
				ret = True
				success = (plan=='fail')
				if not success:
					ret = False
				self.include(D, I, G, plan, success, False)
				#print 'Read planning context', self.availableId
				planf.close()
		except IOError:
			print 'can\'t open', self.availableId
			pass
		except:
			traceback.print_exc()
	def include(self, domain, initialstate, goalstate, plan, success, shouldIWrite=True):
		domain = string.rstrip(domain, '\n')
		initialstate = string.rstrip(initialstate, '\n')
		goalstate = string.rstrip(goalstate, '\n')
		#try:
		if shouldIWrite:
			try:
				#print 'PLAN: ', plan
				D = open('cache_D'+str(self.availableId)+'.py', 'w')
				D.write(domain)
				D.close()
				I = open('cache_I'+str(self.availableId)+'.xml', 'w')
				I.write(initialstate)
				I.close()
				G = open('cache_G'+str(self.availableId)+'.py', 'w')
				G.write(goalstate)
				G.close()
				P = open('cache_plan'+str(self.availableId)+'.agglp', 'w')
				if success: P.write(plan)
				else: P.write('fail')
				P.close()
			except:
				print 'Can\'t write cache'
				traceback.print_exc()
				sys.exit(-1)
		self.availableId += 1
		md = md5.new()
		md.update(domain+initialstate+goalstate)
		checksum = md.hexdigest()
		#print 'Including planning context with checksum', checksum
		try:
			self.data[checksum].append((domain, initialstate, goalstate, plan, success))
		except:
			self.data[checksum] = []
			self.data[checksum].append((domain, initialstate, goalstate, plan, success))
		return True

	def getPlan(self, domain, initialstate, goalstate):
		domain = string.rstrip(domain, '\n')
		initialstate = string.rstrip(initialstate, '\n')
		goalstate = string.rstrip(goalstate, '\n')
		md = md5.new()
		md.update(domain+initialstate+goalstate)
		checksum = md.hexdigest()
		print 'Query cache, checksum', checksum
		if checksum in self.data.keys():
			lis = self.data[checksum]
			print 'Match for the checksum'
			for i in lis:
				if i[0] == domain and i[1] == initialstate and i[2] == goalstate:
					if i[3] == 'fail':
						return False, i[3]
					else:
						print 'GOT PLAN FROM CACHE'
						return True, i[3]
			print 'No exact cache match'
			return None
		else:
			#print self.data.keys()
			print 'No cache for the checksum'
			return None

	def getPlanFromFiles(self, domain, initialstate, goalstate):
		initialstate = re.sub('<attribute [^>]*>', '', initialstate)
		initialstate = '\n'.join([ x for x in initialstate.split("\n") if len(x.strip())>0 ])
		goalstate    = re.sub('<attribute [^>]*>', '', goalstate)
		goalstate = '\n'.join([ x for x in goalstate.split("\n") if len(x.strip())>0 ])
		return self.getPlan(domain, initialstate, goalstate)

	def includeFromFiles(self, domain, initialstate, goalstate, plan, success, shouldIWrite=True):
		initialstate = re.sub('<attribute [^>]*>', '', initialstate)
		initialstate = '\n'.join([ x for x in initialstate.split("\n") if len(x.strip())>0 ])
		goalstate    = re.sub('<attribute [^>]*>', '', goalstate)
		goalstate = '\n'.join([ x for x in goalstate.split("\n") if len(x.strip())>0 ])
		return self.include(domain, initialstate, goalstate, plan, success, shouldIWrite)
