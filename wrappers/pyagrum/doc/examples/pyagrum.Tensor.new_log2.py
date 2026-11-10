import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([1, 2, 8])
p.new_log2().tolist()
# [0.0, 1.0, 3.0]
p.tolist()  # p is unchanged
# [1.0, 2.0, 8.0]
