import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
s = mrf.saveGUMstring()
mrf2 = gum.MarkovRandomField()
mrf2.loadGUMstring(s)
mrf2.hasSameStructure(mrf)
# True
