
// ATVDLRDenoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ATVDLRDenoise.h"
#include "ATVDLRDenoiseDlg.h"
#include "afxdialogex.h"

#include"t_SVD.h"
#include"t_Product.h"
#include"t_ConTran.h"
#include"m_NuclearNorm.h"
#include"m_Tran.h"
#include"m_Multi.h"
#include"m_SVD.h"

#include"m_Lemma.h"
#include"t_Lemma.h"

#include"t_XSub.h"
#include"t_TSub.h"

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


// CATVDLRDenoiseDlg 对话框



CATVDLRDenoiseDlg::CATVDLRDenoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ATVDLRDENOISE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CATVDLRDenoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CATVDLRDenoiseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CATVDLRDenoiseDlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// CATVDLRDenoiseDlg 消息处理程序

BOOL CATVDLRDenoiseDlg::OnInitDialog()
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

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//使得GDAL不默认使用UTF8编码，以支持中文路径

	mclmcrInitialize();
	if (!mclInitializeApplication(NULL, 0))
	{
		return TRUE;
	}

	if (!m_LemmaInitialize())
	{
		return TRUE;
	}

	if (!t_LemmaInitialize())
	{
		return TRUE;
	}

	if (!t_TSubInitialize())
	{
		return TRUE;
	}

	if (!t_XSubInitialize())
	{
		return TRUE;
	}
	
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CATVDLRDenoiseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CATVDLRDenoiseDlg::OnPaint()
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
HCURSOR CATVDLRDenoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CATVDLRDenoiseDlg::OnBnClickedButtonRun()
{
	CString sPathTifIn = "D:\\TestData\\TestImage\\Stripe_WP60_BP60_M0_R20_Pre.tif";
	CString sPathTifOut = "D:\\TestData\\TestImage\\Out\\";

	pHSI = ReadTifIn(sPathTifIn);
	if (pHSI == NULL)
	{
		AfxMessageBox("图片读取失败");
	}

	MallocData();

	Copy(pHSI, H, iHeight, iWidth, iBandNum);
	Copy(H, X, iHeight, iWidth, iBandNum);
	Copy(H, Z12f, iHeight, iWidth, iBandNum);
	Copy(H, Z13f, iHeight, iWidth, iBandNum);
	Copy(H, Z23f, iHeight, iWidth, iBandNum);

	//WriteTifOut(H, "D:\\TestData\\TestImage\\Out\\H.tif", iHeight, iWidth, iBandNum);

	for (int ii = 0; ii < 10; ii++)
	{
		CString sii;
		sii.Format("%d", ii);
		CString sOutZ12 = sPathTifOut + "Z12_" + sii + ".tif";
		CString sOutZ13 = sPathTifOut + "Z13_" + sii + ".tif";
		CString sOutZ23 = sPathTifOut + "Z23_" + sii + ".tif";

		CString sOutX = sPathTifOut + "X_" + sii + ".tif";
		CString sOutS = sPathTifOut + "S_" + sii + ".tif";
		CString sOutB = sPathTifOut + "B_" + sii + ".tif";
		CString sOutN = sPathTifOut + "N_" + sii + ".tif";
		CString sOutHX = sPathTifOut + "HX_" + sii + ".tif";
		CString sOutT = sPathTifOut + "T_" + sii + ".tif";
		CString sOutJ = sPathTifOut + "J_" + sii + ".tif";
		CString sOutP = sPathTifOut + "P_" + sii + ".tif";
		CString sOutQ = sPathTifOut + "Q_" + sii + ".tif";

		CString sOutMp = sPathTifOut + "Mp_" + sii + ".tif";
		CString sOutMq = sPathTifOut + "Mq_" + sii + ".tif";
		CString sOutMb = sPathTifOut + "Mb_" + sii + ".tif";
		CString sOutMt = sPathTifOut + "Mt_" + sii + ".tif";
		CString sOutMh = sPathTifOut + "Mh_" + sii + ".tif";

		Grad_y_t(J, DvtJ, iHeight, iWidth, iBandNum);
		Grad_y_t(Mt, DvtMt, iHeight, iWidth, iBandNum);

		Update_Z();
		//WriteTifOut(Z12f, sOutZ12, iHeight, iWidth, iBandNum);
		//WriteTifOut(Z13f, sOutZ13, iHeight, iWidth, iBandNum);
		//WriteTifOut(Z23f, sOutZ23, iHeight, iWidth, iBandNum);

		Update_S();
		//WriteTifOut(S, sOutS, iHeight, iWidth, iBandNum);

		Update_X();
		WriteTifOut(X, sOutX, iHeight, iWidth, iBandNum);
		
		Update_P();
		//WriteTifOut(P, sOutP, iHeight, iWidth, iBandNum);

		Update_Q();
		//WriteTifOut(Q, sOutQ, iHeight, iWidth, iBandNum);

		Update_B();
		//WriteTifOut(B, sOutB, iHeight, iWidth, iBandNum);

		Update_T();
		//WriteTifOut(T, sOutT, iHeight, iWidth, iBandNum);

		Update_J();
		//WriteTifOut(J, sOutJ, iHeight, iWidth, iBandNum);

		LagrangianMultiplierUpdate();

		CalN();
		//WriteTifOut(N, sOutN, iHeight, iWidth, iBandNum);

		Subtract(H, X, H_X, iHeight, iWidth, iBandNum);
		//WriteTifOut(H_X, sOutHX, iHeight, iWidth, iBandNum);

		//WriteTifOut(Mp, sOutMp, iHeight, iWidth, iBandNum);
		//WriteTifOut(Mq, sOutMq, iHeight, iWidth, iBandNum);
		//WriteTifOut(Mb, sOutMb, iHeight, iWidth, iBandNum);
		//WriteTifOut(Mt, sOutMt, iHeight, iWidth, iBandNum);
		//WriteTifOut(Mh, sOutMh, iHeight, iWidth, iBandNum);

		μ = ρ * μ;
		if (μ > μmax)
			μ = μmax;
	}

	//WriteTifOut(pHSI, sPathTifOut, iHeight, iWidth, iBandNum);

	DeleteData();

	m_LemmaTerminate();
	t_LemmaTerminate();
	t_XSubTerminate();
	t_TSubTerminate();

	AfxMessageBox("Done");
	return;
	
	// TODO: 在此添加控件通知处理程序代码
}




