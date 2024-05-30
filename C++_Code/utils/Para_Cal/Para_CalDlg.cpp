
// Para_CalDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Para_Cal.h"
#include "Para_CalDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CParaCalDlg 对话框



CParaCalDlg::CParaCalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARA_CAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CParaCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATALIST, m_DataList);
}

BEGIN_MESSAGE_MAP(CParaCalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CAL, &CParaCalDlg::OnBnClickedButtonCal)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CParaCalDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_ORIIN, &CParaCalDlg::OnBnClickedButtonOriin)
	ON_BN_CLICKED(IDC_BUTTON_PREIN, &CParaCalDlg::OnBnClickedButtonPrein)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CParaCalDlg::OnBnClickedButtonOutput)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATALIST, &CParaCalDlg::OnLvnItemchangedListDatalist)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CParaCalDlg::OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CParaCalDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_BAND, &CParaCalDlg::OnBnClickedButtonBand)
	ON_BN_CLICKED(IDC_BUTTON_MEANBAND, &CParaCalDlg::OnBnClickedButtonMeanband)
	ON_BN_CLICKED(IDC_BUTTON_SPECL, &CParaCalDlg::OnBnClickedButtonSpecl)
END_MESSAGE_MAP()


// CParaCalDlg 消息处理程序

BOOL CParaCalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	iChangeflag = 0;
	GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BAND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MEANBAND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SPECL)->EnableWindow(FALSE);
	m_DataList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_DataList.InsertColumn(0, _T("NAME"), LVCFMT_CENTER, 160);
	m_DataList.InsertColumn(1, _T("MPSNR"), LVCFMT_CENTER, 80);
	m_DataList.InsertColumn(2, _T("MSSIM"), LVCFMT_CENTER, 80);
	m_DataList.InsertColumn(3, _T("MSAD"), LVCFMT_CENTER, 80);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CParaCalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CParaCalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CParaCalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CParaCalDlg::OnBnClickedButtonCal()
{
	CString sPathInOri;
	CString sPathInPre;
	GetDlgItem(IDC_EDIT_ORIPATH)->GetWindowTextA(sPathInOri);
	GetDlgItem(IDC_EDIT_PREPATH)->GetWindowTextA(sPathInPre);

	if (sPathInOri == "" || sPathInPre == "")
	{
		AfxMessageBox("原始图像路径或去噪图像路径缺失");
		return;
	}
		
	if (!ParaCaler.Set_pDataOri(sPathInOri))
	{
		AfxMessageBox("读入原始图像错误");
		return;
	}

	if(!ParaCaler.Set_pDataPre(sPathInPre))
	{
		AfxMessageBox("读入去噪图像错误");
		return;
	}

	if(ParaCaler.Cal_Para() == 0.0)
	{
		AfxMessageBox("计算参数错误");
		return;
	}

	double MPSNR = ParaCaler.Get_MPSNR();
	double MSSIM = ParaCaler.Get_MSSIM();
	double MSAD = ParaCaler.Get_MSAD();

	CString sMPSNR;
	CString sMSSIM;
	CString sMSAD;
	sMPSNR.Format("%.2f", MPSNR);
	sMSSIM.Format("%.2f", MSSIM);
	sMSAD.Format("%.2f", MSAD);

	SetDlgItemTextA(IDC_EDIT_MPSNR, sMPSNR);
	SetDlgItemTextA(IDC_EDIT_MSSIM, sMSSIM);
	SetDlgItemTextA(IDC_EDIT_MSAD, sMSAD);

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_BAND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MEANBAND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SPECL)->EnableWindow(TRUE);
}


void CParaCalDlg::OnBnClickedButtonAdd()
{
	CString sName;
	GetDlgItem(IDC_EDIT_NAME)->GetWindowTextA(sName);

	if (sName == "")
	{
		AfxMessageBox("测试名称未指定");
		return;
	}

	ParaCaler.Set_Node(sName);

	ShowNodeData();

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);

	SetDlgItemText(IDC_EDIT_MPSNR, "");
	SetDlgItemText(IDC_EDIT_MSSIM, "");
	SetDlgItemText(IDC_EDIT_MSAD, "");
	SetDlgItemText(IDC_EDIT_NAME, "");
	SetDlgItemText(IDC_EDIT_PREPATH, "");
}

