#!/bin/bash
#
# Shell script to merge timestamped log files.
# Merge log files into a single one.
#
# Author: Ale Cruces
# Updated 2020-03
#


# --- Begin script ---
if [ $# -lt 2 ]; then
	printf "Wrong number of arguments.\n"
	printf 'Usage: rcmergelogs "mergedLog" "logFile1" [ "logFile2" ... ]\n'
	printf 'Examples:\n'
	printf '\trcmergelogs full.log *.log\n'
	printf '\trcmergelogs session.log session/*.log\n'
	exit 1
fi

mergedLog="$1"
shift
logFiles="$@"

cat $logFiles | sort -n > "$mergedLog"

exit 0
