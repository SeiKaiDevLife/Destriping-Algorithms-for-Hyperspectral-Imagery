
// DTVDenoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DTVDenoise.h"
#include "DTVDenoiseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.1415926535


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


// CDTVDenoiseDlg 对话框



CDTVDenoiseDlg::CDTVDenoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DTVDENOISE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDTVDenoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDTVDenoiseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DTV, &CDTVDenoiseDlg::OnBnClickedButtonDtv)
END_MESSAGE_MAP()


// CDTVDenoiseDlg 消息处理程序

BOOL CDTVDenoiseDlg::OnInitDialog()
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


	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDTVDenoiseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDTVDenoiseDlg::OnPaint()
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
HCURSOR CDTVDenoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDTVDenoiseDlg::OnBnClickedButtonDtv()
{
	/*程序开始，读取基本信息
	*********************************************************************************************************************************************************************************/

	//设置读取图像的路径和输出图像的路径
	CString sPathTifIn = "D:\\TestData\\DTVDenoise\\DTV_IN\\134tif.tif";
	//CString sPathTifIn = "D:\\TestData\\Result\\Test_NL_SN.tif";
	CString sPathTifOut = "D:\\TestData\\DTVDenoise\\DTV_OUT\\134tif_";
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
	//BYTE* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (BYTE*)malloc(iHeight * iWidth * sizeof(BYTE));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	short* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	pDataInput = (short*)malloc(iWidth * iHeight * sizeof(short));
	for (int ii = 0; ii < iWidth * iHeight; ii++)	//初始化
	{
		pDataInput[ii] = 0;
	}

	//int* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (int*)malloc(iHeight * iWidth * sizeof(int));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	//float** pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (float**)malloc(iBandNum * sizeof(float*));
	//for (int ii = 0; ii < iBandNum; ii++)	//初始化
	//{
	//	float* pTemp = NULL;
	//	pTemp = (float*)malloc(iHeight * iWidth * sizeof(float));
	//	for (int jj = 0; jj < iHeight * iWidth; jj++)
	//	{
	//		pTemp[jj] = 0;
	//	}
	//	pDataInput[ii] = pTemp;
	//}

	//double** pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (double**)malloc(iBandNum * sizeof(double*));
	//for (int ii = 0; ii < iBandNum; ii++)	//初始化
	//{
	//	double* pTemp = NULL;
	//	pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
	//	for (int jj = 0; jj < iHeight * iWidth; jj++)
	//	{
	//		pTemp[jj] = 0;
	//	}
	//	pDataInput[ii] = pTemp;
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

	/*创立迭代所需的一些变量空间
	********************************************************************************************************************************************************************************/

	MallocData();

	/*主程序部分
	********************************************************************************************************************************************************************************/

	Copy(pDataInput, f);
	Copy(pDataInput, p);
	Copy(pDataInput, I);
	WriteTIfOut(p, "D:\\TestData\\DTVDenoise\\DTV_OUT\\p.tif");
	GuidedFilter();
	Subtract(p, q, q);
	WriteTIfOut(q, "D:\\TestData\\DTVDenoise\\DTV_OUT\\q.tif");

	fre_spectrum(q, q_FFT);

	WriteTIfOut(q_FFT, "D:\\TestData\\DTVDenoise\\DTV_OUT\\q_FFT.tif");

	Max_Fre_Dre_Cal();

	Copy(pDataInput, f);

	Grad_x(f, fx);
	Grad_y(f, fy);
	Grad_xx(f, fxx);
	Grad_yy(f, fyy);
	Grad_θθ(f, fθθ);

	lambd = 15;
	iter = 20;

	ρ1 = 13;
	ρ2 = 20;
	ρ3 = 3;
	ρ4 = 12;

	λ1 = 0.2;
	λ2 = 0.4;
	λ3 = 0.1;


	for (int ii = 0; ii < 20; ii++)
	{
		Grad_x(Sk, Skx);
		Grad_y(Sk, Sky);
		Grad_θ(Sk, Skθ);

		Grad_xx(Sk, Skxx);
		Grad_yy(Sk, Skyy);
		Grad_θθ(Sk, Skθθ);

		Grad_θ(J, Jθ);
		Grad_θ(p1, p1θ);
		Grad_y(W, Wy);
		Grad_y(p3, p3y);
		Grad_x(V, Vx);
		Grad_x(p4, p4x);

		for (int jj = 0; jj < 100; jj++)
		{
			Gaussian_method();		//Sk -> Sk1
			Copy(Sk1, Sk);
		}

		Shrink_J(Skθ, p1, J);
		Shrink_H(Sk, p2, H);
		Shrink_W(Sky, p3, W);
		Shrink_V(Skx, p4, V);

		P1_iter(p1, Skθ, J);
		P2_iter(p2, Sk, H);
		P3_iter(p3, Sky, W);
		P4_iter(p4, Skx, V);

		Subtract(f, Sk, u);

		CString sii;
		sii.Format("%d", ii);
		CString Path = sPathTifOut + sii + ".tif";
		WriteTIfOut(Sk, "D:\\TestData\\DTVDenoise\\DTV_OUT\\Sk.tif");
		WriteTIfOut(u, Path);

		continue;
	}

	/*释放空间并结束程序
	********************************************************************************************************************************************************************************/

	DeleteData();

	AfxMessageBox("Done");

}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CDTVDenoiseDlg::Copy(BYTE* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CDTVDenoiseDlg::Copy(short* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CDTVDenoiseDlg::Copy(int* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CDTVDenoiseDlg::Copy(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//相减函数
//输入：pInputData1：被减数；pInputData2：减数；pOutputData：减法结果
void CDTVDenoiseDlg::Subtract(double* pInputData1, double* pInputData2, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = pInputData1[ii] - pInputData2[ii];
	}
}

//相加函数
//输入：pInputData1：加数；pInputData2：加数；pOutputData：加法结果
void CDTVDenoiseDlg::Add(double* pInputData1, double* pInputData2, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = pInputData1[ii] + pInputData2[ii];
	}
}

//求U邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CDTVDenoiseDlg::NeighborsU(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_lu = ii - 1;
			int jj_lu = jj - 2;
			if (ii_lu < 0)
				ii_lu = 0;
			if (jj_lu < 0)
				jj_lu = 0;

			int ii_ld = ii + 2;
			int jj_ld = jj - 1;
			if (ii_ld > iHeight - 1)
				ii_ld = iHeight - 1;
			if (jj_ld < 0)
				jj_ld = 0;

			int ii_ru = ii - 2;
			int jj_ru = jj + 1;
			if (ii_ru < 0)
				ii_ru = 0;
			if (jj_ru > iWidth - 1)
				jj_ru = iWidth - 1;

			int ii_rd = ii + 1;
			int jj_rd = jj + 2;
			if (ii_rd > iHeight - 1)
				ii_rd = iHeight - 1;
			if (jj_rd > iWidth - 1)
				jj_rd = iWidth - 1;

			pOutputData[ii * iWidth + jj] =
				pInputData[ii_ld * iWidth + jj_ld] +
				pInputData[ii_ru * iWidth + jj_ru] +
				lambd * pInputData[ii_lu * iWidth + jj_lu] +
				lambd * pInputData[ii_rd * iWidth + jj_rd];
		}
	}
}

