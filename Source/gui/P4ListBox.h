#pragma once
typedef int (*PTRFUNC)(UINT flags, CPoint pt);

// CP4ListBox

class CP4ListBox : public CListBox
{
	DECLARE_DYNAMIC(CP4ListBox)

public:
	CP4ListBox();
	virtual ~CP4ListBox();

	int AddString(LPCTSTR s);
	int InsertString(int i, LPCTSTR s);
	void ResetContent();
	int DeleteString(int i);
	int get_width() const;
	void SetRightClkCallback(PTRFUNC f) { m_RightClkCallback = (PTRFUNC)f; }

protected:
	PTRFUNC m_RightClkCallback;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonUp( UINT flags, CPoint pt );

private:
	void update_width(LPCTSTR s);
	int width_;
};
