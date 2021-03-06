/*
FTP File plugin
Copyright (C) 2007-2010 Jan Holub

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "job_delete.h"
#include "manager.h"
#include "dbentry.h"
#include "options.h"
#include "utils.h"

Manager *manDlg = NULL;
Manager *Manager::instance = NULL;

extern Options &opt;
extern ServerList &ftpList;

Manager::Manager()
{
}

Manager::~Manager()
{
	for (UINT i = 0; i < m_rootItems.size(); i++)
		delete m_rootItems[i];

	for (UINT i = 0; i < m_items.size(); i++)
		delete m_items[i];

	ImageList_Destroy(m_himlStates);
	DBEntry::cleanupDB();

	instance = NULL;
	manDlg = NULL;
}

void Manager::init()
{
	ServerList::FTP *ftp = ftpList.getSelected();
	if (ftp->m_bEnabled) {
		m_hwnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DLG_MANAGER), NULL, Manager::ManagerDlgProc);
		m_hwndFileTree = GetDlgItem(m_hwnd, IDC_FILELIST);
		initImageList();
		fillTree();
		show();
	}
	else {
		Utils::msgBox(TranslateT("You have to fill and enable at least one FTP server in setting."), MB_OK);
		delete this;
	}
}

void Manager::show()
{
	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	BringWindowToTop(m_hwnd);
}

void Manager::initImageList()
{
	char buff[256];

	m_himlStates = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, ServerList::FTP_COUNT + 4, 0);
	ImageList_AddIcon(m_himlStates, Skin_LoadIcon(SKINICON_OTHER_DELETE)); // image index 0 is useless for INDEXTOSTATEIMAGEMASK
	ImageList_AddIcon(m_himlStates, Skin_LoadIcon(SKINICON_OTHER_DELETE));
	ImageList_AddIcon(m_himlStates, Skin_LoadIcon(SKINICON_OTHER_NOTICK));
	ImageList_AddIcon(m_himlStates, Skin_LoadIcon(SKINICON_OTHER_TICK));

	for (int i = 0; i < ServerList::FTP_COUNT; i++) {
		mir_snprintf(buff, "ftp%d", i);
		ImageList_AddIcon(m_himlStates, Utils::loadIconEx(buff));
	}

	TreeView_SetImageList(m_hwndFileTree, m_himlStates, TVSIL_STATE);
	TreeView_SetItemHeight(m_hwndFileTree, 18);
}

void Manager::initRootItems()
{
	TVINSERTSTRUCT tvi = { 0 };
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_TEXT | TVIF_STATE;
	tvi.item.stateMask = TVIS_STATEIMAGEMASK | TVIS_EXPANDED | TVIS_BOLD;

	for (UINT i = 0; i < ServerList::FTP_COUNT; i++) {
		if (ftpList[i]->m_bEnabled) {
			tvi.item.pszText = ftpList[i]->m_stzName;
			tvi.item.state = INDEXTOSTATEIMAGEMASK(i + 4) | TVIS_EXPANDED | TVIS_BOLD;
			HTREEITEM hItem = TreeView_InsertItem(m_hwndFileTree, &tvi);
			AddRoot(hItem);
		}
	}
}

void Manager::fillTree()
{
	initRootItems();

	TVINSERTSTRUCT tvi = { 0 };
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_TEXT | TVIF_STATE;
	tvi.item.stateMask = TVIS_STATEIMAGEMASK;
	tvi.item.state = TreeItem::_UNCHECKED();

	mir_cslock lock(DBEntry::mutexDB);

	DBEntry *entry = DBEntry::getFirst();
	while (entry != NULL) {
		if ((UINT)entry->m_iFtpNum < m_rootItems.size()) {
			tvi.item.pszText = mir_a2u(entry->m_szFileName);
			tvi.hParent = m_rootItems[entry->m_iFtpNum]->m_handle;
			HTREEITEM hItem = TreeView_InsertItem(m_hwndFileTree, &tvi);
			AddLeaf(hItem, tvi.hParent, entry->m_fileID);
			FREE(tvi.item.pszText);
		}

		entry = DBEntry::getNext(entry);
	}
}

int Manager::indexOf(HTREEITEM handle)
{
	for (UINT i = 0; i < m_rootItems.size(); i++) {
		if (m_rootItems[i]->m_handle == handle)
			return i;
	}

	return -1;
}

Manager::TreeItem *Manager::getItem(HTREEITEM handle)
{
	for (UINT i = 0; i < m_rootItems.size(); i++) {
		if (m_rootItems[i]->m_handle == handle)
			return m_rootItems[i];
	}

	for (UINT i = 0; i < m_items.size(); i++) {
		if (m_items[i]->m_handle == handle)
			return m_items[i];
	}

	return NULL;
}

Manager::TreeItem::TreeItem(HTREEITEM _handle, HTREEITEM _parent, int _id) :
	m_handle(_handle),
	m_parent(_parent),
	m_fileID(_id)
{
	m_tszToolTip[0] = 0;
}

void Manager::TreeItem::setState(UINT state)
{
	TVITEM item = { 0 };
	item.mask = TVIF_HANDLE | TVIF_STATE;
	item.stateMask = TVIS_STATEIMAGEMASK;
	item.hItem = m_handle;
	TreeView_GetItem(manDlg->m_hwndFileTree, &item);

	if (TreeItem::_GETSTATE(item.state) != 0) {
		item.state = state;
		TreeView_SetItem(manDlg->m_hwndFileTree, &item);
	}
}

UINT Manager::TreeItem::getState()
{
	TVITEM item = { 0 };
	item.mask = TVIF_HANDLE | TVIF_STATE;
	item.stateMask = TVIS_STATEIMAGEMASK;
	item.hItem = m_handle;
	TreeView_GetItem(manDlg->m_hwndFileTree, &item);
	return TreeItem::_GETSTATE(item.state);
}

void Manager::TreeItem::toggleState()
{
	TVITEM item = { 0 };
	item.mask = TVIF_HANDLE | TVIF_STATE;
	item.stateMask = TVIS_STATEIMAGEMASK;
	item.hItem = m_handle;
	TreeView_GetItem(manDlg->m_hwndFileTree, &item);

	UINT state = TreeItem::_GETSTATE(item.state);
	if (state == STATE_UNCHECKED)
		item.state = TreeItem::_CHECKED();
	else if (state == STATE_CHECKED || state == STATE_ERROR)
		item.state = TreeItem::_UNCHECKED();

	TreeView_SetItem(manDlg->m_hwndFileTree, &item);
}

void Manager::TreeItem::remove()
{
	TreeView_DeleteItem(manDlg->m_hwndFileTree, m_handle);
	DBEntry::remove(m_fileID);
}

bool Manager::TreeItem::isRoot()
{
	return (m_parent != NULL) ? false : true;
}


INT_PTR CALLBACK Manager::ManagerDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		TranslateDialogDefault(hwndDlg);
		Window_SetIcon_IcoLib(hwndDlg, Utils::getIconHandle("main"));

		SendDlgItemMessage(hwndDlg, IDC_BTN_SELECTALL, BUTTONSETASFLATBTN, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_BTN_SELECTALL, BM_SETIMAGE, IMAGE_ICON, (LPARAM)Skin_LoadIcon(SKINICON_OTHER_TICK));
		SendDlgItemMessage(hwndDlg, IDC_BTN_SELECTALL, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Select All"), BATF_UNICODE);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DESELECTALL, BUTTONSETASFLATBTN, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DESELECTALL, BM_SETIMAGE, IMAGE_ICON, (LPARAM)Skin_LoadIcon(SKINICON_OTHER_NOTICK));
		SendDlgItemMessage(hwndDlg, IDC_BTN_DESELECTALL, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Clear All"), BATF_UNICODE);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETEFROMLIST, BUTTONSETASFLATBTN, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETEFROMLIST, BM_SETIMAGE, IMAGE_ICON, (LPARAM)Utils::loadIconEx("clear"));
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETEFROMLIST, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Delete from List"), BATF_UNICODE);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETE, BUTTONSETASFLATBTN, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)Utils::loadIconEx("delete"));
		SendDlgItemMessage(hwndDlg, IDC_BTN_DELETE, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Delete from FTP"), BATF_UNICODE);
		SendDlgItemMessage(hwndDlg, IDC_BTN_CLOSE, BUTTONSETASFLATBTN, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_BTN_CLOSE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)Skin_LoadIcon(SKINICON_OTHER_EXIT));
		SendDlgItemMessage(hwndDlg, IDC_BTN_CLOSE, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Close"), BATF_UNICODE);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			switch (LOWORD(wParam)) {
			case IDC_BTN_DELETE:
				for (UINT i = 0; i < manDlg->m_items.size(); i++) {
					TreeItem *item = manDlg->m_items[i];
					if (item->getState() == STATE_CHECKED) {
						DeleteJob *job = new DeleteJob(DBEntry::get(item->m_fileID), item);
						job->start();
					}
				}
				break;

			case IDC_BTN_DELETEFROMLIST:
				for (UINT i = 0; i < manDlg->m_items.size(); i++) {
					TreeItem *item = manDlg->m_items[i];
					if (item->getState() == STATE_CHECKED)
						item->remove();
				}
				break;

			case IDC_BTN_SELECTALL:
			case IDC_BTN_DESELECTALL:
				{
					UINT newState = (LOWORD(wParam) == IDC_BTN_SELECTALL) ?
						TreeItem::_CHECKED() : TreeItem::_UNCHECKED();

					for (UINT i = 0; i < manDlg->m_items.size(); i++)
						manDlg->m_items[i]->setState(newState);

				}
				break;

			case IDC_BTN_CLOSE:
				DestroyWindow(hwndDlg);
				break;
			}
		}
		break;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom == IDC_FILELIST) {
			switch (((LPNMHDR)lParam)->code) {
			case TVN_KEYDOWN:
				if (((LPNMTVKEYDOWN)lParam)->wVKey != VK_SPACE)
					break;

			case NM_CLICK:
				{
					HTREEITEM hItem;
					TVHITTESTINFO hti = { 0 };
					hti.pt.x = (short)LOWORD(GetMessagePos());
					hti.pt.y = (short)HIWORD(GetMessagePos());
					ScreenToClient(((LPNMHDR)lParam)->hwndFrom, &hti.pt);
					if (TreeView_HitTest(((LPNMHDR)lParam)->hwndFrom, &hti) || ((LPNMHDR)lParam)->code == TVN_KEYDOWN) {
						if (((LPNMHDR)lParam)->code == TVN_KEYDOWN) {
							hti.flags |= TVHT_ONITEMSTATEICON;
							hItem = TreeView_GetSelection(((LPNMHDR)lParam)->hwndFrom);
						}
						else {
							hItem = hti.hItem;
						}

						TreeItem *item = manDlg->getItem(hItem);
						if (item && (hti.flags & TVHT_ONITEMSTATEICON)) {
							if (item->isRoot()) {
								for (UINT i = 0; i < manDlg->m_items.size(); i++) {
									if (manDlg->m_items[i]->m_parent == item->m_handle)
										manDlg->m_items[i]->toggleState();
								}
							}
							else {
								item->toggleState();
							}
						}
					}
				}
				return TRUE;

			case NM_RCLICK:
				{
					TVHITTESTINFO hti;
					hti.pt.x = (short)LOWORD(GetMessagePos());
					hti.pt.y = (short)HIWORD(GetMessagePos());
					ScreenToClient(manDlg->m_hwndFileTree, &hti.pt);
					if (TreeView_HitTest(manDlg->m_hwndFileTree, &hti)) {
						HTREEITEM hItem = hti.hItem;
						TreeItem *item = manDlg->getItem(hItem);
						if (item && !item->isRoot()) {
							POINT pt;
							GetCursorPos(&pt);
							SetForegroundWindow(hwndDlg);
							HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_MANAGER));
							if (hMenu) {
								HMENU hPopupMenu = GetSubMenu(hMenu, 0);
								TranslateMenu(hPopupMenu);
								int command = TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwndDlg, NULL);
								switch (command) {
								case IDM_DELETEFROMLIST:
									item->remove();
									break;

								case IDM_DELETEFROMFTP:
									(new DeleteJob(DBEntry::get(item->m_fileID), item))->start();
									break;

								case IDM_COPYLINK:
								case IDM_DOWNLOAD:
									int ftpNum = manDlg->indexOf(item->m_parent);
									if (ftpNum != -1) {
										char buff[256];

										DBEntry *entry = DBEntry::get(item->m_fileID);
										Utils::createFileDownloadLink(ftpList[ftpNum]->m_szUrl, entry->m_szFileName, buff, sizeof(buff));
										delete entry;

										if (command == IDM_COPYLINK)
											Utils::copyToClipboard(buff);
										else
											ShellExecuteA(NULL, "open", buff, NULL, NULL, SW_SHOWNORMAL);
									}
									break;
								}
								DestroyMenu(hMenu);
							}
						}
					}
					return TRUE;
				}
			case TVN_GETINFOTIP:
				NMTVGETINFOTIP *tvInfoTip = (NMTVGETINFOTIP *)lParam;
				TreeItem *item = manDlg->getItem(tvInfoTip->hItem);

				if (item) {
					if (item->m_tszToolTip[0]) {
						wcsncpy(tvInfoTip->pszText, item->m_tszToolTip, tvInfoTip->cchTextMax - 1);
						tvInfoTip->pszText[tvInfoTip->cchTextMax - 1] = 0;
					}
				}

				return TRUE;
			}
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		return TRUE;

	case WM_DESTROY:
		delete manDlg;
		return TRUE;
	}

	return FALSE;
}