//求V邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CDTVDenoiseDlg::NeighborsV(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_lu = ii - 1;
			int jj_lu = jj - 2;
			if (ii_lu < 0)
				ii_lu = 0;
			if (jj_lu < 0)
				jj_lu = 0;

			int ii_ld = ii + 2;
			int jj_ld = jj - 1;
			if (ii_ld > iHeight - 1)
				ii_ld = iHeight - 1;
			if (jj_ld < 0)
				jj_ld = 0;

			int ii_ru = ii - 2;
			int jj_ru = jj + 1;
			if (ii_ru < 0)
				ii_ru = 0;
			if (jj_ru > iWidth - 1)
				jj_ru = iWidth - 1;

			int ii_rd = ii + 1;
			int jj_rd = jj + 2;
			if (ii_rd > iHeight - 1)
				ii_rd = iHeight - 1;
			if (jj_rd > iWidth - 1)
				jj_rd = iWidth - 1;

			pOutputData[ii * iWidth + jj] =
				pInputData[ii * iWidth + jj] * 2 -
				pInputData[ii_ld * iWidth + jj_ld] -
				pInputData[ii_ru * iWidth + jj_ru];
		}
	}
}

//求U邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CDTVDenoiseDlg::Neighbors(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_r = ii;
			int jj_r = jj + 1;
			if (jj_r > iWidth - 1)
				jj_r = iWidth - 1;

			int ii_l = ii;
			int jj_l = jj - 1;
			if (jj_l < 0)
				jj_l = 0;

			int ii_u = ii - 1;
			int jj_u = jj;
			if (ii_u < 0)
				ii_u = 0;

			int ii_d = ii + 1;
			int jj_d = jj;
			if (ii_d > iHeight - 1)
				ii_d = iHeight - 1;

			int ii_ru = ii - 2;
			int jj_ru = jj + 1;
			if (ii_ru < 0)
				ii_ru = 0;
			if (jj_ru > iWidth - 1)
				jj_ru = iWidth - 1;

			int ii_ld = ii + 2;
			int jj_ld = jj - 1;
			if (ii_ld > iHeight - 1)
				ii_ld = iHeight - 1;
			if (jj_ld < 0)
				jj_ld = 0;

			pOutputData[ii * iWidth + jj] = 0 +
				ρ1 * (pInputData[ii_ru * iWidth + jj_ru] + pInputData[ii_ld * iWidth + jj_ld]) +
				ρ3 * (pInputData[ii_u * iWidth + jj_u] + pInputData[ii_d * iWidth + jj_d]) + 
				ρ4 * (pInputData[ii_r * iWidth + jj_r] + pInputData[ii_l * iWidth + jj_l]);
		}
	}
}

