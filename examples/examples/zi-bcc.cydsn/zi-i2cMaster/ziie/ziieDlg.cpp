
// ziieDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ziie.h"
#include "ziieDlg.h"
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


// CZiieDlg 대화 상자



CZiieDlg::CZiieDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZIIE_DIALOG, pParent), m_pCom(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZiieDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LIST, m_log);
}

BEGIN_MESSAGE_MAP(CZiieDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_EXECUTE_BUTTON, &CZiieDlg::OnBnClickedExecuteButton)
END_MESSAGE_MAP()


// CZiieDlg 메시지 처리기

BOOL CZiieDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	L(_T("Initializing COM"));
	if (FAILED(CoInitialize(NULL)))
	{
		L(_T("Unable to initialize COM"));
	}
	L(_T("COM Initialized"));

	m_pCom = new CCom(_T("PSoCProgrammerCOM.PSoCProgrammerCOM_Object"));
	ASSERT(m_pCom);

	HRESULT hr = m_pCom->OpenPort();
	if (!SUCCEEDED(hr)) {
		L(_T("COM OpenPort failed, HRESULT: 0x%08X"), hr);
	}
	else {
		L(_T("COM OpenPort OK"));
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CZiieDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CZiieDlg::OnPaint()
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
HCURSOR CZiieDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CZiieDlg::L(const TCHAR* str, ...)
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

void CZiieDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pCom->ClosePort();
	CoUninitialize();
	delete m_pCom;
}


void CZiieDlg::OnBnClickedExecuteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//Setup Power - "5.0V" and internal
	HRESULT hr = m_pCom->SetPowerVoltage(_T("5.0"));
	if (!SUCCEEDED(hr)) {
		L(_T("Setup Power error,HRESULT: 0x%08X"), hr);
	}
	L(_T("Setup Power - 5.0V and internal done"));

	//Power On
	hr = m_pCom->PowerOn();
	if (!SUCCEEDED(hr)) {
		L(_T("Power On error,HRESULT: 0x%08X"), hr);
	}
	L(_T("Power On"));

	//Set protocol, connector and frequency
	hr = m_pCom->SetProtocol(enumInterfaces::I2C); //I2C-protocol
	if (!SUCCEEDED(hr)) {
		L(_T("SetProtocol error,HRESULT: 0x%08X"), hr);
	}
	L(_T("Set protocol, connector and frequency"));

	//Reset bus
	hr = m_pCom->I2C_ResetBus();
	L(_T("Reset bus!"));

	//Sleep script for 100 milliseconds
	Sleep(100);

	//Set I2C speed
	hr = m_pCom->I2C_SetSpeed(enumI2Cspeed::CLK_100K);
	if (!SUCCEEDED(hr)) {
		L(_T("Set speed: 100K error,HRESULT: 0x%08X"), hr);
	}
	L(_T("Set speed: 100K!"));
}
