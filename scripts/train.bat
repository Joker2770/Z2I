@echo off
set n=1
set batch_num=1
set do_prepare=0
set i=0

if %do_prepare%==1 (
    echo preparing........
    call train_net.bat prepare
    call python ..\\python\\learner.py
) else (
    echo skip prepare!
)
echo start train......

echo --------------%n%-th train------------------
:INNER_LOOP
if %i% LSS %batch_num% (
    echo generate data......
    call train_net.bat generate %i%
    set /A i+=1
    goto INNER_LOOP
)
if %n% LEQ 2000 (
    echo --------------%n%-th train------------------
    set /A i=0
    call python ..\\python\\learner.py train
    :: call train_net.bat eval_with_winner 10
    call train_net.bat eval_with_random 10
    set /A n+=1
    goto INNER_LOOP
)
