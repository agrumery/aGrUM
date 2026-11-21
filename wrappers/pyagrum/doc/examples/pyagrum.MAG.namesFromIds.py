import pyagrum as gum

dag = gum.fastDAG("A->B->C;L->C;L->D")
mag = gum.MAG.fromDAG(dag, {dag.idFromName("L")})  # L is latent
sorted(mag.namesFromIds(mag.spouses(mag.idFromName("C"))))  # C<->D
# ['D']
