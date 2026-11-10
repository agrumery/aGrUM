import pyagrum as gum

bn = gum.fastBN("A->B[3]->C")
bn.saveUAI("model.uai")
bn2 = gum.BayesNet()
bn2.loadUAI("model.uai")
bn2.size(), sorted(bn2.arcs())
# (3, [(0, 1), (1, 2)])
sorted(bn2.names())  # the UAI format does not store the names of the variables
# ['0', '1', '2']
