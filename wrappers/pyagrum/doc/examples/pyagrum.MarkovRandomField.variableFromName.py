import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
print(mrf.variableFromName("C"))
# C:Range([0,1])
