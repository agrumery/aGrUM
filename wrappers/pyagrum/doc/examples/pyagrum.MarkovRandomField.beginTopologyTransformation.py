import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.beginTopologyTransformation()  # postpone the updates of the factors
mrf.eraseFactor(["B", "C"])
f = mrf.addFactor(["A", "C"])
mrf.endTopologyTransformation()
sorted(mrf.edges())
# [(0, 1), (0, 2)]
