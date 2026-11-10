import pyagrum as gum

mrf = gum.MarkovRandomField()
mrf.add(gum.LabelizedVariable("A", "", ["no", "yes"]))
# 0
mrf.add("B[3]")
# 1
sorted(mrf.names())
# ['A', 'B']
