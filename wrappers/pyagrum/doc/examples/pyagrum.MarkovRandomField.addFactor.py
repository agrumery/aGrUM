import pyagrum as gum

mrf = gum.MarkovRandomField()
mrf.addVariables(["A", "B", "C"])
# [0, 1, 2]
f = mrf.addFactor(["A", "B"])
f.fillWith([1, 2, 3, 4])
f.names
# ('A', 'B')
sorted(mrf.edges())
# [(0, 1)]