/*	Z	参	数	更	新	*/
/******************************************************************************************************************************************************************************************************************************************************/

//Z参数更新总函数
//调用其他Z参数更新函数
void CATVDLRDenoiseDlg::Update_Z()
{
	Update_Z12();
	Update_Z13();
	Update_Z23();
}

//Z12的更新函数
void CATVDLRDenoiseDlg::Update_Z12()
{
	int iHei = iHeight;
	int iWid = iWidth;
	int iBNm = iBandNum;

	double Thr = ω12 / μ;

	unFold_Mode_12(X, X12, iHeight, iWidth, iBandNum);
	unFold_Mode_12(A12f, A12uf, iHeight, iWidth, iBandNum);
	unFold_Mode_12(Z12f, Z12uf, iHeight, iWidth, iBandNum);

	Divide(A12uf, μ, A12_μ, iHei, iWid, iBNm);
	Add(X12, A12_μ, X12_A12_μ, iHei, iWid, iBNm);

	mwSize mdim[3] = { iHei, iWid, iBNm };
	mwArray mwInputData(3, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(3, mdim, mxDOUBLE_CLASS, mxREAL);

	mwArray mwThr(1, 1, mxDOUBLE_CLASS, mxREAL);
	mwThr(1) = Thr;

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				mwInputData(ii + 1, jj + 1, kk + 1) = X12_A12_μ[kk][ii * iWid + jj];
			}
		}
	}

	t_Lemma(1, mwOutputData, mwInputData, mwThr);

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				Z12uf[kk][ii * iWid + jj] = mwOutputData(ii + 1, jj + 1, kk + 1);
			}
		}
	}

	Fold_Mode_12(Z12uf, Z12f, iHei, iWid, iBNm);
}

//Z13的更新函数
void CATVDLRDenoiseDlg::Update_Z13()
{
	int iHei = iHeight;
	int iWid = iBandNum;
	int iBNm = iWidth;

	double Thr = ω13 / μ;

	unFold_Mode_13(X, X13, iHeight, iWidth, iBandNum);
	unFold_Mode_13(A13f, A13uf, iHeight, iWidth, iBandNum);
	unFold_Mode_13(Z13f, Z13uf, iHeight, iWidth, iBandNum);

	Divide(A13uf, μ, A13_μ, iHei, iWid, iBNm);
	Add(X13, A13_μ, X13_A13_μ, iHei, iWid, iBNm);

	mwSize mdim[3] = { iHei, iWid, iBNm };
	mwArray mwInputData(3, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(3, mdim, mxDOUBLE_CLASS, mxREAL);

	mwArray mwThr(1, 1, mxDOUBLE_CLASS, mxREAL);
	mwThr(1) = Thr;

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				mwInputData(ii + 1, jj + 1, kk + 1) = X13_A13_μ[kk][ii * iWid + jj];
			}
		}
	}

	t_Lemma(1, mwOutputData, mwInputData, mwThr);

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				Z13uf[kk][ii * iWid + jj] = mwOutputData(ii + 1, jj + 1, kk + 1);
			}
		}
	}

	Fold_Mode_13(Z13uf, Z13f, iHei, iWid, iBNm);
}

