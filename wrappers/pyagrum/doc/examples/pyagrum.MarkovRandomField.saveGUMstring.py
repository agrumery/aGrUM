import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
s = mrf.saveGUMstring()
mrf2 = gum.MarkovRandomField()
mrf2.loadGUMstring(s)
sorted(mrf2.edges())
# [(0, 1), (1, 2)]
