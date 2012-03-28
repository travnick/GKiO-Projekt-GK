# check_cmake_filelist_client.py
#
# Checks the files present in the /src directory and generates apropriate 
# .cmake files overwriting those present in the /ide/cmake directory. Can be 
# run either with argument "AUTO" which is used when the script is run during 
# commiting changeset in Mercurial, or without - in this case the files will 
# be overwritten but nothing will be changed in Mercurial.
#
# Author: Mateusz Duchnowski

import filecmp
import os
import re
import sys
import shutil
import subprocess

# ============================== Configure paths =============================

# Script should be run from inside the repo.
REPO = CWD = os.getcwd()

# Main repo directory - erase anything after (Engine|Other|Tools) in CWD
#REPO = re.sub(r'(/|\\)(Engine|Other|Tools).*', '', CWD)

# Make it outside repository.
TMP_DIR = REPO + '/ide_cmake/'

SRC_DIR = REPO + '/src/'
CMAKE_INC_DIR = REPO + '/cmake/includes/'

TARGETS = (
        'Model',
        'Controller'
)

VSFOLDERS_FILE = 'VSFolders.cmake'

# ================================ Functions ===============================

def printToFile(FILE, TEXT):
    f = open(FILE, 'a')
    f.write(TEXT)

def convertWindowsPathToLinux(windowsPath):
    return re.sub('\\\\', '/', windowsPath)
    
def listFiles(NAME, EXTENSION):
    result = ''
    for root, dirs, files in os.walk(SRC_DIR + NAME):
        list.sort(files)
        for file in files:
            fullPath = os.path.join(root,file)
            fullPath = convertWindowsPathToLinux(fullPath)
            if re.search(r'\.' + EXTENSION + '$', fullPath):
                result += re.sub(r'^.*/src/','  ${SOURCE_DIR}/', fullPath) + '\n'
    return result

def process(NAME):
    FILE = NAME + '.cmake'
    NEW_FILE = TMP_DIR + FILE
    printToFile(NEW_FILE, 'set(HDRS_' + NAME + '\n')
    files = listFiles(NAME, 'h')
    printToFile(NEW_FILE, files + ')\nset(SRCS_' + NAME + '\n')
    files = listFiles(NAME, 'cpp')
    printToFile(NEW_FILE, files + ')\n')

def processAllCmakeFiles():
    for target in TARGETS:
        process(target)

def copyChangedCmakeFiles():
    for target in TARGETS:
        file = target + '.cmake'
        # Copy the file only if it's different to the original version.
        if not filecmp.cmp(TMP_DIR + file, CMAKE_INC_DIR + file):
            shutil.copy(TMP_DIR + file, CMAKE_INC_DIR)

def generateNewCMakeFiles():
    processAllCmakeFiles()
    copyChangedCmakeFiles()
    
def VSFolders(NAME):
    result = '\n# ' + NAME + '.\n'
    print (SRC_DIR + NAME)
    for root, dirs, files in os.walk(SRC_DIR + NAME):
        headers = sources = ''
        list.sort(files)
        for file in files:
            fullPath = os.path.join(root,file)
            fullPath = convertWindowsPathToLinux(fullPath)
            if re.search(r'\.h$', fullPath):
                headers += re.sub(r'^.*/src/', '  ${SOURCE_DIR}/', fullPath) + '\n'
            elif re.search(r'\.cpp$', fullPath):
                sources += re.sub(r'^.*/src/', '  ${SOURCE_DIR}/', fullPath) + '\n'
        
        # Remove everything before (and including) /Engine/src/(Core|...)
        folderName = re.sub(r'^.*/src/' + NAME, '', root)
        
        folderName = re.sub(r'(/|\\)', '\\\\\\\\', folderName)
        if not (headers == ''): 
            result += 'SOURCE_GROUP("Header Files' + folderName + '" FILES\n' + headers + ')\n'
        if not (sources == ''):
            result += 'SOURCE_GROUP("Source Files' + folderName + '" FILES\n' + sources + ')\n'
    return result
    
def updateVSVirtualFolders():
    vsResult = ''
    for target in TARGETS:
        vsResult += VSFolders(target)
    printToFile(TMP_DIR + VSFOLDERS_FILE, vsResult)
    shutil.copy(TMP_DIR + VSFOLDERS_FILE, CMAKE_INC_DIR)

# ================================= Run =================================

# Prevent recursive calls of this script.
LOCK_FILE = REPO + '/cmake_script.lock'
if os.path.isfile(LOCK_FILE):
    sys.exit(0)
lock_file = open(LOCK_FILE, 'w')
lock_file.write('.')
lock_file.close()

# Clean temporary directory.
shutil.rmtree(TMP_DIR, ignore_errors=True)
os.mkdir(TMP_DIR)

try:
    generateNewCMakeFiles()
    updateVSVirtualFolders()
    if len(sys.argv) == 2:
        if sys.argv[1] == 'AUTO':
            reCommit()
finally:
    # A finally clause is always executed before leaving the try statement, 
    # whether an exception has occurred or not.
    os.remove(LOCK_FILE)
    shutil.rmtree(TMP_DIR, ignore_errors=True)
