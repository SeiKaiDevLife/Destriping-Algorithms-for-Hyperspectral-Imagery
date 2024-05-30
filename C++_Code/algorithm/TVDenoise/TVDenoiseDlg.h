
// TVDenoiseDlg.h: 头文件
//

#pragma once

#include "gdal_priv.h"

// CTVDenoiseDlg 对话框
class CTVDenoiseDlg : public CDialogEx
{
// 构造
public:
	CTVDenoiseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TVDENOISE_DIALOG };
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
	double mu;
	double tol;
	double iter;
	double* f;
	double* uk;
	double* nei;
	double* uk1;
	double* uk1_uk;
	double* dx;
	double* dy;
	double* bx;
	double* by;
	double* uk_x;
	double* uk_y;
	double* ddx;
	double* ddy;
	double* dbx;
	double* dby;
	afx_msg void OnBnClickedButtonTv();
	void Copy(BYTE* InputData, double* pOutputData);
	void Copy(int* InputData, double* pOutputData);
	void Copy(float* pInputData, double* pOutputData);
	void Copy(double* pInputData, double* pOutputData);
	double Norm(double* pInputData);
	void Subtract(double* pInputData1, double* pInputData2, double* pOutputData);
	void Grad_x(double* pInputData, double* pOutputData);
	void Grad_y(double* pInputData, double* pOutputData);

	void Grad_x_t(double* pInputData, double* pOutputData);
	void Grad_y_t(double* pInputData, double* pOutputData);

	void FFT_method();

	void Neighbors(double* pInputData, double* pOutputData);
	void Gaussian_method(double* uk, double* uk1, double* f, double* dx, double* dy, double* bx, double* by, double lambd, double mu);
	void Shrink_x(double* uk_x, double* bx, double lambd, double* dx);
	void Shrink_y(double* uk_y, double* by, double lambd, double* dy);
	void Bx_iter(double* uk_x, double* dx, double* bx);
	void By_iter(double* uk_y, double* dy, double* by);
	void WriteTIfOut(double* pData, CString sTifPathOut);
};
