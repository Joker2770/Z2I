@echo off
set n=1
set batch_num=1
set do_prepare=0
set i=0

if %do_prepare%==1 (
    echo preparing........
    train_net.bat prepare
    python ..\\python\\learner.py
) else (
    echo skip prepare!
)
echo start train......

echo --------------%n%-th train------------------
:INNER_LOOP
if %i% LSS %batch_num% (
    echo generate data......
    start /B /WAIT train_net.bat generate %i%
    set /A i+=1
    goto INNER_LOOP
)
if %n% LEQ 10 (
    echo --------------%n%-th train------------------
    set /A i=0
    python ..\\python\\learner.py train
    :: train_net.bat eval_with_winner 10
    train_net.bat eval_with_random 10
    set /A n+=1
    goto INNER_LOOP
)
