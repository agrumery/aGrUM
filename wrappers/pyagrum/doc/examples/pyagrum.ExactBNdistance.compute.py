import pyagrum as gum

p = gum.fastBN("A->B")
p.cpt("A").fillWith([0.2, 0.8])
p.cpt("B").fillWith([0.5, 0.5, 0.9, 0.1])
q = gum.BayesNet(p)
q.cpt("A").fillWith([0.5, 0.5])
res = gum.ExactBNdistance(p, q).compute()
sorted(res)
# ['bhattacharya', 'errorPQ', 'errorQP', 'hellinger', 'jensen-shannon', 'klPQ', 'klQP']
round(res["klPQ"], 4)
# 0.2781
round(res["hellinger"], 4)
# 0.3204