//求f邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CDTVDenoiseDlg::Neighborsf(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			pOutputData[ii * iWidth + jj] = 0 - 
				ρ3 * fyy[ii * iWidth + jj] - 
				ρ4 * fxx[ii * iWidth + jj];
		}
	}
}

//G-S迭代
void CDTVDenoiseDlg::Gaussian_method()  
{
	Neighbors(Sk, SkNei);
	Neighbors(f, fNei);

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = (ρ2 + 2 * ρ1 + 2 * ρ3 + 2 * ρ4);
			Sk1[ii * iWidth + jj] =
				(
					SkNei[ii * iWidth + jj]
					 - ρ1 * Jθ[ii * iWidth + jj] + p1θ[ii * iWidth + jj]
					 + ρ2 * H[ii * iWidth + jj] - p2[ii * iWidth + jj]
					 - ρ3 * fyy[ii * iWidth + jj] - ρ3 * Wy[ii * iWidth + jj] + p3y[ii * iWidth + jj]
					 - ρ4 * fxx[ii * iWidth + jj] - ρ4 * Vx[ii * iWidth + jj] + p4x[ii * iWidth + jj]
					) / Temp;
		}
	}
}

//快速傅里叶变换
void CDTVDenoiseDlg::fre_spectrum(double* pInputData, double* pOutputData)
{
	double re, im, temp;
	int move;

	for (int i = 0; i < iHeight; i++) 
	{
		for (int j = 0; j < iWidth; j++) 
		{
			re = 0;
			im = 0;

			for (int x = 0; x < iHeight; x++) 
			{
				for (int y = 0; y < iWidth; y++) 
				{
					temp = (double)i * x / (double)iHeight +
						(double)j * y / (double)iWidth;
					move = (x + y) % 2 == 0 ? 1 : -1;

					re += pInputData[x * iWidth + y] * cos(-2 * PI * temp) * move;
					im += pInputData[x * iWidth + y] * sin(-2 * PI * temp) * move;
				}
			}

			pOutputData[i * iWidth + j] = (sqrt(re * re + im * im) / sqrt(iWidth * iHeight));
			if (pOutputData[i * iWidth + j] > 0xff)
				pOutputData[i * iWidth + j] = 0xff;
			else if (pOutputData[i * iWidth + j] < 0)
				pOutputData[i * iWidth + j] = 0;
		}
	}
}

