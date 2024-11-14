#if !defined(AFX_CLISTOCX_H__DE6B879B_C4B0_4248_81D0_A311D5A7FCCC__INCLUDED_)
#define AFX_CLISTOCX_H__DE6B879B_C4B0_4248_81D0_A311D5A7FCCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListOCX.h : header file
//
#include "resource.h"
#include "..\common\ResizePage.h"
#include "peexe\peexe.h"
#include "..\..\wincmd\common\TCutils.h"

// typedef void ( * pfuncOcx )(PVOID ptr, CListCtrl &, CRichEditCtrl &);
/////////////////////////////////////////////////////////////////////////////
// CListOcx dialog
typedef HRESULT (CALLBACK *HCRET)(void);
class CListOcx : public CResizePage
{
	DECLARE_DYNCREATE(CListOcx)
// Construction
public:
	CListOcx();   // standard constructor
	void Resize(CRect &m_rectPage);
	void SetPtr(PE_EXE *ptr) { m_pPE = ptr; }
//	void SetFillList(pfuncOcx FT) { FillList = FT; }
	void Renew(PE_EXE *pPE);

// Dialog Data
	//{{AFX_DATA(CListOcx)
	enum { IDD = IDD_OCX };
	CButton	m_bunreg;
	CButton	m_breg;
	CListCtrl	m_listocx;
	CRichEditCtrl	m_ocx;
	CString	m_REocx;
	//}}AFX_DATA
	CRect	m_rectList;
	CRect	m_rectOcx;
	PE_EXE	*m_pPE;
	bool	m_bwrap;
	FontOptions	m_fo;

	void UpdateFont( void );
	void SetFontPpty( FontOptions fo) { m_fo = fo; if (!m_first) UpdateFont();}
	void SetWrap(bool wrap)
	{ 
		m_ocx.SetTargetDevice(NULL, wrap); // 1 = wrap on, 0 = wrap off
		m_ocx.ShowScrollBar( SB_HORZ , wrap);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListOcx)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CListOcx)
	   virtual BOOL OnInitDialog();
	afx_msg void OnRegServer();
	afx_msg void OnUnRegServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Load();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLISTOCX_H__DE6B879B_C4B0_4248_81D0_A311D5A7FCCC__INCLUDED_)
