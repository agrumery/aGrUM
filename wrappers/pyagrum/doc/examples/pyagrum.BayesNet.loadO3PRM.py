import pyagrum as gum
import pyagrum.prm  # adds the O3PRM methods to BayesNet

bn = gum.fastBN("A->B->C")
bn.saveO3PRM("model.o3prm")
bn2 = gum.BayesNet()
bn2.loadO3PRM("./model.o3prm")  # returns the warnings of the parser
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
