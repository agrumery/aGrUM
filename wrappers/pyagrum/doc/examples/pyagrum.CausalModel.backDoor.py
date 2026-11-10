import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
{cm.nameFromId(i) for i in cm.backDoor("X", "Y")}
# {'Z'}
