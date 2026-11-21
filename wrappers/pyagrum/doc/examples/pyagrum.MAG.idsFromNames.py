import pyagrum as gum

dag = gum.fastDAG("A->B->C;L->C;L->D")
mag = gum.MAG.fromDAG(dag, {dag.idFromName("L")})  # L is latent
mag.idsFromNames(["A", "D"])
# [0, 4]
