#rclogtool

Shell script to log a command output while keep showing it at the screen.
The log file will include a timestamp and (optionally) a label and will be stored at $ROBOCOMP_LOGS or local folder if not defined.

Generated log files can be merged afterwards using 'rcmergelogs' tool to get a single log file:
 rcmergelogs "mergedLog" "logFile1" [ "logFile2" ... ]
to create a file "mergedLog" with the result of unify every given log file.

##Use
The first argument defines a label to be included after the timestamp. An empty field will indicate to do not use any label.
An optional argument can be given to specify the destination filename, that defaults to "label.log".
The destination file path will be relative to $ROBOCOMP_LOGS or local folder if not defined.
That means that typically, if used only with a single argument, the generated log file will be "label.log" at $ROBOCOMP_LOGS folder.

 rclogtool "label" [ "filename" ]

###Example: To log a command output (stdout and stderr) using "commandLabel" as label and "commandLabel.log" as filename:

 command 2>&1 | rclogtool "commandLabel"

###Example: To log a command output (only stdout) using "LogFile" as filename, without any label:

 command | rclogtool "" "LogFile"

###Example: To log a command output (stdout and stderr) using "commandLabel" as label and "LogFile" as filename:

 command 2>&1 | rclogtool "commandLabel" "LogFile"

###Example: To merge "*.log" files into "folder" and store the unified log file into "full.log":

 rcmergelogs "full.log" "folder/*.log"
