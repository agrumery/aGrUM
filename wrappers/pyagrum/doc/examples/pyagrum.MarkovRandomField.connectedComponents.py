import pyagrum as gum

mrf = gum.fastMRF("A--B;C--D;E")
sorted(mrf.connectedComponents().items())  # node id: component id
# [(0, 0), (1, 0), (2, 1), (3, 1), (4, 2)]
