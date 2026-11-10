import pyagrum as gum

bn = gum.fastBN("A->B[3]->C")
bn.saveUAI("model.uai")
open("model.uai").readline().strip()  # the type of the model
# 'BAYES'
bn2 = gum.loadBN("model.uai")
bn2.dim() == bn.dim()
# True
