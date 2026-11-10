import pyagrum as gum

bn = gum.fastBN("Smoking->Tar->Cancer;Smoking->Cancer")
cm = gum.CausalModel(bn, [("Genotype", ["Smoking", "Cancer"])])
cm.backDoor("Smoking", "Cancer") is None  # the latent confounder cannot be adjusted for
# True
{cm.nameFromId(i) for i in cm.frontDoor("Smoking", "Cancer")}
# {'Tar'}
