
// TVDenoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TVDenoise.h"
#include "TVDenoiseDlg.h"
#include "afxdialogex.h"

#include "FFT.h"

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


// CTVDenoiseDlg 对话框



CTVDenoiseDlg::CTVDenoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TVDENOISE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTVDenoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTVDenoiseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TV, &CTVDenoiseDlg::OnBnClickedButtonTv)
END_MESSAGE_MAP()


// CTVDenoiseDlg 消息处理程序

BOOL CTVDenoiseDlg::OnInitDialog()
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

	if (!FFTInitialize())
	{
		return TRUE;
	}



	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTVDenoiseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTVDenoiseDlg::OnPaint()
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
HCURSOR CTVDenoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTVDenoiseDlg::OnBnClickedButtonTv()
{
	/*程序开始，读取基本信息
	*********************************************************************************************************************************************************************************/

	//设置读取图像的路径和输出图像的路径
	CString sPathTifIn = "D:\\TestData\\TestImage\\Stripe_WP60_BP60_M0_R20_Pre_Band5.tif";
	//CString sPathTifIn = "D:\\TestData\\Result\\Test_NL_SN.tif";
	CString sPathTifOut = "D:\\TestData\\TestImage\\Out\\Stripe_WP60_BP60_M0_R20_Pre_Band5";
	//CString sPathTifOut = "D:\\TestData\\TVDenoise\\Result\\Test_NL_SN.tif";

	//以只读方式打开图像
	GDALDataset* pDatasetInput;
	pDatasetInput = (GDALDataset*)GDALOpen(sPathTifIn, GA_ReadOnly);
	if (!pDatasetInput)
	{
		AfxMessageBox("Can not open the image file!");
		return;
	}

	//读取影像的基本信息
	iWidth = pDatasetInput->GetRasterXSize();	//宽度：512
	iHeight = pDatasetInput->GetRasterYSize();	//高度：512
	iBandNum = pDatasetInput->GetRasterCount();	//波段数量：1
	GDALDataType eDataType = pDatasetInput->GetRasterBand(1)->GetRasterDataType();	//GDT_Byte

	/*开辟内存用来存放输入和输出数据
	*********************************************************************************************************************************************************************************/

	//开辟存放图像数据的储存空间
	//BYTE * pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (BYTE*)malloc(iHeight * iWidth * sizeof(BYTE));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	float* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	pDataInput = (float*)malloc(iHeight * iWidth * sizeof(float));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		pDataInput[ii] = NULL;
	}

	//int* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (int*)malloc(iHeight * iWidth * sizeof(int));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	/*将需要读取的图像读入内存
	********************************************************************************************************************************************************************************/

	//读取图像
	GDALRasterBand* pBandInput = pDatasetInput->GetRasterBand(1);

	CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pDataInput, iWidth, iHeight, eDataType, 0, 0);
	if (ReadErr == CE_Failure)	//如果读取失败，则返回
	{
		AfxMessageBox("读取图像错误");
		return;
	}

	/*创立Bregman迭代所需的一些变量空间
	********************************************************************************************************************************************************************************/
	
	f = NULL;	//f
	f = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		f[ii] = NULL;
	}

	nei = NULL;	//uk+1
	nei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		nei[ii] = NULL;
	}

	uk = NULL;	//uk
	uk = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		uk[ii] = NULL;
	}

	uk1 = NULL;	//uk+1
	uk1 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		uk1[ii] = NULL;
	}
	
	uk1_uk = NULL;	//uk+1
	uk1_uk = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		uk1_uk[ii] = NULL;
	}

	dx = NULL;	//dx
	dx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		dx[ii] = NULL;
	}

	dy = NULL;	//dy
	dy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		dy[ii] = NULL;
	}

	bx = NULL;	//bx
	bx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		bx[ii] = NULL;
	}

	by = NULL;	//by
	by = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		by[ii] = NULL;
	}

	uk_x = NULL;	//uk_x
	uk_x = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		uk_x[ii] = NULL;
	}

	uk_y = NULL;	//uk_y
	uk_y = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		uk_y[ii] = NULL;
	}

	ddx = NULL;	//ddx
	ddx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		ddx[ii] = NULL;
	}

	ddy = NULL;	//ddy
	ddy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		ddy[ii] = NULL;
	}

	dbx = NULL;	//dbx
	dbx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		dbx[ii] = NULL;
	}

	dby = NULL;	//dby
	dby = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		dby[ii] = NULL;
	}


	/*初始化所有变量
	********************************************************************************************************************************************************************************/

	Copy(pDataInput, uk1);
	Copy(pDataInput, f);
	WriteTIfOut(f, sPathTifOut + "_f.tif");
	lambd = 100;
	mu = 0.1;
	tol = 0.001;
	iter = 20;

	for (int ii = 0; ii < iter; ii++)
	{
		Subtract(uk1, uk, uk1_uk);
		double Temptol = Norm(uk1_uk) / Norm(uk1);
		if (Temptol < tol)
			break;

		Copy(uk1, uk);
		FFT_method();
		Grad_x(uk1, uk_x);
		Grad_y(uk1, uk_y);
		Shrink_x(uk_x, bx, lambd, dx);
		Shrink_y(uk_y, by, lambd, dy);
		Bx_iter(uk_x, dx, bx);
		Bx_iter(uk_y, dy, by);

		CString sii;
		sii.Format("%d", ii);
		WriteTIfOut(uk_x, sPathTifOut + "_uk_x_" + sii + ".tif");
		WriteTIfOut(uk_y, sPathTifOut + "_uk_y_" + sii + ".tif");
		WriteTIfOut(dx, sPathTifOut + "_dx_" + sii + ".tif");
		WriteTIfOut(dy, sPathTifOut + "_dy_" + sii + ".tif");
		WriteTIfOut(bx, sPathTifOut + "_bx_" + sii + ".tif");
		WriteTIfOut(by, sPathTifOut + "_by_" + sii + ".tif");
		WriteTIfOut(uk1, sPathTifOut + "_uk1_" + sii + ".tif");
	}

	AfxMessageBox("Done");
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CTVDenoiseDlg::Copy(BYTE* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CTVDenoiseDlg::Copy(int* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CTVDenoiseDlg::Copy(float* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CTVDenoiseDlg::Copy(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//范数
//范数求法：各项平方和相加然后开平方
//输入：pInputData：需要求范数的数据
//输出：dResult：范数数值
double CTVDenoiseDlg::Norm(double* pInputData)
{
	double dResult = 0;
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		dResult += pInputData[ii] * pInputData[ii];
	}
	dResult = sqrt(dResult);
	return dResult;
}

//相减函数
//输入：pInputData1：被减数；pInputData2：减数；pOutputData：减法结果
void CTVDenoiseDlg::Subtract(double* pInputData1, double* pInputData2, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = pInputData1[ii] - pInputData2[ii];
	}
}

//横向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CTVDenoiseDlg::Grad_x(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth - 1; jj++)
		{
			pOutputData[ii * iWidth + jj] = pInputData[ii * iWidth + (jj + 1)] - pInputData[ii * iWidth + jj];
		}
	}
	for (int ii = 0; ii < iHeight; ii++)
	{
		pOutputData[ii * iWidth + (iWidth - 1)] = 0.0;
	}
}