//Z23的更新函数
void CATVDLRDenoiseDlg::Update_Z23()
{
	int iHei = iWidth;
	int iWid = iBandNum;
	int iBNm = iHeight;

	double Thr = ω23 / μ;

	unFold_Mode_23(X, X23, iHeight, iWidth, iBandNum);
	unFold_Mode_23(A23f, A23uf, iHeight, iWidth, iBandNum);
	unFold_Mode_23(Z23f, Z23uf, iHeight, iWidth, iBandNum);

	Divide(A23uf, μ, A23_μ, iHei, iWid, iBNm);
	Add(X23, A23_μ, X23_A23_μ, iHei, iWid, iBNm);

	mwSize mdim[3] = { iHei, iWid, iBNm };
	mwArray mwInputData(3, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(3, mdim, mxDOUBLE_CLASS, mxREAL);

	mwArray mwThr(1, 1, mxDOUBLE_CLASS, mxREAL);
	mwThr(1) = Thr;

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				mwInputData(ii + 1, jj + 1, kk + 1) = X23_A23_μ[kk][ii * iWid + jj];
			}
		}
	}

	t_Lemma(1, mwOutputData, mwInputData, mwThr);

	for (int ii = 0; ii < iHei; ii++)
	{
		for (int jj = 0; jj < iWid; jj++)
		{
			for (int kk = 0; kk < iBNm; kk++)
			{
				Z23uf[kk][ii * iWid + jj] = mwOutputData(ii + 1, jj + 1, kk + 1);
			}
		}
	}

	Fold_Mode_23(Z23uf, Z23f, iHei, iWid, iBNm);
}


/*	B	参	数	更	新	*/
/******************************************************************************************************************************************************************************************************************************************************/

//B参数更新函数
void CATVDLRDenoiseDlg::Update_B()
{
	double Thr = β / (2 * μ);

	double* tmpIN = (double*)malloc(iHeight * iWidth * sizeof(double));

	for (int ii = 0; ii < iBandNum; ii++)
	{
		mwSize mdimIN[2] = { iHeight, iWidth };
		mwSize mdimBi_k1[2] = { iHeight, iHeight };

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				tmpIN[jj * iWidth + kk] =
					+ H[ii][jj * iWidth + kk] / 2
					- X[ii][jj * iWidth + kk] / 2
					- S[ii][jj * iWidth + kk] / 2
					+ T[ii][jj * iWidth + kk] / 2
					+ Mh[ii][jj * iWidth + kk] / (2 * μ)
					- Mb[ii][jj * iWidth + kk] / (2 * μ);
			}
		}

		mwArray mwIN(2, mdimIN, mxDOUBLE_CLASS, mxREAL);
		mwArray mwBi_k1(2, mdimBi_k1, mxDOUBLE_CLASS, mxREAL);

		mwArray mwThr(1, 1, mxDOUBLE_CLASS, mxREAL);
		mwThr(1) = Thr;

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				mwIN(jj + 1, kk + 1) = tmpIN[jj * iWidth + kk];
			}
		}

		m_Lemma(1, mwBi_k1, mwIN, mwThr);

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				B[ii][jj * iWidth + kk] = mwBi_k1(jj + 1, kk + 1);
			}
		}
	}

	free(tmpIN);
	tmpIN = NULL;
}


/*	Gauss-Seidel  操  作  更  新  */
/******************************************************************************************************************************************************************************************************************************************************/

//Gauss-Seidel更新总函数
//调用所有其他的需要经过Gauss-Seidel操作更新的参数对应的函数
void CATVDLRDenoiseDlg::GaussSeidelUpdata()
{
	for (int ii = 0; ii < 50; ii++)
	{
		Update_X();
	}

	for (int ii = 0; ii < 50; ii++)
	{
		Update_T();
	}
}

//X的FFT操作更新总函数
//调用所有其他的X的Gauss-Seidel操作更新函数
void CATVDLRDenoiseDlg::Update_X()
{
	Grad_x_t(P, DhtP, iHeight, iWidth, iBandNum);
	Grad_x_t(Mp, DhtMp, iHeight, iWidth, iBandNum);

	Grad_z_t(Q, DstQ, iHeight, iWidth, iBandNum);
	Grad_z_t(Mq, DstMq, iHeight, iWidth, iBandNum);

	mwSize mdim[3] = { iHeight, iWidth, iBandNum };
	mwArray mwInputData(3, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(3, mdim, mxDOUBLE_CLASS, mxREAL);

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				mwInputData(jj + 1, kk + 1, ii + 1) = 0
					+ H[ii][jj * iWidth + kk] - S[ii][jj * iWidth + kk] - B[ii][jj * iWidth + kk] + Mh[ii][jj * iWidth + kk] / μ
					+ DhtP[ii][jj * iWidth + kk] + DhtMp[ii][jj * iWidth + kk] / μ
					+ Z12f[ii][jj * iWidth + kk] - A12f[ii][jj * iWidth + kk] / μ
					+ Z13f[ii][jj * iWidth + kk] - A13f[ii][jj * iWidth + kk] / μ
					+ Z23f[ii][jj * iWidth + kk] - A23f[ii][jj * iWidth + kk] / μ
					+ DstQ[ii][jj * iWidth + kk] + DstMq[ii][jj * iWidth + kk] / μ;
			}
		}
	}

	t_XSub(1, mwOutputData, mwInputData);

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				X[ii][jj * iWidth + kk] = mwOutputData(jj + 1, kk + 1, ii + 1);
			}
		}
	}
}

