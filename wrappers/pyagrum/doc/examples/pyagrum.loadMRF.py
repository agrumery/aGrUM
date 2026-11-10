import pyagrum as gum

gum.saveMRF(gum.fastMRF("A--B;B--C"), "model.uai")
mrf = gum.loadMRF("model.uai")
mrf.size()
# 3
len(mrf.factors())
# 2
