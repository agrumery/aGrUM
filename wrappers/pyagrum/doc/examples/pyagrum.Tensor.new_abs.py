import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-1, 0, 2])
p.new_abs().tolist()
# [1.0, 0.0, 2.0]
p.tolist()  # p is unchanged
# [-1.0, 0.0, 2.0]
