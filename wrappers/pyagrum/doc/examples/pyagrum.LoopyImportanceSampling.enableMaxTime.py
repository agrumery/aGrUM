import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.disableMaxTime()
ie.enableMaxTime()
ie.isEnabledMaxTime()
# True
