@echo off
set n=1
set batch_num=1
set do_prepare=0

if %do_prepare%==1 (
    echo preparing........
    train_net.bat prepare
    python ..\\python\\learner.py
) else (
    echo skip prepare!
)
echo start train......

:LOOP
if %%n LEQ 2000 (
    echo --------------%%n-th train------------------
    set i=0
    :INNER_LOOP
    if %%i LSS %batch_num% (
		echo generate data......
        start /B cmd /C "train_net.bat generate %%i"
        set /A i+=1
        goto INNER_LOOP
    )
    python ..\\python\\learner.py train
    :: train_net.bat eval_with_winner 10
    train_net.bat eval_with_random 10
    set /A n+=1
    goto LOOP
)
