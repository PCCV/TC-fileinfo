#if !defined(AFX_LISTEXPORT_H__FEFEEF48_7B39_4E6A_902C_F9986749D054__INCLUDED_)
#define AFX_LISTEXPORT_H__FEFEEF48_7B39_4E6A_902C_F9986749D054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListExport.h : header file
//
#include "resource.h"
#include "..\common\ResizePage.h"
#include "peexe\peexe.h"
/////////////////////////////////////////////////////////////////////////////
// CListExport dialog

class CListExport : public CResizePage
{
	DECLARE_DYNCREATE(CListExport)

// Construction
public:
	CListExport();
	~CListExport();
	void Resize(CRect &rectPage);
    void SetPtr(PE_EXE * ptr) { m_pe = ptr; }
	void SetSortOption( BOOL b) { m_bsort = b; }
	void Renew(PE_EXE *pPE);

	CRect   m_rectList;
	CRect   m_rectModule;

	PE_EXE		*m_pe;
//	CStringList m_Mlist, m_Flist;
	int		m_NbIF; //, m_NbDIF;
	int		m_Hsize;
	CFont *font;

//	CListBox	*m_plist;
	DWORD		m_ListStyle;

// Dialog Data
	//{{AFX_DATA(CListExport)
	enum { IDD = IDD_Export };
	CListBox	m_listmodule;
	CListBox	m_list;
	BOOL	m_undecorate;
	BOOL	m_bsort;
	DWORD	m_nbfunc;
	//}}AFX_DATA
protected:
	void CleanUp();

private:
	void OnToggleListStyle();
	void Load();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CListExport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void AddFunction(int sel);
	// Generated message map functions
	//{{AFX_MSG(CListExport)
	afx_msg void OnDestroy( );
   virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeModules();
	afx_msg void Onundecorate();
	afx_msg void OnTestImport();
	afx_msg void OnSelchangeFunc();
	afx_msg void OnSort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTEXPORT_H__FEFEEF48_7B39_4E6A_902C_F9986749D054__INCLUDED_)
