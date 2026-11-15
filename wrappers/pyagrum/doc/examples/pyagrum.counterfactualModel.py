import pyagrum as gum

bn = gum.fastBN("X->Y->Z;W->Y")
cm = gum.CausalModel(bn)
twin = gum.counterfactualModel(cm, whatif="Y")
# copies of Y and of its descendant Z; X and W are shared by both worlds
sorted(twin.observationalBN().names())
# ['W', 'X', 'Y', 'Y*', 'Z', 'Z*']
