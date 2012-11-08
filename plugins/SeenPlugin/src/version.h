/*

'File Association Manager'-Plugin for Miranda IM

Copyright (C) 2005-2007 H. Herkenrath

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (AssocMgr-License.txt); if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define __MAJOR_VERSION				5
#define __MINOR_VERSION				0
#define __RELEASE_NUM				4
#define __BUILD_NUM					7

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#define __PLUGIN_NAME				"Last Seen Mod"
#define __INTERNAL_NAME				"Last Seen"
#define __FILENAME					"SeenPlugin.dll"
#define __DESCRIPTION 				"Logs when a user was last seen online and which users were online while you were away."
#define __AUTHOR					"Heiko Schillinger, YB"
#define __AUTHOREMAIL				"y_b@saaplugin.no-ip.info"
#define __AUTHORWEB					"http://miranda-ng.org/"
#define __COPYRIGHT					"� 2001-2002 Heiko Schillinger, 2003 modified by Bruno Rino, 2005-7 Modified by YB"
