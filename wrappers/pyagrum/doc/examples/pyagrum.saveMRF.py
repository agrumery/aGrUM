import pyagrum as gum

mrf = gum.fastMRF("A--B--C")
gum.saveMRF(mrf, "model.uai")
gum.loadMRF("model.uai").factors()
# [{0, 1, 2}]
