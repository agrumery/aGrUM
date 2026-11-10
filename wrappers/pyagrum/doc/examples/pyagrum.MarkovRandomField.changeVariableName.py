import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.changeVariableName("A", "Alpha")
sorted(mrf.names())
# ['Alpha', 'B', 'C']
