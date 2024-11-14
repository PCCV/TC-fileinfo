// PageTree.cpp : implementation file
//

#include "stdafx.h"
#include "PageTree.h"

#include "..\..\common\verwin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageTree property page
IMPLEMENT_DYNCREATE(CPageTree, CResizePage)

CPageTree::CPageTree() : CResizePage(CPageTree::IDD)
{
//{{AFX_DATA_INIT(CPageTree)
	FillTree = NULL;
	pImageList = NULL;
//}}AFX_DATA_INIT
}

CPageTree::~CPageTree()
{
	TRACE0("Delete TreePage \n");
	CleanUp();
}

void CPageTree::OnDestroy() 
{
	TRACE0("CPageTree : OnDestroy \n");
	CleanUp();
}

void CPageTree::CleanUp()
{
	TRACE0("CPageTree : CleanUp \n");
	if (pImageList) delete pImageList;
	pImageList = NULL;
}

void CPageTree::DoDataExchange(CDataExchange* pDX)
{
	CResizePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTree)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTree, CResizePage)
	//{{AFX_MSG_MAP(CPageTree)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTree message handlers
void CPageTree::Resize(CRect &rectPage)
{
	CResizePage::Resize(rectPage);

	m_tree.GetWindowRect(&m_rectTree);
	ScreenToClient(&m_rectTree);
	m_rectTree.right -= m_sizeRelChange.cx; // + rectPage.left;
	m_rectTree.bottom -= m_sizeRelChange.cy; // + rectPage.top;
	m_tree.MoveWindow(&m_rectTree);

//	m_Redit.SetSel( 0, 0);
}

void CPageTree::Renew(PVOID pPE) 
{
	CleanUp();
	m_ptr = pPE;
	if (m_tree.m_hWnd) 
		if (FillTree) 
		{	
			CWait wait(this);
			((pfunc) FillTree) (m_ptr, m_tree, wait);

	// Expand the first branch on W95/WNT/W2k  ( à TESTER )
			VERSION ver = GetSystemVersion();
			if (ver.ver >= WND_XP)
				m_tree.Expand(m_tree.GetRootItem(), TVE_EXPAND);
		}
}

BOOL CPageTree::OnInitDialog() 
{
	CResizePage::OnInitDialog();

	CBitmap      bitmap;
	pImageList = new CImageList();
	pImageList->Create(23, 16, ILC_COLOR16 | ILC_MASK, 4, 4);

	bitmap.LoadBitmap(IDB_FOUND);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_NOTFOUND);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_DELAYED);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_NFDELAYED);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_MISSING);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_DELMISSING);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	m_tree.SetImageList(pImageList, TVSIL_NORMAL);

	if (FillTree) 
	{	
		CWait wait(this);
		((pfunc) FillTree) (m_ptr, m_tree, wait);

// Expand the first branch on W95/WNT/W2k  ( à TESTER )
		VERSION ver = GetSystemVersion();
		if (ver.ver >= WND_XP)
			m_tree.Expand(m_tree.GetRootItem(), TVE_EXPAND);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageTree::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN ) 
   {
		if (pMsg->wParam == 0x6a)
		{
			m_tree.Expand(m_tree.GetRootItem(), TVE_TOGGLE);
//			BOOL EnsureVisible( HTREEITEM hItem );
			return 0;
		}
		if (pMsg->wParam == VK_F3)
		{
			SHELLEXECUTEINFO sei  = { sizeof(sei ) };
			sei.fMask = SEE_MASK_DOENVSUBST;
			sei.nShow = SW_SHOWNORMAL;
	//		sei.lpVerb = argv[1];
			sei.lpFile = "%commander_exe%";
			CString com = "/S=L \" " + m_tree.GetItemText(m_tree.GetSelectedItem()) + "\"";
			sei.lpParameters = com;
			if(ShellExecuteEx(&sei) == FALSE)
			{
				CString mess;
				mess.Format("ShellExecuteEx Error \narg : %s",(const char *)com);
				AfxMessageBox(mess,MB_ICONEXCLAMATION);
			}


			//%commander_exe% /S=L "nom du fichier"

//			com.GetBuffer(0);
//			com.ReleaseBuffer();
			return 0;
		}
	}
	return CResizePage::PreTranslateMessage(pMsg);
}
