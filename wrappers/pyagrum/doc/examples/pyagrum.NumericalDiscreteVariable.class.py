import pyagrum as gum

# creating a variable with 3 values: 1.5, 3.14, 1.42
va = gum.NumericalDiscreteVariable("a", "a numerical variable", [1.5, 3.14, 1.42])
print(va)
# a:NumericalDiscrete({1.42|1.5|3.14})
print(va.addValue(2.01))
# a:NumericalDiscrete({1.42|1.5|2.01|3.14})
va.changeValue(3.14, 3.1415)
print(va)
# a:NumericalDiscrete({1.42|1.5|2.01|3.1415})
vb = gum.NumericalDiscreteVariable("b", "b").addValue(3.14).addValue(1.42).addValue(1.5)
print(vb)
# b:NumericalDiscrete({1.42|1.5|3.14})
vb.labels()
# ('1.42', '1.5', '3.14')
vc = gum.NumericalDiscreteVariable("c", "c", 1.2, 3.8, 5)  # 5 values from 1.2 to 3.8
print(vc)
# c:NumericalDiscrete({1.2|1.85|2.5|3.15|3.8})
