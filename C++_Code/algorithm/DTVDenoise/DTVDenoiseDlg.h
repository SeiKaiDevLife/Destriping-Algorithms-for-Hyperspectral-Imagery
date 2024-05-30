
// DTVDenoiseDlg.h: 头文件
//

#pragma once

#include "gdal_priv.h"

// CDTVDenoiseDlg 对话框
class CDTVDenoiseDlg : public CDialogEx
{
// 构造
public:
	CDTVDenoiseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DTVDENOISE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int iWidth;	//宽度：5000
	int iHeight;	//高度：5000
	int iBandNum;	//波段数量：1
	double lambd;

	double* u;
	double iter;

	double λ1;
	double λ2;
	double λ3;

	double μ1;
	double μ2;
	double μ3;

	double ρ1;
	double ρ2;
	double ρ3;
	double ρ4;

	double* p1;
	double* p2;
	double* p3;
	double* p4;

	double* J;
	double* H;
	double* W;
	double* V;

	double* f;
	double* fx;
	double* fy;
	double* fxx;
	double* fyy;
	double* fθθ;

	double* p;
	double* I;
	double* pnei;
	double* Inei;
	double* I2nei;
	double* pInei;
	double* q;
	double StripeDegree;
	double* q_FFT;

	double* Sk;
	double* Sk1;

	double* Skx;
	double* Sky;
	double* Skθ;

	double* Skxx;
	double* Skyy;
	double* Skθθ;

	double* SkNei;
	double* fNei;

	double* Jθ;
	double* p1θ;
	double* Wy;
	double* p3y;
	double* Vx;
	double* p4x;

	afx_msg void OnBnClickedButtonDtv();

	void Copy(BYTE* InputData, double* pOutputData);
	void Copy(short* InputData, double* pOutputData);
	void Copy(int* InputData, double* pOutputData);
	void Copy(double* pInputData, double* pOutputData);

	void Subtract(double* pInputData1, double* pInputData2, double* pOutputData);
	void Add(double* pInputData1, double* pInputData2, double* pOutputData);

	void NeighborsU(double* pInputData, double* pOutputData);
	void NeighborsV(double* pInputData, double* pOutputData);

	void Neighbors(double* pInputData, double* pOutputData);
	void Neighborsf(double* pInputData, double* pOutputData);

	void Gaussian_method();

	void fre_spectrum(double* pInputData, double* pOutputData);
	void Max_Fre_Dre_Cal();

	void GuidedFilter();
	void p_nei_Cal(int iRange);
	void I_nei_Cal(int iRange);
	void I2_nei_Cal(int iRange);
	void pI_nei_Cal(int iRange);

	void Grad_x(double* pInputData, double* pOutputData);
	void Grad_y(double* pInputData, double* pOutputData);
	void Grad_θ(double* pInputData, double* pOutputData);

	void Grad_xx(double* pInputData, double* pOutputData);
	void Grad_yy(double* pInputData, double* pOutputData);
	void Grad_θθ(double* pInputData, double* pOutputData);

	void Shrink_J(double* Skθ, double* p1, double* J);
	void Shrink_H(double* Sk, double* p2, double* H);
	void Shrink_W(double* Sky, double* p3, double* W);
	void Shrink_V(double* Skx, double* p4, double* V);

	void P1_iter(double* p1, double* Skθ, double* J);
	void P2_iter(double* p2, double* Sk, double* H);
	void P3_iter(double* p3, double* Sky, double* W);
	void P4_iter(double* p4, double* Skx, double* V);

	void WriteTIfOut(double* pData, CString sTifPathOut);
	void MallocData();
	void DeleteData();
};
