import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
s = mrf.toDotAsFactorGraph()
s.startswith("graph")
# True
