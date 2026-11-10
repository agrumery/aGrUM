import pyagrum as gum

bn = gum.fastBN("A[3]->B{yes|no}")
i = gum.Instantiation(bn.cpt("B"))
i.fromdict({"A": 2, "B": "no"})
i.todict()
# {'B': 1, 'A': 2}
i.todict(withLabels=True)
# {'B': 'no', 'A': '2'}
