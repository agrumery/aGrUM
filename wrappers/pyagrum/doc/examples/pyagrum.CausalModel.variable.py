import pyagrum as gum

bn = gum.fastBN("A{yes|no}->B[3]")
cm = gum.CausalModel(bn)
print(cm.variable("A"))
# A:Labelized({yes|no})
cm.variable(cm.idFromName("B")).labels()
# ('0', '1', '2')
