import pyagrum as gum

a = gum.LabelizedVariable("a", "a", ["x", "y", "z"])
gum.Tensor.deterministicTensor(a, "y").tolist()
# [0.0, 1.0, 0.0]
gum.Tensor.deterministicTensor(a, 2).tolist()
# [0.0, 0.0, 1.0]
