import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
cm = gum.CausalModel(bn)
cf = gum.Counterfactual(cm, {"Y"}, {"X"}, {"X": "yes", "Y": "yes"}, {"X": "no"})
# the real world and the copies X*, Y* of the counterfactual world, which share U
twin = cf.twinModel().observationalBN()
sorted(twin.names())
# ['U', 'X', 'X*', 'Y', 'Y*']
sorted(twin.variable(p).name() for p in twin.parents("Y*"))
# ['U', 'X*']
