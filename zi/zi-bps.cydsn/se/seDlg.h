
// seDlg.h: 헤더 파일
//

#pragma once

class CCom;

// CseDlg 대화 상자
class CseDlg : public CDialogEx
{
// 생성입니다.
public:
	CseDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SE_DIALOG };
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
	CListBox m_log;
	CComboBox m_portsCombo;

	void L(const TCHAR* str, ...);
	BOOL COM_Init();
	void COM_UnInit();
	void FillPortsCombo();
	void CreateWriteBuffer();
	void EnableCombos(BOOL b);
	CComboBox m_scopeKindChangeNotiCombo;
	CComboBox m_scopeOutChangeNotiCombo;
	CComboBox m_scopeOperationModeCombo;
	CComboBox m_scopeStateKindCombo;
	CComboBox m_scopeStateBatteryCombo;
	CComboBox m_scopeStateIrCombo;
	CComboBox m_scopeStateEoCombo;
	CComboBox m_scopeStateOutCombo;
	CString m_strScopeKindChangeNoti;
	CString m_strScopeOutChangeNoti;
	CString m_strScopeOperationMode;
	CListCtrl m_writeBufferListCtrl;
	BOOL m_bReadBuffer;
	BOOL m_bWriteBuffer;
	afx_msg void OnBnClickedRawClearButton();
	afx_msg void OnBnClickedReadBufferCheck();
	afx_msg void OnBnClickedWriteBufferCheck();
};
