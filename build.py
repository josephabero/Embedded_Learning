import os
import sys
from enum import Enum, auto

CONSOLE = os.system

class Option(Enum):
	BUILD = auto()
	FLASH = auto()
	BUILD_AND_FLASH = auto

class Progress(Enum):
	INIT = auto()
	BUILDING = auto()
	BUILD_ERROR = auto()
	BUILD_SUCCESS = auto()

args = list(sys.argv)

option = Option.BUILD_AND_FLASH
project = None

if "--project" in args:
	print(args.index("--project"))
	project = args[args.index("--project") + 1]
elif "-p" in args:
	print(args.index("-p"))
	project = args[args.index("-p") + 1]

if "--option" in args:
	print(args.index("--option"))
	option = args[args.index("--option") + 1]
elif "-o" in args:
	print(args.index("-o"))
	option = args[args.index("-o") + 1]


CONSOLE("echo START...")
progress_status = Progress.INIT

if project is not None:
	if option == Option.BUILD or option == Option.BUILD_AND_FLASH:
		CONSOLE("echo BUILDING...")
		progress_status = Progress.BUILDING
		project_name = sys.argv[2]
		CONSOLE(f"echo scons --project={str(project_name)}")
		CONSOLE(f"scons --project={project_name} > error.txt")
		CONSOLE(f"cat error.txt")
		CONSOLE(f"cat error.txt | grep error > error.txt")
	
		print("ERROR.TXT")
		CONSOLE("cat error.txt")
		filepath = os.getcwd() + "/error.txt"
		if os.stat(filepath).st_size > 0:
		    progress_status = Progress.BUILD_ERROR
	
	if (progress_status != Progress.BUILD_ERROR) and (option == Option.FLASH or option == Option.BUILD_AND_FLASH):
		CONSOLE(f"echo flash_board -i {str(project_name)}.bin")
		CONSOLE(f"python nxp-programmer/flash.py -i {project_name}.bin")
	
	progress_status = Progress.BUILD_SUCCESS
	CONSOLE("echo BUILD SUCCESSFUL")

if progress_status == Progress.INIT:
	CONSOLE(f"echo Build FAILED! Failed at {progress_status}")