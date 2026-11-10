import pyagrum as gum

mrf = gum.MarkovRandomField.fastPrototype("A--B[3]--C{yes|no};C--D", 2)
sorted(sorted(f) for f in mrf.factors())
# [[0, 1, 2], [2, 3]]
print(mrf.variable("C"))
# C:Labelized({yes|no})