//T的FFT操作更新函数
void CATVDLRDenoiseDlg::Update_T()
{
	Grad_y_t(J, DvtJ, iHeight, iWidth, iBandNum);
	Grad_y_t(Mt, DvtMt, iHeight, iWidth, iBandNum);

	mwSize mdim[3] = { iHeight, iWidth, iBandNum };
	mwArray mwInputData(3, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(3, mdim, mxDOUBLE_CLASS, mxREAL);

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				mwInputData(jj + 1, kk + 1, ii + 1) = 0
					+ B[ii][jj * iWidth + kk] + Mb[ii][jj * iWidth + kk] / μ
					+ DvtJ[ii][jj * iWidth + kk] + DvtMt[ii][jj * iWidth + kk] / μ;
			}
		}
	}

	t_TSub(1, mwOutputData, mwInputData);

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				T[ii][jj * iWidth + kk] = mwOutputData(jj + 1, kk + 1, ii + 1);
			}
		}
	}
}




/*	Shrink  操  作	*/
/******************************************************************************************************************************************************************************************************************************************************/

//Shrink更新总函数
//调用所有其他的需要经过Shrink操作更新的参数对应的函数
void CATVDLRDenoiseDlg::ShrinkUpdata()
{
	Update_S();
	Update_P();
	Update_Q();
	Update_J();
}

//S的Shrink操作更新函数
void CATVDLRDenoiseDlg::Update_S()
{
	double Thr = λ / μ;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			double Temp = H[ii][jj] - X[ii][jj] - B[ii][jj] + Mh[ii][jj] / μ;

			if (Temp > Thr)
				S[ii][jj] = Temp - Thr;
			else if (Temp < 0 - Thr)
				S[ii][jj] = Temp + Thr;
			else
				S[ii][jj] = 0;
		}
	}
}

//P的Shrink操作更新函数
void CATVDLRDenoiseDlg::Update_P()
{
	Grad_x(X, DhX, iHeight, iWidth, iBandNum);

	double Thr = τ1 / μ;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			double Temp = DhX[ii][jj] - Mp[ii][jj] / μ;
			if (Temp > Thr)
				P[ii][jj] = Temp - Thr;
			else if (Temp < 0 - Thr)
				P[ii][jj] = Temp + Thr;
			else
				P[ii][jj] = 0;
		}
	}
}

//Q的Shrink操作更新函数
void CATVDLRDenoiseDlg::Update_Q()
{
	Grad_z(X, DsX, iHeight, iWidth, iBandNum);

	double Thr = τ3 / μ;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			double Temp = DsX[ii][jj] - Mq[ii][jj] / μ;

			if (Temp > Thr)
				Q[ii][jj] = Temp - Thr;
			else if (Temp < 0 - Thr)
				Q[ii][jj] = Temp + Thr;
			else
				Q[ii][jj] = 0;
		}
	}
}

//J的Shrink操作更新函数
void CATVDLRDenoiseDlg::Update_J()
{
	Grad_y(T, DvT, iHeight, iWidth, iBandNum);

	double Thr = τ2 / μ;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			double Temp = DvT[ii][jj] - Mt[ii][jj] / μ;

			if (Temp > Thr)
				J[ii][jj] = Temp - Thr;
			else if (Temp < 0 - Thr)
				J[ii][jj] = Temp + Thr;
			else
				J[ii][jj] = 0;
		}
	}
}




/*	拉	格	朗	日	参	数	更	新	*/
/******************************************************************************************************************************************************************************************************************************************************/

//拉格朗日乘子更新总函数
//调用所有其他的乘子更新函数
void CATVDLRDenoiseDlg::LagrangianMultiplierUpdate()
{
	Update_Ak1k2();
	Update_Mp();
	Update_Mq();
	Update_Mb();
	Update_Mt();
	Update_Mh();
}

//Ak1k2乘子更新函数
void CATVDLRDenoiseDlg::Update_Ak1k2()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			A12f[ii][jj] = A12f[ii][jj] + μ * (X[ii][jj] - Z12f[ii][jj]);
			A13f[ii][jj] = A13f[ii][jj] + μ * (X[ii][jj] - Z13f[ii][jj]);
			A23f[ii][jj] = A23f[ii][jj] + μ * (X[ii][jj] - Z23f[ii][jj]);
		}
	}
}

//Mp乘子更新函数
void CATVDLRDenoiseDlg::Update_Mp()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			Mp[ii][jj] = Mp[ii][jj] + μ * (P[ii][jj] - DhX[ii][jj]);
		}
	}
}

//Mq乘子更新函数
void CATVDLRDenoiseDlg::Update_Mq()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			Mq[ii][jj] = Mq[ii][jj] + μ * (Q[ii][jj] - DsX[ii][jj]);
		}
	}
}

