import pyagrum as gum

gum.config.push()  # to restore the configuration at the end
gum.config["tensor", "visibleDigits"] = 6
gum.config["tensor", "visibleDigits"]
# '6'
gum.config.pop()
