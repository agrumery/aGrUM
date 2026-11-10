import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.toFast()
# 'A[2]--B[2];B--C[2]'
