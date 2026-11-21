import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")
sorted(mrf.namesFromIds(mrf.neighbours("C")))
# ['A', 'B', 'D']
