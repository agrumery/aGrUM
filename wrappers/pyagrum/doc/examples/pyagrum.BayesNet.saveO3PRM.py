import pyagrum as gum
import pyagrum.prm  # adds the O3PRM methods to BayesNet

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.25, 0.75])
bn.saveO3PRM("model.o3prm")
print(open("model.o3prm").read().splitlines()[1])
#   int (0, 1) A  {[0.25, 0.75]};
