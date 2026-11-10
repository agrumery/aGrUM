import pyagrum as gum

mrf = gum.fastMRF("A--B;C--D;E")
sorted(mrf.connectedComponentsList().items())
# [(0, {0, 1}), (1, {2, 3}), (2, {4})]