//最大频率方向求解函数
void CDTVDenoiseDlg::Max_Fre_Dre_Cal()
{
	double Max = q_FFT[0];
	int iiMax = 0;
	int jjMax = 0;
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			if (q_FFT[ii * iWidth + jj] > Max)
			{
				Max = q_FFT[ii * iWidth + jj];
				iiMax = ii;
				jjMax = jj;
			}
		}
	}
	//75.963
	StripeDegree = 90 - atan((iiMax - (iHeight - 1) / 2) / (double)(jjMax - (iWidth - 1) / 2)) / PI * 180;
	return;
}

//导向滤波
void CDTVDenoiseDlg::GuidedFilter()
{
	I_nei_Cal(3);
	p_nei_Cal(3);
	I2_nei_Cal(3);
	pI_nei_Cal(3);

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double ak = (pInei[ii * iWidth + jj] - pnei[ii * iWidth + jj] * Inei[ii * iWidth + jj]) /
				(I2nei[ii * iWidth + jj] - Inei[ii * iWidth + jj] * Inei[ii * iWidth + jj] + 10);
			double bk = pnei[ii * iWidth + jj] - ak * Inei[ii * iWidth + jj];
			q[ii * iWidth + jj] = ak * I[ii * iWidth + jj] + bk;
		}
	}
}

//pnei求解函数
void CDTVDenoiseDlg::p_nei_Cal(int iRange)
{
	int** Filter;
	Filter = (int**)malloc(iRange* iRange * sizeof(int*));
	for (int ii = 0; ii < iRange * iRange; ii++)
	{
		int* Temp;
		Temp = (int*)malloc(2 * sizeof(int));
		for (int jj = 0; jj < 2; jj++)
		{
			Temp[jj] = 0;
		}
		Filter[ii] = Temp;
	}

	int TempRange = (iRange - 1) / 2;

	for (int ii = 0 - TempRange; ii <= TempRange; ii++)
	{
		for (int jj = 0 - TempRange; jj <= TempRange; jj++)
		{
			int* Temp = Filter[(ii + TempRange) * iRange + (jj + TempRange)];
			Temp[0] = ii;
			Temp[1] = jj;
		}
	}

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			for (int mm = 0; mm < iRange; mm++)
			{
				for (int nn = 0; nn < iRange; nn++)
				{
					int* FilterTemp = Filter[mm * iRange + nn];
					int iiTemp = ii + FilterTemp[0];
					int jjTemp = jj + FilterTemp[1];

					if (iiTemp < 0)
						iiTemp = 0;
					if (iiTemp > iHeight - 1)
						iiTemp = iHeight - 1;

					if (jjTemp < 0)
						jjTemp = 0;
					if (jjTemp > iWidth - 1)
						jjTemp = iWidth - 1;

					pnei[ii * iWidth + jj] = pnei[ii * iWidth + jj] + p[iiTemp * iWidth + jjTemp];
				}
			}

			pnei[ii * iWidth + jj] = pnei[ii * iWidth + jj] / (iRange * iRange);
		}
	}
}

//Inei求解函数
void CDTVDenoiseDlg::I_nei_Cal(int iRange)
{
	int** Filter;
	Filter = (int**)malloc(iRange * iRange * sizeof(int*));
	for (int ii = 0; ii < iRange * iRange; ii++)
	{
		int* Temp;
		Temp = (int*)malloc(2 * sizeof(int));
		for (int jj = 0; jj < 2; jj++)
		{
			Temp[jj] = 0;
		}
		Filter[ii] = Temp;
	}

	int TempRange = (iRange - 1) / 2;

	for (int ii = 0 - TempRange; ii <= TempRange; ii++)
	{
		for (int jj = 0 - TempRange; jj <= TempRange; jj++)
		{
			int* Temp = Filter[(ii + TempRange) * iRange + (jj + TempRange)];
			Temp[0] = ii;
			Temp[1] = jj;
		}
	}

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			for (int mm = 0; mm < iRange; mm++)
			{
				for (int nn = 0; nn < iRange; nn++)
				{
					int* FilterTemp = Filter[mm * iRange + nn];
					int iiTemp = ii + FilterTemp[0];
					int jjTemp = jj + FilterTemp[1];

					if (iiTemp < 0)
						iiTemp = 0;
					if (iiTemp > iHeight - 1)
						iiTemp = iHeight - 1;

					if (jjTemp < 0)
						jjTemp = 0;
					if (jjTemp > iWidth - 1)
						jjTemp = iWidth - 1;

					Inei[ii * iWidth + jj] = Inei[ii * iWidth + jj] + I[iiTemp * iWidth + jjTemp];
				}
			}

			Inei[ii * iWidth + jj] = Inei[ii * iWidth + jj] / (iRange * iRange);
		}
	}
}

