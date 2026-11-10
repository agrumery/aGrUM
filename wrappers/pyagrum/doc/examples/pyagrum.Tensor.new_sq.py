import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-3, 0, 2])
p.new_sq().tolist()
# [9.0, 0.0, 4.0]
p.tolist()  # p is unchanged
# [-3.0, 0.0, 2.0]
