import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
sorted(mrf.names())
# ['A', 'B', 'C']
