#sudo apt-get install pypy pypy-setuptools
#git clone https://github.com/eleme/thriftpy.git
#cd thriftpy
#make sudo pypy setup.py install

#from thriftpy.protocol.binary import TBinaryProtocolFactory
#from thriftpy.transport.buffered import TBufferedTransportFactory
#from thriftpy.transport.framed import TFramedTransportFactory

import thriftpy
agglplanner_thrift = thriftpy.load("/usr/local/share/agm/agglplanner.thrift", module_name="agglplanner_thrift")
from thriftpy.rpc import make_client

client = make_client(agglplanner_thrift.AGGLPlanner, '127.0.0.1', 6000)


print 'Get domain id...'
domainText = open('/home/robocomp/robocomp/components/robocomp-shelly/files/planningDomain/domain_min.aggl', 'r').read()
domainId = client.getDomainIdentifier(domainText)
print domainId

print 'Reading init world...'
initWorld = open('/home/robocomp/robocomp/components/robocomp-shelly/etc/initialModel_hybrid.xml', 'r').read()

print 'Get target id...'
targetText = open('/home/robocomp/robocomp/components/robocomp-shelly/etc/targetRestPosition.aggt', 'r').read()
print type(targetText), targetText

targetId = client.getTargetIdentifier(targetText)
print targetId
print 'Calling planner...'
jobIdentifier = client.startPlanning(domainId, initWorld, targetId, [], [])
print 'got job identifier', jobIdentifier
print 'Asking for results...'
result = client.getPlanningResults(jobIdentifier)
print result.plan
