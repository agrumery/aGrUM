import pyagrum as gum

x = gum.fastVariable("x[0:10:5]")
gum.Tensor.evEq(x, 5).tolist()
# [0.0, 0.0, 1.0, 0.0, 0.0]
