import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-3, 0, 2])
p.new_sgn().tolist()
# [-1.0, 0.0, 1.0]
p.tolist()  # p is unchanged
# [-3.0, 0.0, 2.0]
