import pyagrum as gum

gum.fastVariable("x[0:10:5]").isEmpirical()
# False
gum.fastVariable("x+[0:10:5]").isEmpirical()  # '+': bounds defined by data
# True
