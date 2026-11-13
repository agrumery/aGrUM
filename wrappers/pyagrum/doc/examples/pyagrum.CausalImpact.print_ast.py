import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X")
# the tree of the identified formula: the sum on Z of P(Y|Z,X)*P(Z)
ci.print_ast()
# ┌────────────┐
# │ P(Y|do(X)) │
# └────────────┘
#       └─ sum
#          ├─ var: Z
#          └─ *
#             ├─ P
#             │  ├─ vars: ['Y']
#             │  └─ knowing: ['Z', 'X']
#             └─ P
#                └─ vars: ['Z']
