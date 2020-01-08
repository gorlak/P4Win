#pragma once

#include "P4ImageList.h"

class CP4Image;

class CP4ViewImageList : public CP4ImageList
{
	void MakeBitmap(CP4Image &dst, CP4Image &src, COLORREF bg);
public:
	enum SimpleImages
	{
		VI_REMOTEDEPOT,
		VI_DEPOT,
		VI_FOLDER,
		VI_YOURPENDING,
		VI_THEIRPENDING,
		VI_YOURCHANGE,
		VI_THEIRCHANGE,
		VI_YOUROTHERCHANGE,
		VI_OLDCHANGE,
		VI_BRANCH,
		VI_LABEL,
		VI_JOB,
		VI_STATUS_MSG,
		VI_STATUS_COMPLETION,
		VI_STATUS_WARNING,
		VI_STATUS_ERROR,
		VI_STATUS_DEBUG,
		VI_STATUS_WARNSUMMARY,
		VI_STATUS_BLANK,
		VI_STATUS_TOOL,
		VI_OPENFOLDER,
		VI_YOURCHGUNRES,
		VI_YOUROTHERCHGUNRES,
		VI_YOURCHANGE_SHELVED,
		VI_THEIRCHANGE_SHELVED,
		VI_YOUROTHERCHANGE_SHELVED,
		VI_YOURCHGUNRES_SHELVED,
		VI_YOUROTHERCHGUNRES_SHELVED,
		VI_CLIENT,
		VI_CLIENT_CUR,
		VI_CLIENT_DEF,
		VI_CLIENT_CUR_DEF,
		VI_USER,
		VI_USER_CUR,
		VI_USER_DEF,
		VI_USER_CUR_DEF,
	};
	enum ClientUserStateBadges
	{
		CUSB_USER = 0x0001,
		CUSB_CLIENT = 0x0002,
		CUSB_DEF = 0x0004,
		CUSB_CUR = 0x0008,
		CUSB_NUM_BITS = 4
	};
	enum FileStateBadges
	{
		FSB_GHOST = 0x0001,	// file is not in client view
		FSB_TEXT = 0x0002,
		FSB_BINARY = 0x0004,
		FSB_YOUR_EDIT = 0x0008,
		FSB_YOUR_DELETE = 0x0010,
		FSB_YOUR_ADD = 0x0020,
		FSB_THEIR_EDIT = 0x0040,
		FSB_THEIR_DELETE = 0x0080,
		FSB_THEIR_ADD = 0x0100,
		FSB_YOUR_LOCK = 0x0200,
		FSB_THEIR_LOCK = 0x0400,
		FSB_SYNCED = 0x0800,
		FSB_NOT_SYNCED = 0x1000,
		FSB_NUM_BITS = 13,
	};

	virtual bool Create();

	static int GetCUIndex(int state);
	static int GetClientIndex(bool isCurrent, bool isDefault);
	static int GetUserIndex(bool isCurrent, bool isDefault);
	static int GetFileIndex(int state);
	static int GetChangeIndex(bool yourClient, bool yourUser, bool unresolved, bool shelved);
};

class CP4WinToolBarImageList : public CP4ToolBarImageList
{
public:
	enum Images
	{
		TBI_SYNC,
		TBI_EDIT,
		TBI_ADD,
		TBI_DELETE,
		TBI_REVERT,
		TBI_LOCK,
		TBI_UNLOCK,
		TBI_BOOKMARKS,
		TBI_CONNECTIONS,
		TBI_SUBMIT,
		TBI_HISTORY,
		TBI_DIFF,
		TBI_QUICKBROWSE,
		TBI_QUICKEDIT,
		TBI_PENDING,
		TBI_SUBMITTED,
		TBI_BRANCH,
		TBI_LABEL,
		TBI_CLIENT,
		TBI_USER,
		TBI_JOB,
		TBI_SETTINGS,
		TBI_INFO,
		TBI_SETFILTER,
		TBI_VIEWOPTIONS,
		TBI_COLUMNS,
		TBI_REFRESH,
		TBI_CLEARFILTER,
		TBI_CANCELBUTTON,
		TBI_DUMMY
	};

	virtual bool Create();
};