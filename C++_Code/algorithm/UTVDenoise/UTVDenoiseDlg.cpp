
// UTVDenoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "UTVDenoise.h"
#include "UTVDenoiseDlg.h"
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


// CUTVDenoiseDlg 对话框



CUTVDenoiseDlg::CUTVDenoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UTVDENOISE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUTVDenoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUTVDenoiseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_UTV, &CUTVDenoiseDlg::OnBnClickedButtonUtv)
END_MESSAGE_MAP()


// CUTVDenoiseDlg 消息处理程序

BOOL CUTVDenoiseDlg::OnInitDialog()
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

void CUTVDenoiseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUTVDenoiseDlg::OnPaint()
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
HCURSOR CUTVDenoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUTVDenoiseDlg::OnBnClickedButtonUtv()
{
		/*程序开始，读取基本信息
	*********************************************************************************************************************************************************************************/

	//设置读取图像的路径和输出图像的路径
	CString sPathTifIn = "D:\\RS-RPC\\TEST\\IN\\20P.tif";
	//CString sPathTifIn = "D:\\TestData\\Result\\Test_NL_SN.tif";
	CString sPathTifOut = "D:\\RS-RPC\\TEST\\OUT\\U_";
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
	//	pDataInput[ii] = 0;
	//}

	//float* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (float*)malloc(iHeight * iWidth * sizeof(float));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = 0;
	//}

	//int* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (int*)malloc(iHeight * iWidth * sizeof(int));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = 0;
	//}

	double* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	pDataInput = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		pDataInput[ii] = 0;
	}

	/*将需要读取的图像读入内存
	********************************************************************************************************************************************************************************/

	//读取图像
	GDALRasterBand* pBandInput = pDatasetInput->GetRasterBand(45);
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
	Copy(pDataInput, vk);
	Copy(pDataInput, uk1);
	lambd = 100;
	iter = 20;

	for (int ii = 0; ii < iter; ii++)
	{
		CString sii;
		sii.Format("%d", ii);
		//G-S不动点迭代法求当前Lambda下的uk图像
		for (int jj = 0; jj < 100; jj++)
		{
			Copy(uk1, uk);
			Gaussian_method();
		}
		

		//将迭代结果uk加入到u图像中
		Copy(u, utemp);
		Add(utemp, uk1, u);

		//从原图中减去u图像，剩下的vk图像作为下一次迭代的输入数据
		Subtract(f, u, vk);

		WriteTIfOut(vk, sPathTifOut + sii + "vk.tif");

		//Lambda变为原来的一半
		lambd = lambd / 2;

		//将vk的数值赋值给uk1，进入下一次G-S不动点迭代法
		Copy(vk, uk1);
		WriteTIfOut(uk1, sPathTifOut + sii + "uk1.tif");
		//暂时输出当前的图像
		
		WriteTIfOut(u, sPathTifOut + sii + ".tif");
	}

	/*释放空间并结束程序
	********************************************************************************************************************************************************************************/

	DeleteData();

	AfxMessageBox("Done");
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CUTVDenoiseDlg::Copy(BYTE* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CUTVDenoiseDlg::Copy(short* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CUTVDenoiseDlg::Copy(int* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CUTVDenoiseDlg::Copy(float* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CUTVDenoiseDlg::Copy(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = (double)pInputData[ii];
	}
}

//相减函数
//输入：pInputData1：被减数；pInputData2：减数；pOutputData：减法结果
void CUTVDenoiseDlg::Subtract(double* pInputData1, double* pInputData2, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = pInputData1[ii] - pInputData2[ii];
	}
}

//相加函数
//输入：pInputData1：加数；pInputData2：加数；pOutputData：加法结果
void CUTVDenoiseDlg::Add(double* pInputData1, double* pInputData2, double* pOutputData)
{
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutputData[ii] = pInputData1[ii] + pInputData2[ii];
	}
}

//求U邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CUTVDenoiseDlg::NeighborsU(double* pInputData, double* pOutputData)
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
				lambd * pInputData[ii_left * iWidth + jj_left] +
				lambd * pInputData[ii_right * iWidth + jj_right] +
				pInputData[ii_up * iWidth + jj_up] +
				pInputData[ii_down * iWidth + jj_down];
		}
	}
}

//求V邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CUTVDenoiseDlg::NeighborsV(double* pInputData, double* pOutputData)
{
	for (int ii = 0; ii < iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			int ii_up = ii + 1;
			int jj_up = jj;
			if (ii_up == iHeight)
				ii_up = 0;

			int ii_down = ii - 1;
			int jj_down = jj;
			if (ii_down < 0)
				ii_down = iHeight - 1;

			pOutputData[ii * iWidth + jj] =
				pInputData[ii * iWidth + jj] * 2 -
				pInputData[ii_up * iWidth + jj_up] -
				pInputData[ii_down * iWidth + jj_down];
		}
	}
}

//G-S迭代
void CUTVDenoiseDlg::Gaussian_method()
{
	NeighborsU(uk, NeiU);
	NeighborsV(vk, NeiV);

	//WriteTIfOut(NeiU, "D:\\PaviaC\\Test\\NeiU.tif");
	//WriteTIfOut(NeiV, "D:\\PaviaC\\Test\\NeiV.tif");

	for (int ii = 0; ii<iHeight; ii++)
	{
		for (int jj = 0; jj < iWidth; jj++)
		{
			uk1[ii * iWidth + jj] = (NeiU[ii * iWidth + jj] + NeiV[ii * iWidth + jj]) / (2 + 2 * lambd);
		}
	}
}

//输出图像函数
//输入：pData：需要输出的数据；sTifPathOut：输出路径
void CUTVDenoiseDlg::WriteTIfOut(double* pData, CString sTifPathOut)
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

//开辟储存空间函数
void CUTVDenoiseDlg::MallocData()
{
	u = NULL;	//u
	u = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		u[ii] = NULL;
	}

	utemp = NULL;	//utemp
	utemp = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		utemp[ii] = NULL;
	}

	f = NULL;	//f
	f = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		f[ii] = NULL;
	}

	NeiU = NULL;	//NeiU
	NeiU = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		NeiU[ii] = NULL;
	}

	NeiV = NULL;	//NeiV
	NeiV = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		NeiV[ii] = NULL;
	}

	vk = NULL;	//vk
	vk = (double*)malloc(iHeight * iWidth * sizeof(double));
	for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	{
		vk[ii] = NULL;
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
}

//释放储存空间函数
void CUTVDenoiseDlg::DeleteData()
{
	free(u);
	u = NULL;		//u

	free(utemp);
	utemp = NULL;	//utemp

	free(f);
	f = NULL;		//f

	free(NeiU);
	NeiU = NULL;	//NeiU

	free(NeiV);
	NeiV = NULL;	//NeiV

	free(vk);
	vk = NULL;		//vk

	free(uk);
	uk = NULL;		//uk

	free(uk1);
	uk1 = NULL;		//uk+1
}