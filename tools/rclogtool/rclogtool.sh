#!/bin/bash
#
# Shell script to log a command output while keep showing it at the screen.
# The log file will be stored at ${ROBOCOMP_LOGS} folder, including a timestamp and (optionally) a label.
# 
# Generated log files can be merged using 'rcmergelogs' tool to get a single log file.
#
# Author: Ale Cruces
# Updated 2020-03
#


# --- Begin script ---
if [ $# -ne 1 ] && [ $# -ne 2 ]; then
	printf "Wrong number of arguments.\n"
	printf 'Usage: rclogtool "label" [ "filename" ]\n'
	exit 1
fi

# Disable interruption signals SIGINT and SIGHUP if stdin is not a terminal
if ! [ -t 0 ]; then
	trap "" SIGINT
	trap "" SIGHUP
fi


label="$1"
if [ $# -eq 2 ]; then
	relfilename="$2"
else
	if [ "$label" ]; then
		relfilename="${1}.log"
	else
		printf "If no \"label\" is used, a \"filename\" must be specified.\n"
		exit 2
	fi
fi

if [ "${ROBOCOMP_LOGS}" ]; then
	filename="${ROBOCOMP_LOGS}/${relfilename}"
else
	filename="${relfilename}"
fi


# Check if 'ts' command is found (provided by 'moreutils' package)
if [ "$(command -v ts)" ]; then
	# 'ts' command exists
	
	if [ "$label" ]; then
		tee >(ts "%F %.T   $label:" > "$filename")
	else
		tee >(ts "%F %.T  " > "$filename")
	fi
else
	# 'ts' command not found, using builtin bash's printf date-time functionality (available since Bash 4.2)
	printf "Warning: 'ts' command not found. Install 'moreutils' package to get subsecond resolution\n\n"
	
	if [ "$label" ]; then
		tee >(
			while read -r; do
				printf '%(%F %T)T   %s: %s\n' -1 "$label" "$REPLY"
			done > "$filename")
	else
		tee >(
			while read -r; do
				printf '%(%F %T)T   %s\n' -1 "$REPLY"
			done > "$filename")
	fi
fi

exit 0
