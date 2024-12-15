
// seDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "se.h"
#include "seDlg.h"
#include "afxdialogex.h"
#include "com.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CseDlg 대화 상자



CseDlg::CseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LIST, m_log);
	DDX_Control(pDX, IDC_PORTS_COMBO, m_portsCombo);
	DDX_Control(pDX, IDC_SCOPE_KIND_CHANGE_NOTI_COMBO, m_scopeKindChangeNotiCombo);
	DDX_Control(pDX, IDC_SCOPE_OUT_CHANGE_NOTI_COMBO, m_scopeOutChangeNotiCombo);
	DDX_Control(pDX, IDC_OPMODE_SCOPE_COMBO, m_scopeOperationModeCombo);
	DDX_Control(pDX, IDC_SCOPE_STATE_KIND_COMBO, m_scopeStateKindCombo);
	DDX_Control(pDX, IDC_SCOPE_STATE_OUT_COMBO, m_scopeStateOutCombo);
	DDX_Control(pDX, IDC_SCOPE_STATE_BATTERY_COMBO, m_scopeStateBatteryCombo);
	DDX_Control(pDX, IDC_SCOPE_STATE_IR_COMBO, m_scopeStateIrCombo);
	DDX_Control(pDX, IDC_SCOPE_STATE_EO_COMBO, m_scopeStateEoCombo);
}

BEGIN_MESSAGE_MAP(CseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CseDlg 메시지 처리기

BOOL CseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	FillPortsCombo();
	m_scopeKindChangeNotiCombo.AddString(_T("알림 없음/알림 확인 완료"));
	m_scopeKindChangeNotiCombo.AddString(_T("EO 변경 알림"));
	m_scopeKindChangeNotiCombo.AddString(_T("IR 백상 변경 알림"));
	m_scopeKindChangeNotiCombo.AddString(_T("IR 흑상 변경 알림"));

	m_scopeOutChangeNotiCombo.AddString(_T("알림 없음/알림 확인 완료"));
	m_scopeOutChangeNotiCombo.AddString(_T("영상 출력 변경 알림"));
	m_scopeOutChangeNotiCombo.AddString(_T("영상 미출력 변경 알림"));

	m_scopeOperationModeCombo.AddString(_T("(default)"));
	m_scopeOperationModeCombo.AddString(_T("운용모드"));
	m_scopeOperationModeCombo.AddString(_T("대기모드"));
	m_scopeOperationModeCombo.AddString(_T("절전모드"));

	m_scopeStateKindCombo.AddString(_T("EO"));
	m_scopeStateKindCombo.AddString(_T("IR 백상(Default)"));
	m_scopeStateKindCombo.AddString(_T("IR 흑상"));

	m_scopeStateOutCombo.AddString(_T("출력"));
	m_scopeStateOutCombo.AddString(_T("미출력"));

	for (int i = 0; i <= 100; i++) {
		CString str;
		str.Format(_T("%d%%"), i);
		m_scopeStateBatteryCombo.AddString(str);
	}

	m_scopeStateIrCombo.AddString(_T("정상"));
	m_scopeStateIrCombo.AddString(_T("IR모듈 이상"));

	m_scopeStateEoCombo.AddString(_T("정상"));
	m_scopeStateEoCombo.AddString(_T("EO모듈 이상"));



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CseDlg::L(const TCHAR* str, ...)
{
	va_list args;
	va_start(args, str);

	int len = _vsctprintf(str, args) + 1;	//_vscprintf doesn't count terminating '\0'
	TCHAR* buffer = new TCHAR[len];
	_vstprintf(buffer, len, str, args);
	va_end(args);

	m_log.AddString(buffer);
	delete[](buffer);

	m_log.SetTopIndex(m_log.GetCount() - 1);
}

BOOL CseDlg::COM_Init()
{
	L(_T("Initializing COM"));
	if (FAILED(CoInitialize(NULL)))
	{
		L(_T("Unable to initialize COM"));
		return FALSE;
	}
	L(_T("COM Initialized"));
	return TRUE;
}

void CseDlg::COM_UnInit()
{
	CoUninitialize();
	L(_T("COM Uninitialized"));
}

void CseDlg::FillPortsCombo()
{
	COM_Init();

	CCom com;
	std::vector<std::wstring> ports;
	long hr = com.GetPorts(ports);
	if (!SUCCEEDED(hr)) {
		COM_UnInit();
		return;
	}
	for (size_t i = 0; i < ports.size(); i++) m_portsCombo.AddString(ports[i].c_str());

	if (ports.size() > 0) {
		m_portsCombo.SetCurSel(0);
	}
	else {
		GetDlgItem(IDC_I2C_READ_BUTTON)->EnableWindow(FALSE);
	}

	COM_UnInit();
}