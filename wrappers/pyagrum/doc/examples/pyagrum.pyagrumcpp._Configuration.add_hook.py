import pyagrum as gum

changes = []
gum.config.push()
gum.config.add_hook(lambda: changes.append(gum.config["tensor", "visibleDigits"]))
gum.config["tensor", "visibleDigits"] = 6
changes
# ['6']
gum.config.pop()