//I2nei求解函数
void CDTVDenoiseDlg::I2_nei_Cal(int iRange)
{
	int** Filter;
	Filter = (int**)malloc(iRange * iRange * sizeof(int*));
	for (int ii = 0; ii < iRange * iRange; ii++)
	{
		int* Temp;
		Temp = (int*)malloc(2 * sizeof(int));
		for (int jj = 0; jj < 2; jj++)
		{
			Temp[jj] = 0;
		}
		Filter[ii] = Temp;
	}

	int TempRange = (iRange - 1) / 2;

	for (int ii = 0 - TempRange; ii <= TempRange; ii++)
	{
		for (int jj = 0 - TempRange; jj <= TempRange; jj++)
		{
			int* Temp = Filter[(ii + TempRange) * iRange + (jj + TempRange)];
			Temp[0] = ii;
			Temp[1] = jj;
		}
	}

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			for (int mm = 0; mm < iRange; mm++)
			{
				for (int nn = 0; nn < iRange; nn++)
				{
					int* FilterTemp = Filter[mm * iRange + nn];
					int iiTemp = ii + FilterTemp[0];
					int jjTemp = jj + FilterTemp[1];

					if (iiTemp < 0)
						iiTemp = 0;
					if (iiTemp > iHeight - 1)
						iiTemp = iHeight - 1;

					if (jjTemp < 0)
						jjTemp = 0;
					if (jjTemp > iWidth - 1)
						jjTemp = iWidth - 1;

					I2nei[ii * iWidth + jj] = I2nei[ii * iWidth + jj] + I[iiTemp * iWidth + jjTemp] * I[iiTemp * iWidth + jjTemp];
				}
			}

			I2nei[ii * iWidth + jj] = I2nei[ii * iWidth + jj] / (iRange * iRange);
		}
	}
}

//pInei求解函数
void CDTVDenoiseDlg::pI_nei_Cal(int iRange)
{
	int** Filter;
	Filter = (int**)malloc(iRange * iRange * sizeof(int*));
	for (int ii = 0; ii < iRange * iRange; ii++)
	{
		int* Temp;
		Temp = (int*)malloc(2 * sizeof(int));
		for (int jj = 0; jj < 2; jj++)
		{
			Temp[jj] = 0;
		}
		Filter[ii] = Temp;
	}

	int TempRange = (iRange - 1) / 2;

	for (int ii = 0 - TempRange; ii <= TempRange; ii++)
	{
		for (int jj = 0 - TempRange; jj <= TempRange; jj++)
		{
			int* Temp = Filter[(ii + TempRange) * iRange + (jj + TempRange)];
			Temp[0] = ii;
			Temp[1] = jj;
		}
	}

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			for (int mm = 0; mm < iRange; mm++)
			{
				for (int nn = 0; nn < iRange; nn++)
				{
					int* FilterTemp = Filter[mm * iRange + nn];
					int iiTemp = ii + FilterTemp[0];
					int jjTemp = jj + FilterTemp[1];

					if (iiTemp < 0)
						iiTemp = 0;
					if (iiTemp > iHeight - 1)
						iiTemp = iHeight - 1;

					if (jjTemp < 0)
						jjTemp = 0;
					if (jjTemp > iWidth - 1)
						jjTemp = iWidth - 1;

					pInei[ii * iWidth + jj] = pInei[ii * iWidth + jj] + p[iiTemp * iWidth + jjTemp] * I[iiTemp * iWidth + jjTemp];
				}
			}

			pInei[ii * iWidth + jj] = pInei[ii * iWidth + jj] / (iRange * iRange);
		}
	}
}

