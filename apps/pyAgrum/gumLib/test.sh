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