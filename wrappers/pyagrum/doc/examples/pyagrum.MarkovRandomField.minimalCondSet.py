import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
mrf.minimalCondSet("A", ["B", "C", "D"])
# {1}
