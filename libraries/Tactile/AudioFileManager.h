/* -*-C-*-
+======================================================================
| Copyright (c) 2025, Craig A. James
|
| This file is part of of the "Tactile" library.
|
| Tactile is free software: you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License (LGPL) as published by
| the Free Software Foundation, either version 3 of the License, or (at
| your option) any later version.
|
| Tactile is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.
|
| You should have received a copy of the LGPL along with Tactile. If not,
| see <https://www.gnu.org/licenses/>.
+======================================================================
*/

/*----------------------------------------------------------------------
 * A very simple "file manager" for the specific configuration of .WAV
 * files expected/supported for the Tactile system. On initialization,
 * the AudioFileManager class reads the contents of the SD file and
 * stores it. Subsequently, files in the root directory and the 
 * subdirectories E1-En are referred to by an integer index.
 *
 * (Note: The subdirectories are named starting with 1 (i.e. E1..EN)
 * for simplicity  with the expected use of this module, but are indexed
 * starting with zero.)
 ----------------------------------------------------------------------*/

#ifndef AudioFileManager_h
#define AudioFileManager_h 1

#include <SPI.h>
#include <SD.h>

using namespace std;

#include "TeensyUtils.h"

// This is also the number of subdirectories for selecting random tracks.
#define NUM_FILES_IN_SUBDIR 100

// Max string length of filename on SD card
#define MAX_FILE_NAME 255

class AudioFileManager {

 public:
  AudioFileManager(TeensyUtils *tc);

  // The main methods
  const char *getFileName(int fileNum);
  const char *getFileName(int dirNum, int fileNum);
  int         getNumFiles(int dirNum);

 private:
  char _fileNames[NUM_CHANNELS][MAX_FILE_NAME];
  char _subDirFileNames[NUM_CHANNELS][NUM_FILES_IN_SUBDIR][MAX_FILE_NAME];
  int  _numSubDirFiles[NUM_CHANNELS];

  int _readDirIntoStringArray(File *dir, int subDirNum);

  TeensyUtils *_tu;
};

#endif
