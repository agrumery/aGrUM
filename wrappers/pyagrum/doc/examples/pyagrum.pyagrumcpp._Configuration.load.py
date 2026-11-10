import pyagrum as gum

with open("pyagrum.ini", "w") as f:
  f.write("[tensor]\nvisibleDigits = 6\n")
gum.config.push()
gum.config.load()  # pyagrum.ini of the current directory
gum.config["tensor", "visibleDigits"]
# '6'
gum.config.pop()
