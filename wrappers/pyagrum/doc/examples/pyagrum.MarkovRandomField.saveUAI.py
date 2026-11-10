import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.saveUAI("model.uai")
print(open("model.uai").readline().strip())
# MARKOV
gum.loadMRF("model.uai").sizeEdges()
# 2
