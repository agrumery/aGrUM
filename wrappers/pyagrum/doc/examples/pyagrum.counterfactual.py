import pyagrum as gum

# U is the exogenous cause of Y (its noise), Y = X xor U
bn = gum.fastBN("X->Y<-U")
bn.cpt("X").fillWith([0.5, 0.5])
bn.cpt("U").fillWith([0.3, 0.7])
for x in (0, 1):
  for u in (0, 1):
    bn.cpt("Y")[{"X": x, "U": u}] = [1, 0] if x == u else [0, 1]
cm = gum.CausalModel(bn)
# X=1 and Y=1 were observed (hence U=0): what would Y have been if X had been 0 ?
gum.counterfactual(cm, on="Y", whatif="X", profile={"X": "1", "Y": "1"}, values={"X": "0"}).tolist()
# [1.0, 0.0]

# without the profile, the effect of the intervention only: P(Y | do(X=0))
gum.counterfactual(cm, on="Y", whatif="X", profile={}, values={"X": "0"}).tolist()
# [0.3, 0.7]