//横向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CDTVDenoiseDlg::Grad_x(double* pInputData, double* pOutputData)
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
void CDTVDenoiseDlg::Grad_y(double* pInputData, double* pOutputData)
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

//θ向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CDTVDenoiseDlg::Grad_θ(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int Tempii = ii - 2;
			int Tempjj = jj + 1;

			if (Tempii < 0)
			{
				Tempii = 0;
			}

			if (Tempjj > iWidth - 1)
			{
				Tempjj = iWidth - 1;
			}
			
			pOutputData[ii * iWidth + jj] = pInputData[Tempii * iWidth + Tempjj] - pInputData[ii * iWidth + jj];
		}
	}
}

//x向二重偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CDTVDenoiseDlg::Grad_xx(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int Tempiir = ii;
			int Tempjjr = jj + 1;

			int Tempiil = ii;
			int Tempjjl = jj - 1;

			if (Tempjjr > iWidth - 1)
			{
				Tempjjr = iWidth - 1;
			}

			if (Tempjjl < 0)
			{
				Tempjjl = 0;
			}

			pOutputData[ii * iWidth + jj] = pInputData[Tempiir * iWidth + Tempjjr] + pInputData[Tempiil * iWidth + Tempjjl] - 2 * pInputData[ii * iWidth + jj];
		}
	}
}

//y向二重偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CDTVDenoiseDlg::Grad_yy(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int Tempiiu = ii - 1;
			int Tempjju = jj;

			int Tempiid = ii + 1;
			int Tempjjd = jj;

			if (Tempiiu < 0)
			{
				Tempiiu = 0;
			}

			if (Tempiid > iHeight - 1)
			{
				Tempiid = iWidth - 1;
			}

			pOutputData[ii * iWidth + jj] = pInputData[Tempiiu * iWidth + Tempjju] + pInputData[Tempiid * iWidth + Tempjjd] - 2 * pInputData[ii * iWidth + jj];
		}
	}
}

//θ向二重偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CDTVDenoiseDlg::Grad_θθ(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int Tempiiru = ii - 2;
			int Tempjjru = jj + 1;

			int Tempiild = ii + 2;
			int Tempjjld = jj - 1;

			if (Tempiiru < 0)
			{
				Tempiiru = 0;
			}

			if (Tempjjru > iWidth - 1)
			{
				Tempjjru = iWidth - 1;
			}

			if (Tempiild > iHeight - 1)
			{
				Tempiild = iWidth - 1;
			}

			if (Tempjjld < 0)
			{
				Tempjjld = 0;
			}

			pOutputData[ii * iWidth + jj] = pInputData[Tempiiru * iWidth + Tempjjru] + pInputData[Tempiild * iWidth + Tempjjld] - 2 * pInputData[ii * iWidth + jj];
		}
	}
}

//Shrink算子求J函数
void CDTVDenoiseDlg::Shrink_J(double* Skθ, double* p1, double* J)
{
	double thr = 1 / ρ1;

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = Skθ[ii * iWidth + jj] + p1[ii * iWidth + jj] / ρ1;
			if(Temp > thr)
				J[ii * iWidth + jj] = Temp - thr;
			else if(Temp < 0 - thr)
				J[ii * iWidth + jj] = Temp + thr;
			else
				J[ii * iWidth + jj] = 0;
		}
	}
}

//Shrink算子求H函数
void CDTVDenoiseDlg::Shrink_H(double* Sk, double* p2, double* H)
{
	double thr = λ1 / ρ2;

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = Sk[ii * iWidth + jj] + p2[ii * iWidth + jj] / ρ2;
			if (Temp > thr)
				H[ii * iWidth + jj] = Temp - thr;
			else if (Temp < 0 - thr)
				H[ii * iWidth + jj] = Temp + thr;
			else
				H[ii * iWidth + jj] = 0;
		}
	}
}

//Shrink算子求W函数
void CDTVDenoiseDlg::Shrink_W(double* Sky, double* p3, double* W)
{
	double thr = λ2 / ρ3;

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = Sky[ii * iWidth + jj] - fy[ii * iWidth + jj] + p3[ii * iWidth + jj] / ρ3;
			if (Temp > thr)
				W[ii * iWidth + jj] = Temp - thr;
			else if (Temp < 0 - thr)
				W[ii * iWidth + jj] = Temp + thr;
			else
				W[ii * iWidth + jj] = 0;
		}
	}
}

