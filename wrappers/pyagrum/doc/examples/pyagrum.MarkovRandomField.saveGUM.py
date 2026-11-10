import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.saveGUM("model.jgum")
gum.loadMRF("model.jgum").hasSameStructure(mrf)
# True
