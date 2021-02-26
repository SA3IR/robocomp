from parseAGGL import AGMFileDataParsing


class NoPredictor(object):
	def __init__(self, parsed):
		self.all = {}
		for rule in parsed.agm.rules:
			self.all[rule.name] = 1.0

	def get_distrb(self, init_types, init_binary, init_unary, initModel, targetVariables_types, targetVariables_binary, targetVariables_unary, target): # returns data size time
		return self.all, [1.0], [1000.]