//纵向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CTVDenoiseDlg::Grad_y(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight - 1; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			pOutputData[ii * iWidth + jj] = pInputData[(ii + 1) * iWidth + jj] - pInputData[ii * iWidth + jj];
		}
	}
	for (int jj = 0; jj < iWidth; jj++)
	{
		pOutputData[(iHeight - 1) * iWidth + jj] = 0.0;
	}
}

void CTVDenoiseDlg::Grad_x_t(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_l = ii;
			int jj_l = jj - 1;
			if (jj_l < 0)
				jj_l = 0;
			pOutputData[ii * iWidth + jj] = pInputData[ii * iWidth + jj] - pInputData[ii_l * iWidth + jj_l];
		}
	}
}

void CTVDenoiseDlg::Grad_y_t(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_u = ii - 1;
			int jj_u = jj;
			if (ii_u < 0)
				ii_u = 0;
			pOutputData[ii * iWidth + jj] = pInputData[ii * iWidth + jj] - pInputData[ii_u * iWidth + jj_u];
		}
	}
}

void CTVDenoiseDlg::FFT_method()
{
	mwSize mdim[2] = { iHeight, iWidth };
	mwArray mwf(2, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwdx(2, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwdy(2, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwbx(2, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwby(2, mdim, mxDOUBLE_CLASS, mxREAL);
	mwArray mwOutputData(2, mdim, mxDOUBLE_CLASS, mxREAL);

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			mwf(ii + 1, jj + 1) = f[ii * iWidth + jj];
			mwdx(ii + 1, jj + 1) = dx[ii * iWidth + jj];
			mwdy(ii + 1, jj + 1) = dy[ii * iWidth + jj];
			mwbx(ii + 1, jj + 1) = bx[ii * iWidth + jj];
			mwby(ii + 1, jj + 1) = by[ii * iWidth + jj];
		}
	}

	mwArray mwMu(1, 1, mxDOUBLE_CLASS, mxREAL);
	mwMu(1) = mu;
	mwArray mwLambda(1, 1, mxDOUBLE_CLASS, mxREAL);
	mwLambda(1) = lambd;

	FFT(1, mwOutputData, mwMu, mwLambda, mwf, mwdx, mwbx, mwdy, mwby);

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			uk1[ii * iWidth + jj] = mwOutputData(ii + 1, jj + 1);
		}
	}
}

