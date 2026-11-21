import pyagrum as gum

cm = gum.CausalModel(gum.fastBN("Smoking->Cancer"), [("Genotype", ["Smoking", "Cancer"])])
sorted(cm.namesFromIds(cm.children(cm.idFromName("Genotype"))))  # a latent variable has a name
# ['Cancer', 'Smoking']
