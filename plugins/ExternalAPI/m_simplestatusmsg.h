/*

Simple Status Message plugin for Miranda IM
Copyright (C) 2006-2011 Bartosz 'Dezeath' Bia�ek, (C) 2005 Harven

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef M_SIMPLESTATUSMSG_H__
#define M_SIMPLESTATUSMSG_H__

// Change global status mode/message
// wParam = (int)new status, 0 if for current
// lParam = (LPARAM)(TCHAR *)status message
#define MS_SIMPLESTATUSMSG_SETSTATUS "SimpleStatusMsg/SetStatus"

// Bring up the status message dialog
// wParam = 0
// lParam = (LPARAM)(char *)protocol name, NULL if for all protocols
#define MS_SIMPLESTATUSMSG_SHOWDIALOG "SimpleStatusMsg/ShowDialog"

// Force a change of status mode/message. The status message dialog will
// appear depending on settings
// wParam = (int)new status
// lParam = (LPARAM)(char *)protocol name, NULL if for all protocols
// Returns 1 when changed without showing the status message dialog
#define MS_SIMPLESTATUSMSG_CHANGESTATUSMSG "SimpleStatusMsg/ChangeStatusMessage"

// Copy the status message of a contact to clipboard
// wParam = (WPARAM)(HANDLE)hContact
// lParam = 0
#define MS_SIMPLESTATUSMSG_COPYMSG  "SimpleStatusMsg/CopyMsg"

// Go to URL in status message of a contact
// wParam = (WPARAM)(HANDLE)hContact
// lParam = 0
#define MS_SIMPLESTATUSMSG_GOTOURLMSG  "SimpleStatusMsg/GoToURLMsg"

#endif // M_SIMPLESTATUSMSG_H__
