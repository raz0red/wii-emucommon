#!/bin/bash

#---------------------------------------------------------------------------#
#                                                                           #
#  wii-emucommon:                                                           #
#  Wii emulator common code                                                 #
#                                                                           #
#  [github.com/raz0red/wii-emucommon]                                       #
#                                                                           #
#---------------------------------------------------------------------------#
#                                                                           #
#  Copyright (C) 2019 raz0red                                               #
#                                                                           #
#  This program is free software; you can redistribute it and/or            #
#  modify it under the terms of the GNU General Public License              #
#  as published by the Free Software Foundation; either version 2           # 
#  of the License, or (at your option) any later version.                   #
#                                                                           #
#  This program is distributed in the hope that it will be useful,          #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#  GNU General Public License for more details.                             #
#                                                                           #
#  You should have received a copy of the GNU General Public License        #
#  along with this program; if not, write to the Free Software              #
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA            #
#  02110-1301, USA.                                                         #
#---------------------------------------------------------------------------#

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
DATE="$( date '+%Y%m%d%H%S' )"
DIST_DIR=$SCRIPTPATH/dist
SDL_DIR=$SCRIPTPATH/thirdparty/sdl

#
# Function that is invoked when the script fails.
#
# $1 - The message to display prior to exiting.
#
function fail() {
    echo $1
    echo "Exiting."
    exit 1
}

# Build SDL
echo "Building SDL..."
cd $SDL_DIR || { fail 'Error changing to SDL directory.'; }
make || { fail 'Error building SDL.'; }

# Change to script directory
echo "Changing to script directory..."
cd $SCRIPTPATH || { fail 'Error changing to script directory.'; }

# Build libwiicommon
echo "Building libemucommon..."
make || { fail 'Error building libemucommon.'; }