//Shrink算子求V函数
void CDTVDenoiseDlg::Shrink_V(double* Skx, double* p4, double* V)
{
	double thr = λ3 / ρ4;

	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			double Temp = Skx[ii * iWidth + jj] - fx[ii * iWidth + jj] + p4[ii * iWidth + jj] / ρ4;
			if (Temp > thr)
				V[ii * iWidth + jj] = Temp - thr;
			else if (Temp < 0 - thr)
				V[ii * iWidth + jj] = Temp + thr;
			else
				V[ii * iWidth + jj] = 0;
		}
	}
}

//P1迭代函数
void CDTVDenoiseDlg::P1_iter(double* p1, double* Skθ, double* J)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			p1[ii * iWidth + jj] = p1[ii * iWidth + jj] + ρ1 * (Skθ[ii * iWidth + jj] - J[ii * iWidth + jj]);
		}
	}
}

//P2迭代函数
void CDTVDenoiseDlg::P2_iter(double* p2, double* Sk, double* H)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			p2[ii * iWidth + jj] = p2[ii * iWidth + jj] + ρ2 * (Sk[ii * iWidth + jj] - H[ii * iWidth + jj]);
		}
	}
}

//P3迭代函数
void CDTVDenoiseDlg::P3_iter(double* p3, double* Sky, double* W)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			p3[ii * iWidth + jj] = p3[ii * iWidth + jj] + ρ3 * (Sky[ii * iWidth + jj] - fy[ii * iWidth + jj] - W[ii * iWidth + jj]);
		}
	}
}

//P4迭代函数
void CDTVDenoiseDlg::P4_iter(double* p4, double* Skx, double* V)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			p4[ii * iWidth + jj] = p4[ii * iWidth + jj] + ρ4 * (Skx[ii * iWidth + jj] - fx[ii * iWidth + jj] - V[ii * iWidth + jj]);
		}
	}
}

//输出图像函数
void CDTVDenoiseDlg::WriteTIfOut(double* pData, CString sTifPathOut)
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

//申请存储空间函数
void CDTVDenoiseDlg::MallocData()
{
	u = NULL;				//u
	u = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		u[ii] = NULL;
	}

	f = NULL;				//f
	f = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		f[ii] = NULL;
	}

	fx = NULL;				//fx
	fx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fx[ii] = NULL;
	}

	fy = NULL;				//fy
	fy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fy[ii] = NULL;
	}

	fxx = NULL;				//fxx
	fxx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fxx[ii] = NULL;
	}

	fyy = NULL;				//fyy
	fyy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fyy[ii] = NULL;
	}

	fθθ = NULL;				//fθθ
	fθθ = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fθθ[ii] = NULL;
	}

	p = NULL;				//p
	p = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p[ii] = NULL;
	}

	I = NULL;				//I
	I = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		I[ii] = NULL;
	}

	q = NULL;				//q
	q = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		q[ii] = NULL;
	}

	pnei = NULL;			//pnei
	pnei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		pnei[ii] = NULL;
	}

	Inei = NULL;			//Inei
	Inei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Inei[ii] = NULL;
	}

	I2nei = NULL;			//I2nei
	I2nei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		I2nei[ii] = NULL;
	}

	pInei = NULL;			//pInei
	pInei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		pInei[ii] = NULL;
	}

	q_FFT = NULL;			//q_FFT
	q_FFT = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		q_FFT[ii] = NULL;
	}

	p1 = NULL;			//p1
	p1 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p1[ii] = NULL;
	}

	p2 = NULL;			//p2
	p2 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p2[ii] = NULL;
	}

	p3 = NULL;			//p3
	p3 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p3[ii] = NULL;
	}

	p4 = NULL;			//p4
	p4 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p4[ii] = NULL;
	}

	J = NULL;			//J
	J = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		J[ii] = NULL;
	}

	H = NULL;			//H
	H = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		H[ii] = NULL;
	}

	W = NULL;			//W
	W = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		W[ii] = NULL;
	}

	V = NULL;			//V
	V = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		V[ii] = NULL;
	}

	Sk = NULL;			//Sk
	Sk = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Sk[ii] = NULL;
	}

	Sk1 = NULL;			//Sk1
	Sk1 = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Sk1[ii] = NULL;
	}

	Skx = NULL;			//Skx
	Skx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Skx[ii] = NULL;
	}

	Sky = NULL;			//Sky
	Sky = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Sky[ii] = NULL;
	}

	Skθ = NULL;		//Skθ
	Skθ = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Skθ[ii] = NULL;
	}

	Skxx = NULL;		//Skxx
	Skxx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Skxx[ii] = NULL;
	}

	Skyy = NULL;		//Skyy
	Skyy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Skyy[ii] = NULL;
	}

	Skθθ = NULL;		//Skθθ
	Skθθ = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Skθθ[ii] = NULL;
	}

	SkNei = NULL;		//SkNei
	SkNei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		SkNei[ii] = NULL;
	}

	fNei = NULL;		//fNei
	fNei = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		fNei[ii] = NULL;
	}

	Jθ = NULL;		//Jθ
	Jθ = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Jθ[ii] = NULL;
	}

	p1θ = NULL;		//p1θ
	p1θ = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p1θ[ii] = NULL;
	}

	Wy = NULL;		//Wy
	Wy = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Wy[ii] = NULL;
	}

	p3y = NULL;		//p3y
	p3y = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p3y[ii] = NULL;
	}

	Vx = NULL;		//Vx
	Vx = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		Vx[ii] = NULL;
	}

	p4x = NULL;		//p4x
	p4x = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		p4x[ii] = NULL;
	}
}

