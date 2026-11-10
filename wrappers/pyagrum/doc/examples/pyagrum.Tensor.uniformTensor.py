import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 4)
gum.Tensor.uniformTensor(a).tolist()
# [0.25, 0.25, 0.25, 0.25]
