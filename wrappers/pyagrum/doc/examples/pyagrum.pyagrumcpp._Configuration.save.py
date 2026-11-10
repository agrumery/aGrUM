import pyagrum as gum

gum.config.push()
gum.config.reset()
gum.config["tensor", "visibleDigits"] = 6
gum.config.save()  # the differences with the defaults, in pyagrum.ini
print(open("pyagrum.ini").read())
# [tensor]
#   visibleDigits = 6
gum.config.pop()
