import pyagrum as gum

gum.initRandom(42)
cn = gum.CredalNet(gum.fastBN("A->B"))
cn.bnToCredal(0.8, False)
cn.saveBNsMinMax("min.bif", "max.bif")
bnmin, bnmax = gum.loadBN("min.bif"), gum.loadBN("max.bif")
all(lo <= up for lo, up in zip(bnmin.cpt("A").tolist(), bnmax.cpt("A").tolist()))
# True
