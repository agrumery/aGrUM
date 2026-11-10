import pyagrum as gum

x = gum.fastVariable("x[0:10:5]")
gum.Tensor.evLt(x, 5).tolist()
# [1.0, 1.0, 0.0, 0.0, 0.0]
