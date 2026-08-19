#!/bin/bash
n=1
batch_num=1
game_num=10
do_prepare=1
if [ $do_prepare == 1 ]
then
	echo "preparing........"
	exec ./train_net.sh prepare
	python3 ../python/learner.py
fi

while [  $n -le 2000 ]
do
	echo "--------------$n-th train------------------"
	for ((i=0;i<$batch_num;i++));do
		{
		# sleep 3;echo 1>>aa && echo "done!"
		let p=i*$game_num
		exec ./train_net.sh generate $p
		}
	done
	wait
	python3 ../python/learner.py train
#	exec ./train_net.sh eval_with_winner 10
	exec ./train_net.sh eval_with_random 10
	let n++
done
