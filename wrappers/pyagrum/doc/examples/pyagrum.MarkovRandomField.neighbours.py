import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.neighbours("B")
# {0, 2}
