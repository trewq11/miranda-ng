/*
Popup Plus plugin for Miranda IM

Copyright	� 2002 Luca Santarelli,
� 2004-2007 Victor Pavlychko
� 2010 MPK
� 2010 Merlin_de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __opt_class_h__
#define __opt_class_h__

void LoadClassSettings(POPUPTREEDATA *ptd, char* szModul);
void SaveClassSettings(POPUPTREEDATA *ptd, char* szModul);

INT_PTR CALLBACK DlgProcOptsClasses(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // __opt_class_h__
