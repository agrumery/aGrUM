echo "-----------"
python bn2csv.py ../resources/alarm.dsl 1000 alarm.csv

echo
echo "-----------"
python bn2png.py ../resources/alarm.dsl


echo
echo "-----------"
python BN_roc.py ../resources/alarm.dsl alarm.csv KINKEDTUBE TRUE

echo
echo "-----------"
python BN_scoring.py ../resources/alarm.dsl alarm.csv

echo
echo "-----------"
python compareBN.py ../resources/alarm.dsl ../resources/bn.bif
python compareBN.py ../resources/alarm.dsl ../resources/alarm.dsl

echo
echo "-----------"
python InformationTheory.py ../resources/alarm.dsl 

echo
echo "-----------"
python KL4bns.py ../resources/bnP.bif ../resources/bnQ.bif
python KL4bns.py ../resources/bnQ.bif ../resources/bnP.bif

echo
echo "-----------"
python pretty_cpt.py ../resources/alarm.dsl 

echo
echo "-----------"
python topology.py ../resources/alarm.dsl