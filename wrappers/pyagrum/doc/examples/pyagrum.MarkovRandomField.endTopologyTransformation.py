import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.beginTopologyTransformation()
mrf.erase("C")
mrf.addVariables(["D"])
# [2]
f = mrf.addFactor(["A", "D"])
mrf.endTopologyTransformation()  # the factors are updated now
sorted(mrf.edges())
# [(0, 1), (0, 2)]
