import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.nodes()
# {0, 1, 2}
