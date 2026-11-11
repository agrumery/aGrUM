import pyagrum as gum

# creating a variable with 3 labels: '0', '1' and '2'
va = gum.LabelizedVariable("a", "a labelized variable", 3)
print(va)
# a:Labelized({0|1|2})
print(va.addLabel("foo"))
# a:Labelized({0|1|2|foo})
va.changeLabel(1, "bar")
print(va)
# a:Labelized({0|bar|2|foo})
vb = gum.LabelizedVariable("b", "b", 0).addLabel("A").addLabel("B").addLabel("C")
print(vb)
# b:Labelized({A|B|C})
vb.labels()
# ('A', 'B', 'C')
vb.isLabel("E")
# False
vb.label(2)
# 'C'
vc = gum.LabelizedVariable("c", "c", ["one", "two", "three"])
print(vc)
# c:Labelized({one|two|three})
