#!/bin/bash

ans=0
checked=1
for comp1 in ./$1/*
do
    let "ans++"
    for comp2 in ./$2/*
    do
        if [[ $checked -eq 1 ]]; then
            let "ans++"
        fi
        if cmp -s "$comp1" "$comp2"; then
            printf 'The file "%s" is the same as "%s"\n' "$comp1" "$comp2"
        else
            printf 'The file "%s" is different from "%s"\n' "$comp1" "$comp2"
        fi
    done
    checked=0;
done
printf 'Number of files viewed : %d \n' "$ans"
