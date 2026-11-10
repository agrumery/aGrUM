import pyagrum as gum

bn = gum.fastBN("Smoking->Tar->Cancer;Smoking->Cancer")
cm = gum.CausalModel(bn)
cm.addLatentVariable("Genotype", ["Smoking", "Cancer"])
sorted(cm.latentVariablesNames())
# ['Genotype']
cm.existsArc("Smoking", "Cancer")  # removed: explained by the latent confounder
# False
