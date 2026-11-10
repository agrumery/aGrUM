import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
print(mrf.variable("B"))
# B:Range([0,1])
mrf.variable(0).name()
# 'A'