//Mb乘子更新函数
void CATVDLRDenoiseDlg::Update_Mb()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			Mb[ii][jj] = Mb[ii][jj] + μ * (B[ii][jj] - T[ii][jj]);
		}
	}
}

//Mt乘子更新函数
void CATVDLRDenoiseDlg::Update_Mt()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			Mt[ii][jj] = Mt[ii][jj] + μ * (J[ii][jj] - DvT[ii][jj]);
		}
	}
}

//Mh乘子更新函数
void CATVDLRDenoiseDlg::Update_Mh()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			Mh[ii][jj] = Mh[ii][jj] + μ * (H[ii][jj] - X[ii][jj] - S[ii][jj] - B[ii][jj]);
		}
	}
}




/*	张	量	复	制	*/
/******************************************************************************************************************************************************************************************************************************************************/

//张量复制函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
template <class DataType>
void CATVDLRDenoiseDlg::Copy(DataType** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}




/*	张	量	数	值	提	取	与	设	置	*/
/******************************************************************************************************************************************************************************************************************************************************/

//提取张量中某个位置的数值函数
//pInputData：输入张量； i：张量i方向的位置； j：张量j方向的位置； k：张量k方向的位置； 
//iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
double CATVDLRDenoiseDlg::GetTensorData(double** pInputData, int i, int j, int k, int iHei, int iWid, int iBNm)
{
	if (i >= iHei || j >= iWid || k >= iBNm)
	{
		AfxMessageBox("位置输入错误");
		return 0;
	}

	i = (iHei - 1) - i;

	return pInputData[k][i * iWid + j];
}

//设置张量中某个位置的数值函数
//pInputData：输入张量； i：张量i方向的位置； j：张量j方向的位置； k：张量k方向的位置； dData：设置的数值； 
//iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::SetTensorData(double** pInputData, int i, int j, int k, double dData, int iHei, int iWid, int iBNm)
{
	if (i >= iHei || j >= iWid || k >= iBNm)
	{
		AfxMessageBox("位置输入错误");
		return;
	}

	i = (iHei - 1) - i;

	pInputData[k][i * iWid + j] = dData;
}




/*	张	量	运	算	*/
/******************************************************************************************************************************************************************************************************************************************************/

//张量减法函数
//pInputData1：被减数张量； pInputData2：减数张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Subtract(double** pInputData1, double** pInputData2, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = pInputData1[ii][jj] - pInputData2[ii][jj];
		}
	}

	return;
}

//张量加法函数
//pInputData1：加数张量； pInputData2：被减数张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Add(double** pInputData1, double** pInputData2, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = pInputData1[ii][jj] + pInputData2[ii][jj];
		}
	}

	return;
}

//张量除法函数
//pInputData1：被除数张量； pInputData2：除数； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Divide(double** pInputData1, double pInputData2, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = pInputData1[ii][jj] / pInputData2;
		}
	}

	return;
}

//计算高斯噪声
void CATVDLRDenoiseDlg::CalN()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			N[ii][jj] = H[ii][jj] - S[ii][jj] - X[ii][jj] - B[ii][jj];
		}
	}
}



/*	求	导	*/
/******************************************************************************************************************************************************************************************************************************************************/

//张量x向一阶右求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_x(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int kk_r = kk + 1;
				if (kk_r >= iWid)
					kk_r = iWid - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj * iWid + kk_r] - pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量y向一阶下求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_y(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int jj_d = jj + 1;
				if (jj_d > iHei - 1)
					jj_d = iHei - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj_d * iWid + kk] - pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量z向一阶后求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_z(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int ii_b = ii + 1;
				if (ii_b > iBNm - 1)
					ii_b = iBNm - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii_b][jj * iWid + kk] - pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量x向一阶左求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_x_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int kk_l = kk - 1;
				if (kk_l < 0)
					kk_l = 0;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj * iWid + kk] - pInputData[ii][jj * iWid + kk_l];
			}
		}
	}

	return;
}

//张量y向一阶上求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_y_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int jj_u = jj - 1;
				if (jj_u < 0)
					jj_u = 0;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj * iWid + kk] - pInputData[ii][jj_u * iWid + kk];
			}
		}
	}

	return;
}

//张量z向一阶前求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_z_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int ii_f = ii - 1;
				if (ii_f < 0)
					ii_f = 0;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj * iWid + kk] - pInputData[ii_f][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量x向二阶求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_xx(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int kk_l = kk - 1;
				if (kk_l < 0)
					kk_l = 0;

				int kk_r = kk + 1;
				if (kk_r > iWid - 1)
					kk_r = iWid - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj * iWid + kk_l] + pInputData[ii][jj * iWid + kk_r]
					- 2 * pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量y向二阶求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_yy(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int jj_u = jj - 1;
				if (jj_u < 0)
					jj_u = 0;

				int jj_d = jj + 1;
				if (jj_d > iHei - 1)
					jj_d = iHei - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii][jj_u * iWid + kk] + pInputData[ii][jj_d * iWid + kk]
					- pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}

