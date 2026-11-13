import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.toFast()
# 'A[2]--B[2];B--C[2]'

# a name is quoted when it would not be read back as is otherwise
gum.fastMRF("'a--b'--C").toFast()
# "'a--b'[2]--C[2]"