//释放储存空间函数
void CDTVDenoiseDlg::DeleteData()
{
	free(u);
	u = NULL;			//u

	free(f);
	f = NULL;			//f

	free(fx);
	fx = NULL;			//fx

	free(fy);
	fy = NULL;			//fy

	free(fxx);
	fxx = NULL;			//fxx

	free(fyy);
	fyy = NULL;			//fyy

	free(fθθ);
	fθθ = NULL;		//fθθ

	free(p);
	p = NULL;			//p

	free(I);
	I = NULL;			//I

	free(q);
	q = NULL;			//q

	free(p);
	p = NULL;			//p

	free(I);
	I = NULL;			//I

	free(q);
	q = NULL;			//q

	free(pnei);
	pnei = NULL;		//peni

	free(Inei);
	Inei = NULL;		//Inei

	free(I2nei);
	I2nei = NULL;		//I2nei

	free(pInei);
	pInei = NULL;		//pInei
		
	free(q_FFT);
	q_FFT = NULL;		//q_FFT

	free(p1);
	p1 = NULL;			//p1

	free(p2);
	p2 = NULL;			//p2

	free(p3);
	p3 = NULL;			//p3

	free(p4);
	p4 = NULL;			//p4

	free(J);
	J = NULL;			//J

	free(H);
	H = NULL;			//H

	free(W);
	W = NULL;			//W

	free(V);
	V = NULL;			//V

	free(Sk);
	Sk = NULL;			//Sk

	free(Sk1);
	Sk1 = NULL;			//Sk1

	free(Skx);
	Skx = NULL;			//Skx

	free(Sky);
	Sky = NULL;			//Sky

	free(Skθ);
	Skθ = NULL;		//Sk

	free(Skxx);
	Skxx = NULL;		//Skxx

	free(Skyy);
	Skyy = NULL;		//Skyy

	free(Skθθ);
	Skθθ = NULL;		//Skθθ

	free(SkNei);
	SkNei = NULL;		//SkNei

	free(fNei);
	fNei = NULL;		//fNei

	free(Jθ);
	Jθ = NULL;			//Jθ

	free(p1θ);
	p1θ = NULL;		//p1θ

	free(Wy);
	Wy = NULL;			//Wy

	free(p3y);
	p3y = NULL;			//p3y

	free(Vx);
	Vx = NULL;			//Vx

	free(p4x);
	p4x = NULL;			//p4x
}