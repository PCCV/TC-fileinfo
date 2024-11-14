#if !defined( __resizepage_H )
#define __resizepage_H

#if !defined(AFX_RESIZEPAGE_H__0CD00AF5_B9A4_4D56_926E_5F19F6DE0FE0__INCLUDED_)
#define AFX_RESIZEPAGE_H__0CD00AF5_B9A4_4D56_926E_5F19F6DE0FE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResizePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResizePage dialog

class CResizePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CResizePage)
protected:
	CResizePage() { };
	CSize	m_sizeRelChange;
	CRect	m_oldRect;
	BOOL	m_first;
	bool	m_editmode;
	bool	m_forceredraw;
// Construction
public:
	CResizePage(UINT nIDTemplate);
	~CResizePage();
	virtual void Init() { m_first = TRUE; }
	virtual void Resize(CRect &rectPage);
	inline bool IsEditable() { return m_editmode; }
// Dialog Data
	//{{AFX_DATA(CResizePage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	void CleanUp();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CResizePage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CResizePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZEPAGE_H__0CD00AF5_B9A4_4D56_926E_5F19F6DE0FE0__INCLUDED_)

#endif 