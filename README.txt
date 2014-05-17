================================================================================
*                     (C) New York University Abu Dhabi, 2014                  *
*                                                                              *
*                     Automatic Test Pattern Generation System                 *
*                                                                              *
================================================================================


NAME
	atpg - automatic test pattern generation system

SYNOPSIS
	atpg  -b <benchmark filename> 	[-d] [-D <debug level>] 
	      [-f <fault list filename>] [-h] [-s <test pattern filename]
	      [-u <undetected faults results filename>] [-Z]
	      [-X <don't cares filling option>]  [--help] [--version] 

DESCRIPTION
	TODO: Add the tool's description here

OPTIONS

	-b
	    Specify the filename to parse the circuit netlist from

	-d
	    Print debugging information with the default debugging level 0

	-D
	    Print debugging information with the supplied debugging level

	-f
	    Specify the filename for the faults list of interest

	-h --help
	    Display the detailed help information

	-s
	    Specify the filename to parse test patterns to simulate fault 
	    collapsing with

	-u
	    Specify the filename to save undetected faults into

	--version
	    Display the tools current version number

	-Z
	    Turn ON/OFF fault collapsing

	-X
	    Option for filling in the don't cares with during fault simulation