//求邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CTVDenoiseDlg::Neighbors(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_left = ii;
			int jj_left = jj - 1;
			if (jj_left < 0)
				jj_left = iWidth - 1;

			int ii_right = ii;
			int jj_right = jj + 1;
			if (jj_right == iWidth)
				jj_right = 0;

			int ii_up = ii + 1;
			int jj_up = jj;
			if (ii_up == iHeight)
				ii_up = 0;

			int ii_down = ii - 1;
			int jj_down = jj;
			if (ii_down < 0)
				ii_down = iHeight - 1;

			pOutputData[ii * iWidth + jj] =
				pInputData[ii_left * iWidth + jj_left] +
				pInputData[ii_right * iWidth + jj_right] +
				pInputData[ii_up * iWidth + jj_up] +
				pInputData[ii_down * iWidth + jj_down];
		}
	}
}

//G-S迭代
//输入：各个参数
void CTVDenoiseDlg::Gaussian_method(double* uk, double* uk1, double* f, double* dx, double* dy, double* bx, double* by, double lambd, double mu)
{
	double mul1 = lambd / (mu + 4 * lambd);
	double mul2 = mu / (mu + 4 * lambd);

	Grad_x_t(dx, ddx);
	Grad_y_t(dy, ddy);
	Grad_x_t(bx, dbx);
	Grad_y_t(by, dby);
	Neighbors(uk, nei);

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			uk1[ii * iWidth + jj] = 
				mul1 * (
					nei[ii * iWidth + jj] +
					ddx[ii * iWidth + jj] +
					ddy[ii * iWidth + jj] -
					dbx[ii * iWidth + jj] -
					dby[ii * iWidth + jj]
					) +
				mul2 * f[ii * iWidth + jj];
		}
	}
}

//Shrink算子
//输入：uk1、b、lambd
void CTVDenoiseDlg::Shrink_x(double* uk_x, double* bx, double lambd, double* dx)
{
	double thr = 1.0 / lambd;

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = uk_x[ii * iWidth + jj] + bx[ii * iWidth + jj];
			if (Temp > thr)
				dx[ii * iWidth + jj] = Temp - thr;
			else if (Temp < 0 - thr)
				dx[ii * iWidth + jj] = Temp + thr;
			else
				dx[ii * iWidth + jj] = 0;
		}
	}
}

//Shrink算子
//输入：uk1、b、lambd
void CTVDenoiseDlg::Shrink_y(double* uk_y, double* by, double lambd, double* dy)
{
	double thr = 1.0 / lambd;
	
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = uk_y[ii * iWidth + jj] + by[ii * iWidth + jj];
			if (Temp > thr)
				dy[ii * iWidth + jj] = Temp - thr;
			else if (Temp < 0 - thr)
				dy[ii * iWidth + jj] = Temp + thr;
			else
				dy[ii * iWidth + jj] = 0;
		}
	}
}

//bx迭代函数
//输入：uk_x：uk1的横向偏导；dx、bx：对应的变量
void CTVDenoiseDlg::Bx_iter(double* uk_x, double* dx, double* bx)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			bx[ii * iWidth + jj] = bx[ii * iWidth + jj] + uk_x[ii * iWidth + jj] - dx[ii * iWidth + jj];
		}
	}
}

//by迭代函数
//输入：uk_y：uk1的横向偏导；dy、by：对应的变量
void CTVDenoiseDlg::By_iter(double* uk_y, double* dy, double* by)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			by[ii * iWidth + jj] = by[ii * iWidth + jj] + uk_y[ii * iWidth + jj] - dy[ii * iWidth + jj];
		}
	}
}

//输出图像函数
//输入：pData：需要输出的数据；sTifPathOut：输出路径
void CTVDenoiseDlg::WriteTIfOut(double* pData, CString sTifPathOut)
{
	//创建输出图像
	GDALDataset* pDatasetOutput;
	GDALDriver* pDriver = NULL;
	pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	pDatasetOutput = pDriver->Create(sTifPathOut, iWidth, iHeight, 1, GDT_Float64, NULL);
	if (!pDatasetOutput)
	{
		AfxMessageBox("创建输出文件失败");
		return;
	}

	//将输出数据写入到输出图像中
	GDALRasterBand* pBandOutput = pDatasetOutput->GetRasterBand(1);
	CPLErr WriteErr = pBandOutput->RasterIO(GF_Write, 0, 0, iWidth, iHeight, pData, iWidth, iHeight, GDT_Float64, 0, 0);
	if (WriteErr == CE_Failure)	//如果输出失败，则返回
	{
		AfxMessageBox("输出图像错误");
		return;
	}

	GDALClose(pDatasetOutput);

	return;
}