void CParaCalDlg::ShowNodeData()
{
	m_DataList.DeleteAllItems();

	ParameterNode* DataNode = new ParameterNode;

	if (DataNode)
	{
		DataNode->NextNode = ParaCaler.Get_Node()->NextNode;
	}
	else
	{
		AfxMessageBox("开辟内存空间错误");
		return;
	}
	

	int iCount = 0;

	while (DataNode->NextNode != NULL)
	{
		int nRow = m_DataList.InsertItem(iCount, DataNode->NextNode->sDataName);

		CString sMPSNR;
		sMPSNR.Format("%.2f", DataNode->NextNode->dMPSNR);
		m_DataList.SetItemText(nRow, 1, sMPSNR);

		CString sMSSIM;
		sMSSIM.Format("%.2f", DataNode->NextNode->dMSSIM);
		m_DataList.SetItemText(nRow, 2, sMSSIM);

		CString sMSAD;
		sMSAD.Format("%.2f", DataNode->NextNode->dMSAD);
		m_DataList.SetItemText(nRow, 3, sMSAD);

		DataNode->NextNode = DataNode->NextNode->NextNode;

		iCount++;
	}

	delete(DataNode);
}


void CParaCalDlg::OnBnClickedButtonOriin()
{
	const TCHAR* filter = _T("栅格数据(*.tif)|*.tif||");
	CFileDialog dlg(TRUE, _T("tif"), NULL, 0, filter, this);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("原始图像路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();
	}
	SetDlgItemTextA(IDC_EDIT_ORIPATH, filepath);

	SetDlgItemTextA(IDC_EDIT_MPSNR, "");
	SetDlgItemTextA(IDC_EDIT_MSSIM, "");
	SetDlgItemTextA(IDC_EDIT_NAME, "");
}


void CParaCalDlg::OnBnClickedButtonPrein()
{
	const TCHAR* filter = _T("栅格数据(*.tif)|*.tif||");
	CFileDialog dlg(TRUE, _T("tif"), NULL, 0, filter, this);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("去噪图像路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();
	}
	SetDlgItemTextA(IDC_EDIT_PREPATH, filepath);

	SetDlgItemTextA(IDC_EDIT_MPSNR, "");
	SetDlgItemTextA(IDC_EDIT_MSSIM, "");
	SetDlgItemTextA(IDC_EDIT_NAME, "");
}


void CParaCalDlg::OnBnClickedButtonOutput()
{
	const TCHAR* filter = _T("文本文件(*.txt)|*.txt||");
	CFileDialog dlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("数据保存路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();

		if (ParaCaler.WriteData(filepath) == 0)
		{
			AfxMessageBox("写出路径出错");
			return;
		}
	}
	
	SetDlgItemTextA(IDC_EDIT_PATHOUT, filepath);

	return;
}


void CParaCalDlg::OnLvnItemchangedListDatalist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState & LVIS_SELECTED)
	{
		iItemNum = pNMLV->iItem;

		GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(TRUE);

		CString sName = ParaCaler.Get_NodeName(iItemNum + 1);

		SetDlgItemText(IDC_EDIT_CHANGENAME, sName);
	}
	*pResult = 0;
}


void CParaCalDlg::OnBnClickedButtonChange()
{
	CString sNameChange;
	GetDlgItem(IDC_EDIT_CHANGENAME)->GetWindowTextA(sNameChange);

	if (sNameChange == "")
	{
		AfxMessageBox("未输入修改名称");
		return;
	}

	if (!ParaCaler.ChangeNode(iItemNum + 1, sNameChange))
	{
		AfxMessageBox("修改错误");
		return;
	}

	ShowNodeData();

	GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);
	return;
}


