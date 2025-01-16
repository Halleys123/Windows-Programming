# ! --- Old ---
# Variables
# include_folder = ./include

# Rules
# .PHONY: all test clean

# Commands
# build:
# 	g++ main.cpp -o ./bin/program.exe -I $(include_folder)

# ! --- new ---

# My Folders
include_folder = ./include/**
lib_folder = ./lib/**

# Windows Kit Include Folders
kit_include_1 = "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/winrt"
kit_include_2 = "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/ucrt"
kit_include_3 = "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um"
kit_include_4 = "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/shared"
kit_include_5 = "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/cppwinrt"

# Windows Kit Lib Folders
kit_lib_1 = "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64"
kit_lib_2 = "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/ucrt/x64"

# MSVC Include Folders
msvc_include = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/include"
msvc_atlmfc_include = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/atlmfc/include"

# MSVC Lib Folders
msvc_lib = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/lib/x64"
msvc_atlmfc_lib = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/atlmfc/lib/x64"

# Commands
build:
	cl main.cpp -F10485760 /Fo:./bin/program.obj /Fe:program.exe /I$(msvc_include) /I$(msvc_atlmfc_include) /I$(kit_include_1) /I$(kit_include_2) /I$(kit_include_3) /I$(kit_include_4) /I$(kit_include_5) /link /LIBPATH:$(msvc_lib) /LIBPATH:$(msvc_atlmfc_lib) /LIBPATH:$(kit_lib_1) /LIBPATH:$(kit_lib_2) User32.lib Gdi32.lib /OUT:./bin/program.exe /ENTRY:WinMainCRTStartup /SUBSYSTEM:WINDOWS
