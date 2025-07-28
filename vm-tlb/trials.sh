#!/bin/bash

if [[ -z $1 ]]; then
    num_trials=10000000
else
    num_trials=$1
fi

for ((i = 1; i <= 2**14; i *= 2)); do
    avg_time=$(./tlb $i $num_trials)
    echo $i $avg_time
done
