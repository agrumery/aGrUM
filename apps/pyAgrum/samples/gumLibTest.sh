gumlib_path="../../../wrappers/pyAgrum/gumLib"

echo "-----------"
python $gumlib_path/bn2csv.py ../resources/alarm.dsl 1000 alarm.csv

echo
echo "-----------"
python $gumlib_path/bn2graph.py ../resources/alarm.dsl

echo
echo "-----------"
python $gumlib_path/bn2roc.py ../resources/alarm.dsl alarm.csv KINKEDTUBE TRUE

echo
echo "-----------"
python $gumlib_path/bn2scores.py ../resources/alarm.dsl alarm.csv

echo
echo "-----------"
python $gumlib_path/bn_vs_bn.py ../resources/alarm.dsl ../resources/bn.bif
python $gumlib_path/bn_vs_bn.py ../resources/alarm.dsl ../resources/alarm.dsl
# 
# echo
# echo "-----------"
# python $gumlib_path/InformationTheory.py ../resources/alarm.dsl 

# echo
# echo "-----------"
# python KL4bns.py ../resources/bnP.bif ../resources/bnQ.bif
# python KL4bns.py ../resources/bnQ.bif ../resources/bnP.bif
# 
# echo
# echo "-----------"
# python pretty_cpt.py ../resources/alarm.dsl 
# 
# echo
# echo "-----------"
# python topology.py ../resources/alarm.dsl