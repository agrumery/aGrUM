import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")
sorted(mrf.edges())
# [(0, 1), (0, 2), (1, 2), (2, 3)]
