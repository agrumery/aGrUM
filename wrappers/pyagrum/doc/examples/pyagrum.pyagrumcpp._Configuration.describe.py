import pyagrum as gum

gum.config.push()
gum.config.reset()
gum.config["tensor", "visibleDigits"] = 6
print("\n".join(gum.config.describe().splitlines()[:2]))  # the customized properties first
# [tensor]
#   visibleDigits = 6
gum.config.pop()
