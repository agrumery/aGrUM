import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.nodeset(["A", "C"])
# {0, 2}
