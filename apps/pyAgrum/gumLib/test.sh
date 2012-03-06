python bn2csv.py ../resources/alarm.dsl 1000 alarm.csv
python bn2png.py ../resources/alarm.dsl
python BN_scoring.py ../resources/alarm.dsl alarm.csv 
python BN_roc.py ../resources/alarm.dsl alarm.csv KINKEDTUBE TRUE
