import pyagrum as gum

mrf = gum.fastMRF("A[10]--B[10];B--C[10]")
mrf.log10DomainSize()
# 3.0
