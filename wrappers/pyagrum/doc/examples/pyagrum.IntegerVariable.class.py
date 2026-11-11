import pyagrum as gum

# creating a variable with 3 values: 1, 34, 142
va = gum.IntegerVariable("a", "an integer variable", [1, 34, 142])
print(va)
# a:Integer({1|34|142})
print(va.addValue(25))
# a:Integer({1|25|34|142})
va.changeValue(34, 43)
print(va)
# a:Integer({1|25|43|142})
vb = gum.IntegerVariable("b", "b").addValue(34).addValue(142).addValue(1)
print(vb)
# b:Integer({1|34|142})
vb.labels()
# ('1', '34', '142')
