import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 2)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4])
for i in p.loopIn():  # i is an Instantiation over the variables of p
  p.set(i, p.get(i) * 10 if i["b"] == 1 else 0)
p.tolist()
# [[0.0, 0.0], [30.0, 40.0]]
