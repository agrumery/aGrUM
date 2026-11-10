import pyagrum as gum

with open("devices.o3prm", "w") as f:
  f.write("""
interface Equipment { boolean works; }
interface Printer extends Equipment { boolean hasInk; }
""")
ex = gum.PRMexplorer()
ex.load("./devices.o3prm")
ex.getSuperInterface("Printer")
# 'Equipment'
print(ex.getSuperInterface("Equipment"))
# None
