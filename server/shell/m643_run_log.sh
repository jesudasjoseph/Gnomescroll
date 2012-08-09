#!/bin/bash

if [ -z "$2" ]
then
    ./gnomescroll_server ./settings/production.lua 2>&1 | tee $1
else
    ./gnomescroll_server ./settings/production.lua --map="$2" 2>&1 | tee $1
fi
