import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")  # a factor over A,B,C and another one over C,D
mrf.factors()
# [{0, 1, 2}, {2, 3}]