//张量z向二阶求导函数
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::Grad_zz(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int ii_f = ii - 1;
				if (ii_f < 0)
					ii_f = 0;

				int ii_b = ii + 1;
				if (ii_b > iBNm - 1)
					ii_b = iBNm - 1;

				pOutputData[ii][jj * iWid + kk] = pInputData[ii_f][jj * iWid + kk] + pInputData[ii_b][jj * iWid + kk]
					- pInputData[ii][jj * iWid + kk];
			}
		}
	}

	return;
}




/*	Fold  &  unFold  操  作  */
/******************************************************************************************************************************************************************************************************************************************************/

//张量Mode-12 fold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::Fold_Mode_12(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = pInputData[ii][jj];
		}
	}
}

//张量Mode-13 fold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::Fold_Mode_13(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int i = (iHei - 1) - jj;
				int j = kk;
				int k = ii;

				double Temp = GetTensorData(pInputData, i, j, k, iHei, iWid, iBNm);

				SetTensorData(pOutputData, i, k, j, Temp, iHei, iBNm, iWid);
			}
		}
	}
}

//张量Mode-23 fold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::Fold_Mode_23(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int i = (iHei - 1) - jj;
				int j = kk;
				int k = ii;

				double Temp = GetTensorData(pInputData, i, j, k, iHei, iWid, iBNm);

				SetTensorData(pOutputData, k, i, j, Temp, iBNm, iHei, iWid);
			}
		}
	}
}

//张量Mode-12 unfold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::unFold_Mode_12(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = pInputData[ii][jj];
		}
	}
}

//张量Mode-13 unfold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::unFold_Mode_13(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int i = (iHei - 1) - jj;
				int j = kk;
				int k = ii;

				double Temp = GetTensorData(pInputData, i, j, k, iHei, iWid, iBNm);

				SetTensorData(pOutputData, i, k, j, Temp, iHei, iBNm, iWid);
			}
		}
	}
}

//张量Mode-23 unfold函数
//pInputData：输入张量； pOutputData：输出张量； iHei：输入张量的高； iWid：输入张量的宽； iBNm：输入张量的波段数量
void CATVDLRDenoiseDlg::unFold_Mode_23(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei; jj++)
		{
			for (int kk = 0; kk < iWid; kk++)
			{
				int i = (iHei - 1) - jj;
				int j = kk;
				int k = ii;

				double Temp = GetTensorData(pInputData, i, j, k, iHei, iWid, iBNm);

				SetTensorData(pOutputData, j, k, i, Temp, iWid, iBNm, iHei);
			}
		}
	}
}




/*	图	像	读	取	输	出	*/
/******************************************************************************************************************************************************************************************************************************************************/

