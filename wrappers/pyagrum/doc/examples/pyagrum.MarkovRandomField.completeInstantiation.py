import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
print(mrf.completeInstantiation())
# <A:0|B:0|C:0>
