import os
import sys
import argparse
from enum import Enum, auto
import subprocess
CONSOLE = os.system

class Progress(Enum):
	INIT = auto()
	BUILDING = auto()
	BUILD_ERROR = auto()
	BUILD_SUCCESS = auto()
	FLASH_SUCCESS = auto()

parser = argparse.ArgumentParser();

parser.add_argument(
	'--project',
	action='store',
	dest='project',
	required=True
)

parser.add_argument('-b',
 	action='store_true',
 	default=False,
 	dest='build',
 	required=False
)

parser.add_argument('-r',
 	action='store_true',
 	default=False,
 	dest='run',
 	required=False
)

parser.add_argument('-br',
 	action='store_true',
 	default=True,
 	dest='build_and_run',
 	required=False
)

args = parser.parse_args()

if args.run or args.build:
	args.build_and_run = False

if args.run:		print(f"Going to flash board: {args.project}.bin...")
elif args.build:    print(f"Going to build: {args.project}...")
else:				print("Build and Flash...")


print("START...")
progress_status = Progress.INIT

# BUILD the Project
if args.build or args.build_and_run:
	print("BUILDING...")
	progress_status = Progress.BUILDING
	print(f"echo scons --project={args.project}")
	CONSOLE(f"scons --project={args.project} > error.txt")
	# Check if there was a build error
	CONSOLE(f"cat error.txt")
	CONSOLE(f"grep -i 'error' error.txt > out.txt")
	filepath = os.getcwd() + "/out.txt"
	# print(os.stat(filepath).st_size)
	if os.stat(filepath).st_size > 0:
	    progress_status = Progress.BUILD_ERROR
	else:
		CONSOLE("rm error.txt")
		CONSOLE("rm out.txt")
		progress_status = Progress.BUILD_SUCCESS
# FLASH the SJTwo
if args.run or args.build_and_run and progress_status != Progress.BUILD_ERROR:
	print(f"echo flash_board -i {args.project}.bin")
	CONSOLE(f"python nxp-programmer/flash.py -i {args.project}.bin")

	# Check if there was a flash error
	progress_status = Progress.FLASH_SUCCESS
	
# CONSOLE("echo BUILD SUCCESSFUL")

if progress_status == Progress.INIT:
	CONSOLE(f"echo Build FAILED! Failed at {progress_status}")