import pyagrum as gum

x = gum.fastVariable("x[0:10:5]")
gum.Tensor.evIn(x, 3, 7).tolist()
# [0.0, 1.0, 1.0, 1.0, 0.0]
