import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
sorted(sorted(f) for f in mrf.factors())
# [[0, 1], [1, 2]]