void CParaCalDlg::OnBnClickedButtonDelete()
{
	if (!ParaCaler.DeleteNode(iItemNum + 1))
	{
		AfxMessageBox("删除错误");
		return;
	}

	ShowNodeData();

	GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);
	return;
}


void CParaCalDlg::OnBnClickedButtonBand()
{
	int iNum = 0;

	CString sNum;
	GetDlgItem(IDC_EDIT_BANDNUM)->GetWindowTextA(sNum);

	if (sNum == "")
	{
		AfxMessageBox("波段数未指定");
		return;
	}

	char c;

	for (int i = 0; i < sNum.GetLength(); i++)

	{

		c = sNum.GetAt(i);

		if (c >= '0' && c <= '9' || c == '.')

			continue;

		else

			AfxMessageBox("输入的不是数字");

	}
	
	iNum = atoi(sNum);

	const TCHAR* filter = _T("文本文件(*.txt)|*.txt||");
	CFileDialog dlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("数据保存路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();

		if (ParaCaler.WriteData(filepath) == 0)
		{
			AfxMessageBox("写出路径出错");
			return;
		}
	}

	if (ParaCaler.StripeSpec(filepath, iNum) == 1)
	{
		AfxMessageBox("输入波段大于总波段数");
		return;
	}
	else if (ParaCaler.StripeSpec(filepath, iNum) == 2)
	{
		AfxMessageBox("输入波段小于0");
		return;
	}
	else if (ParaCaler.StripeSpec(filepath, iNum) == 3)
	{
		AfxMessageBox("导出失败");
		return;
	}

	return;
}


void CParaCalDlg::OnBnClickedButtonMeanband()
{
	const TCHAR* filter = _T("文本文件(*.txt)|*.txt||");
	CFileDialog dlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("数据保存路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();

		if (ParaCaler.WriteData(filepath) == 0)
		{
			AfxMessageBox("写出路径出错");
			return;
		}
	}

	if (ParaCaler.StripeSpecMean(filepath) == 1)
	{
		AfxMessageBox("输入失败");
		return;
	}

	return;
}


void CParaCalDlg::OnBnClickedButtonSpecl()
{
	CString sHeightNum;
	GetDlgItem(IDC_EDIT_HEIGHTNUM)->GetWindowTextA(sHeightNum);

	CString sWidthNum;
	GetDlgItem(IDC_EDIT_WIDTHNUM)->GetWindowTextA(sWidthNum);

	if (sHeightNum == "")
	{
		AfxMessageBox("高度位置未指定");
		return;
	}

	if (sWidthNum == "")
	{
		AfxMessageBox("宽度位置未指定");
		return;
	}

	char c;

	for (int i = 0; i < sHeightNum.GetLength(); i++)

	{

		c = sHeightNum.GetAt(i);

		if (c >= '0' && c <= '9' || c == '.')

			continue;

		else

			AfxMessageBox("输入的不是数字");

	}

	for (int i = 0; i < sWidthNum.GetLength(); i++)

	{

		c = sWidthNum.GetAt(i);

		if (c >= '0' && c <= '9' || c == '.')

			continue;

		else

			AfxMessageBox("输入的不是数字");

	}

	const TCHAR* filter = _T("文本文件(*.txt)|*.txt||");
	CFileDialog dlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	//打开对话框的标题
	dlg.m_ofn.lpstrTitle = _T("数据保存路径选择");

	CString filepath;
	if (IDOK == dlg.DoModal())
	{
		filepath = dlg.GetPathName();

		if (ParaCaler.WriteData(filepath) == 0)
		{
			AfxMessageBox("写出路径出错");
			return;
		}
	}

	int iHeightNum = atoi(sHeightNum);
	int iWidthNum = atoi(sWidthNum);

	if (ParaCaler.PixelSpec(filepath, iHeightNum, iWidthNum) == 1)
	{
		AfxMessageBox("导出失败");
	}

	return;
}

