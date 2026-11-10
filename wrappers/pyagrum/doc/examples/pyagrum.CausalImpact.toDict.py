import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X")
d = ci.toDict()
d["op"]
# 'sum'
d
# {'op': 'sum', 'var': 'Z', 'term': {'op': '*', 'op1': {'op': 'P', 'vars': ['Y'], 'knowing': ['Z', 'X']}, 'op2': {'op': 'P', 'vars': ['Z']}}}
