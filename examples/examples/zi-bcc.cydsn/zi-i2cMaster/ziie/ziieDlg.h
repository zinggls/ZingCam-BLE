
// ziieDlg.h: 헤더 파일
//

#pragma once

#define READ_BUFFER_SIZE	174

class CCom;

// CZiieDlg 대화 상자
class CZiieDlg : public CDialogEx
{
// 생성입니다.
public:
	CZiieDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZIIE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CCom *m_pCom;
	CListBox m_log;

	void L(const TCHAR* str, ...);
	long Control_I2C_SCB_Slave(int deviceAddress);
	BOOL SetPowerVoltage();
	BOOL PowerOn();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedExecuteButton();
};
