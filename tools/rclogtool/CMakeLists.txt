cmake_minimum_required(VERSION 2.6)
PROJECT( rclogtool )

INSTALL(PROGRAMS "rclogtool.sh" DESTINATION /opt/robocomp/bin RENAME rclogtool )
INSTALL(PROGRAMS "rcmergelogs.sh" DESTINATION /opt/robocomp/bin RENAME rcmergelogs )