//图像读取函数
//pData：读入数据； sTifPathOut：读入路径；
double** CATVDLRDenoiseDlg::ReadTifIn(CString sTifPathIn)
{
	double** pData;

	GDALDataset* pDatasetInput;
	pDatasetInput = (GDALDataset*)GDALOpen(sTifPathIn, GA_ReadOnly);
	if (!pDatasetInput)
	{
		AfxMessageBox("Can not open the image file!");
		return NULL;
	}

	iWidth = pDatasetInput->GetRasterXSize();	//宽度：400
	iHeight = pDatasetInput->GetRasterYSize();	//高度：200
	iBandNum = pDatasetInput->GetRasterCount();	//波段数量：30
	GDALDataType eDataType = pDatasetInput->GetRasterBand(1)->GetRasterDataType();	//GDT_Int16

	switch (eDataType)
	{
	case GDT_Byte:
	{
		BYTE** pInputData;
		pInputData = (BYTE**)malloc(iBandNum * sizeof(BYTE*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			BYTE* TempData;
			TempData = (BYTE*)malloc(iWidth * iHeight * sizeof(BYTE));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Int16:
	{
		short** pInputData;
		pInputData = (short**)malloc(iBandNum * sizeof(short*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			short* TempData;
			TempData = (short*)malloc(iWidth * iHeight * sizeof(short));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		 
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			GDALRasterBand* pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_UInt16:
	{
		unsigned short** pInputData;
		pInputData = (unsigned short**)malloc(iBandNum * sizeof(unsigned short*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			unsigned short* TempData;
			TempData = (unsigned short*)malloc(iWidth * iHeight * sizeof(unsigned short));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Int32:
	{
		int** pInputData;
		pInputData = (int**)malloc(iBandNum * sizeof(int*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			int* TempData;
			TempData = (int*)malloc(iWidth * iHeight * sizeof(int));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_UInt32:
	{
		unsigned long** pInputData;
		pInputData = (unsigned long**)malloc(iBandNum * sizeof(unsigned long*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			unsigned long* TempData;
			TempData = (unsigned long*)malloc(iWidth * iHeight * sizeof(unsigned long));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Float32:
	{
		float** pInputData;
		pInputData = (float**)malloc(iBandNum * sizeof(float*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			float* TempData;
			TempData = (float*)malloc(iWidth * iHeight * sizeof(float));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Float64:
	{
		double** pInputData;
		pInputData = (double**)malloc(iBandNum * sizeof(double*));
		for (int ii = 0; ii < iBandNum; ii++)
		{
			double* TempData;
			TempData = (double*)malloc(iWidth * iHeight * sizeof(double));
			for (int jj = 0; jj < iHeight * iWidth; jj++)
			{
				TempData[jj] = 0;
			}
			pInputData[ii] = TempData;
		}

		GDALRasterBand* pBandInput;
		for (int ii = 1; ii <= iBandNum; ii++)
		{
			pBandInput = pDatasetInput->GetRasterBand(ii);
			CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pInputData[ii - 1], iWidth, iHeight, eDataType, 0, 0);
			if (ReadErr == CE_Failure)	//如果读取失败，则返回
			{
				AfxMessageBox("读取图像错误");
				return NULL;
			}
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	default:
	{
		AfxMessageBox("图像数据类型错误，请检查图像");
		GDALClose(pDatasetInput);
		return NULL;
	}
	}

	GDALClose(pDatasetInput);

	return pData;
}

//图像输出函数
//pData：输出数据； sTifPathOut：输出路径； iHei：高； iWid：宽； iBNm：波段数量
void CATVDLRDenoiseDlg::WriteTifOut(double** pData, CString sTifPathOut, int iHei, int iWid, int iBNm)
{
	//创建输出图像
	GDALDataset* pDatasetOutput;
	GDALDriver* pDriver = NULL;
	pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	pDatasetOutput = pDriver->Create(sTifPathOut, iWid, iHei, iBNm, GDT_Float64, NULL);
	if (!pDatasetOutput)
	{
		AfxMessageBox("创建输出文件失败");
		return;
	}

	//将输出数据写入到输出图像中
	for (int ii = 1; ii <= iBNm; ii++)
	{
		GDALRasterBand* pBandOutput = pDatasetOutput->GetRasterBand(ii);
		CPLErr WriteErr = pBandOutput->RasterIO(GF_Write, 0, 0, iWid, iHei, pData[ii - 1], iWid, iHei, GDT_Float64, 0, 0);
		if (WriteErr == CE_Failure)	//如果输出失败，则返回
		{
			AfxMessageBox("输出图像错误");
			return;
		}
	}

	GDALClose(pDatasetOutput);

	return;
}




/*	内	存	管	理	*/
/******************************************************************************************************************************************************************************************************************************************************/

//申请内存函数
//调用MallocFunction函数为每个指针申请内存空间
void CATVDLRDenoiseDlg::MallocData()
{
	H = MallocFunction(iHeight, iWidth, iBandNum);
	X = MallocFunction(iHeight, iWidth, iBandNum);
	S = MallocFunction(iHeight, iWidth, iBandNum);
	B = MallocFunction(iHeight, iWidth, iBandNum);
	N = MallocFunction(iHeight, iWidth, iBandNum);

	H_X = MallocFunction(iHeight, iWidth, iBandNum);

	DhX = MallocFunction(iHeight, iWidth, iBandNum);
	DvB = MallocFunction(iHeight, iWidth, iBandNum);
	DsX = MallocFunction(iHeight, iWidth, iBandNum);

	X12 = MallocFunction(iHeight, iWidth, iBandNum);
	X13 = MallocFunction(iHeight, iBandNum, iWidth);
	X23 = MallocFunction(iWidth, iBandNum, iHeight);

	P = MallocFunction(iHeight, iWidth, iBandNum);
	Q = MallocFunction(iHeight, iWidth, iBandNum);
	T = MallocFunction(iHeight, iWidth, iBandNum);
	J = MallocFunction(iHeight, iWidth, iBandNum);

	DvT = MallocFunction(iHeight, iWidth, iBandNum);

	Z = MallocFunction(iHeight, iWidth, iBandNum);
	Z12f = MallocFunction(iHeight, iWidth, iBandNum);
	Z13f = MallocFunction(iHeight, iWidth, iBandNum);
	Z23f = MallocFunction(iHeight, iWidth, iBandNum);
	Z12uf = MallocFunction(iHeight, iWidth, iBandNum);
	Z13uf = MallocFunction(iHeight, iBandNum, iWidth);
	Z23uf = MallocFunction(iWidth, iBandNum, iHeight);

	A = MallocFunction(iHeight, iWidth, iBandNum);
	A12f = MallocFunction(iHeight, iWidth, iBandNum);
	A13f = MallocFunction(iHeight, iWidth, iBandNum);
	A23f = MallocFunction(iHeight, iWidth, iBandNum);
	A12uf = MallocFunction(iHeight, iWidth, iBandNum);
	A13uf = MallocFunction(iHeight, iBandNum, iWidth);
	A23uf = MallocFunction(iWidth, iBandNum, iHeight);

	Mp = MallocFunction(iHeight, iWidth, iBandNum);
	Mq = MallocFunction(iHeight, iWidth, iBandNum);
	Mb = MallocFunction(iHeight, iWidth, iBandNum);
	Mt = MallocFunction(iHeight, iWidth, iBandNum);
	Mh = MallocFunction(iHeight, iWidth, iBandNum);

	A12_μ = MallocFunction(iHeight, iWidth, iBandNum);
	A13_μ = MallocFunction(iHeight, iBandNum, iWidth);
	A23_μ = MallocFunction(iWidth, iBandNum, iHeight);

	X12_A12_μ = MallocFunction(iHeight, iWidth, iBandNum);
	X13_A13_μ = MallocFunction(iHeight, iBandNum, iWidth);
	X23_A23_μ = MallocFunction(iWidth, iBandNum, iHeight);

	DhtP = MallocFunction(iHeight, iWidth, iBandNum);
	DhtMp = MallocFunction(iHeight, iWidth, iBandNum);
	DstQ = MallocFunction(iHeight, iWidth, iBandNum);
	DstMq = MallocFunction(iHeight, iWidth, iBandNum);

	DvtJ = MallocFunction(iHeight, iWidth, iBandNum);
	DvtMt = MallocFunction(iHeight, iWidth, iBandNum);

	CX = MallocFunction(iHeight, iWidth, iBandNum);
	CT = MallocFunction(iHeight, iWidth, iBandNum);

	return;
}

//为单个指针申请内存空间函数
//pData：需要申请内存空间的指针； iHei：高； iWid：宽； iBNm：波段数量
double** CATVDLRDenoiseDlg::MallocFunction(int iHei, int iWid, int iBNm)
{
	double** pData = (double**)malloc(iBNm * sizeof(double*));
	for (int ii = 0; ii < iBNm; ii++)
	{
		double* TempData;
		TempData = (double*)malloc(iHei *iWid * sizeof(double));
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			TempData[jj] = 0;
		}
		pData[ii] = TempData;
	}

	return pData;
}

//释放内存函数
//调用DeleteFunction函数为每个指针释放内存空间
void CATVDLRDenoiseDlg::DeleteData()
{
	DeleteFunction(pHSI, iBandNum);

	DeleteFunction(H, iBandNum);
	DeleteFunction(X, iBandNum);
	DeleteFunction(S, iBandNum);
	DeleteFunction(B, iBandNum);
	DeleteFunction(N, iBandNum);

	DeleteFunction(H_X, iBandNum);

	DeleteFunction(DhX, iBandNum);
	DeleteFunction(DvB, iBandNum);
	DeleteFunction(DsX, iBandNum);

	DeleteFunction(X12, iBandNum);
	DeleteFunction(X13, iWidth);
	DeleteFunction(X23, iHeight);

	DeleteFunction(P, iBandNum);
	DeleteFunction(Q, iBandNum);
	DeleteFunction(T, iBandNum);
	DeleteFunction(J, iBandNum);

	DeleteFunction(DvT, iBandNum);

	DeleteFunction(Z, iBandNum);
	DeleteFunction(Z12f, iBandNum);
	DeleteFunction(Z13f, iBandNum);
	DeleteFunction(Z23f, iBandNum);
	DeleteFunction(Z12uf, iBandNum);
	DeleteFunction(Z13uf, iWidth);
	DeleteFunction(Z23uf, iHeight);

	DeleteFunction(A, iBandNum);
	DeleteFunction(A12f, iBandNum);
	DeleteFunction(A13f, iBandNum);
	DeleteFunction(A23f, iBandNum);
	DeleteFunction(A12uf, iBandNum);
	DeleteFunction(A13uf, iWidth);
	DeleteFunction(A23uf, iHeight);

	DeleteFunction(Mp, iBandNum);
	DeleteFunction(Mq, iBandNum);
	DeleteFunction(Mb, iBandNum);
	DeleteFunction(Mt, iBandNum);
	DeleteFunction(Mh, iBandNum);

	DeleteFunction(A12_μ, iBandNum);
	DeleteFunction(A13_μ, iWidth);
	DeleteFunction(A23_μ, iHeight);

	DeleteFunction(X12_A12_μ, iBandNum);
	DeleteFunction(X13_A13_μ, iWidth);
	DeleteFunction(X23_A23_μ, iHeight);

	DeleteFunction(DhtP, iBandNum);
	DeleteFunction(DhtMp, iBandNum);
	DeleteFunction(DstQ, iBandNum);
	DeleteFunction(DstMq, iBandNum);

	DeleteFunction(DvtJ, iBandNum);
	DeleteFunction(DvtMt, iBandNum);

	DeleteFunction(CX, iBandNum);
	DeleteFunction(CT, iBandNum);

	return;
}

//为单个指针释放内存空间函数
//pData：需要释放内存空间的指针； iBNm：波段数量
void CATVDLRDenoiseDlg::DeleteFunction(double** pData, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		free(pData[ii]);
		pData[ii] = NULL;
	}

	free(pData);
	pData = NULL;

	return;
}
