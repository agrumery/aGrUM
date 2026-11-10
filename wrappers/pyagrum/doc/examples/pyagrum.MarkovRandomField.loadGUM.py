import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.saveGUM("model.jgum")
mrf2 = gum.MarkovRandomField()
mrf2.loadGUM("model.jgum")
mrf2.hasSameStructure(mrf)
# True
