import sys
from tempParser import Parser
initFile = sys.argv[1]
# targetFile = sys.argv[2]

p = Parser()
print(p.parse_initM(initFile))

