
// Para_CalDlg.h: 头文件
//
#pragma once
#include "Cal.h";


// CParaCalDlg 对话框
class CParaCalDlg : public CDialogEx
{
// 构造
public:
	CParaCalDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARA_CAL_DIALOG };
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
	DECLARE_MESSAGE_MAP();
public:
	Parameter ParaCaler;
	int iChangeflag = 1;
	void ShowNodeData();
	afx_msg void OnBnClickedButtonCal();
	afx_msg void OnBnClickedButtonAdd();
	CListCtrl m_DataList;
	afx_msg void OnBnClickedButtonOriin();
	afx_msg void OnBnClickedButtonPrein();
	afx_msg void OnBnClickedButtonOutput();
	afx_msg void OnLvnItemchangedListDatalist(NMHDR* pNMHDR, LRESULT* pResult);
	int iItemNum = 0;
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonBand();
	afx_msg void OnBnClickedButtonMeanband();
	afx_msg void OnBnClickedButtonSpecl();
};
