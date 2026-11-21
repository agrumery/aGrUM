import pyagrum as gum

cm = gum.CausalModel(gum.fastBN("Smoking->Cancer"), [("Genotype", ["Smoking", "Cancer"])])
cm.idsFromNames(["Genotype", "Cancer"])
# [2, 1]
