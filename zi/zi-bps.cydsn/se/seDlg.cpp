
// seDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "se.h"
#include "seDlg.h"
#include "afxdialogex.h"
#include "com.h"
#include "version.h"
#include "git_describe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CseDlg::bRead = FALSE;

class CComAuto {
public:
	CComAuto(CseDlg* p) :m_p(p) { p->COM_Init(); }
	~CComAuto() { m_p->COM_UnInit(); }
private:
	CseDlg* m_p;
};

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
public:
	CString m_strVersion;
	CString m_strDescribe;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
, m_strVersion(_T(""))
, m_strDescribe(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VERSION_STATIC, m_strVersion);
	DDX_Text(pDX, IDC_DESCRIBE_STATIC, m_strDescribe);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CseDlg 대화 상자



CseDlg::CseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SE_DIALOG, pParent)
	, m_pCom(NULL)
	, m_strScopeKindChangeNoti(_T("0.종류 변경요청: "))
	, m_strScopeOutChangeNoti(_T("1.출력 변경요청:"))
	, m_strScopeOperationMode(_T("2.조준경 운용모드:"))
	, m_bReadBuffer(TRUE)
	, m_bWriteBuffer(TRUE)
	, m_bSendWriteBuffer(FALSE)
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
	DDX_Control(pDX, IDC_SCOPE_STATE_USB_DETECT_COMBO, m_scopeStateUsbDetectCombo);
	DDX_Text(pDX, IDC_SCOPE_KIND_CHANGE_NOTI_STATIC, m_strScopeKindChangeNoti);
	DDX_Text(pDX, IDC_SCOPE_OUT_CHANGE_NOTI_STATIC, m_strScopeOutChangeNoti);
	DDX_Text(pDX, IDC_OPMODE_SCOPE_STATIC, m_strScopeOperationMode);
	DDX_Control(pDX, IDC_WRITE_BUFFER_LIST, m_writeBufferListCtrl);
	DDX_Check(pDX, IDC_READ_BUFFER_CHECK, m_bReadBuffer);
	DDX_Check(pDX, IDC_WRITE_BUFFER_CHECK, m_bWriteBuffer);
}

BEGIN_MESSAGE_MAP(CseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RAW_CLEAR_BUTTON, &CseDlg::OnBnClickedRawClearButton)
	ON_BN_CLICKED(IDC_READ_BUFFER_CHECK, &CseDlg::OnBnClickedReadBufferCheck)
	ON_BN_CLICKED(IDC_WRITE_BUFFER_CHECK, &CseDlg::OnBnClickedWriteBufferCheck)
	ON_BN_CLICKED(IDC_I2C_READ_BUTTON, &CseDlg::OnBnClickedI2cReadButton)
	ON_BN_CLICKED(IDC_I2C_WRITE_BUTTON, &CseDlg::OnBnClickedI2cWriteButton)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_SCOPE_KIND_CHANGE_NOTI_COMBO, &CseDlg::OnCbnSelchangeScopeKindChangeNotiCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_OUT_CHANGE_NOTI_COMBO, &CseDlg::OnCbnSelchangeScopeOutChangeNotiCombo)
	ON_CBN_SELCHANGE(IDC_OPMODE_SCOPE_COMBO, &CseDlg::OnCbnSelchangeOpmodeScopeCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_STATE_KIND_COMBO, &CseDlg::OnCbnSelchangeScopeStateKindCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_STATE_OUT_COMBO, &CseDlg::OnCbnSelchangeScopeStateOutCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_STATE_BATTERY_COMBO, &CseDlg::OnCbnSelchangeScopeStateBatteryCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_STATE_IR_COMBO, &CseDlg::OnCbnSelchangeScopeStateIrCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_STATE_EO_COMBO, &CseDlg::OnCbnSelchangeScopeStateEoCombo)
	ON_CBN_DROPDOWN(IDC_PORTS_COMBO, &CseDlg::OnCbnDropdownPortsCombo)
	ON_BN_CLICKED(IDC_I2C_RESET_BUTTON, &CseDlg::OnBnClickedI2cResetButton)
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
	CreateWriteBuffer();

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

	m_scopeStateKindCombo.AddString(_T("미정의"));
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

	EnableCombos(FALSE);

	SetTimer(1, 100, NULL);

	GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(FALSE);

	CString strDescribe(GIT_DESCRIBE);
	SetWindowText(_T("se ") + strDescribe);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.m_strVersion = VER_INFO;
		dlgAbout.m_strDescribe = GIT_DESCRIBE;
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

BOOL CseDlg::COM_Init(BOOL bSilent)
{
	if(!bSilent) L(_T("Initializing COM"));
	if (FAILED(CoInitialize(NULL)))
	{
		L(_T("Unable to initialize COM"));
		return FALSE;
	}
	if (!bSilent) L(_T("COM Initialized"));
	return TRUE;
}

void CseDlg::COM_UnInit(BOOL bSilent)
{
	CoUninitialize();
	if (!bSilent) L(_T("COM Uninitialized"));
}

void CseDlg::ResetI2CReadButton()
{
	bRead = FALSE;
	GetDlgItem(IDC_I2C_READ_BUTTON)->SetWindowText(_T("Read"));
	GetDlgItem(IDC_PORTS_COMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(FALSE);
	EnableCombos(FALSE);
}

void CseDlg::FillPortsCombo()
{
	COM_Init(TRUE);

	CCom com;
	std::vector<std::wstring> ports;
	long hr = com.GetPorts(ports);
	if (!SUCCEEDED(hr)) {
		COM_UnInit();
		return;
	}

	m_portsCombo.ResetContent();
	for (size_t i = 0; i < ports.size(); i++) m_portsCombo.AddString(ports[i].c_str());

	if (ports.size() > 0) {
		m_portsCombo.SetCurSel(0);
		GetDlgItem(IDC_I2C_READ_BUTTON)->EnableWindow(TRUE);
	}
	else {
		GetDlgItem(IDC_I2C_READ_BUTTON)->EnableWindow(FALSE);
	}

	COM_UnInit(TRUE);
}

void CseDlg::CreateWriteBuffer()
{
	m_writeBufferListCtrl.EnableWindow(FALSE);

	int nHexWidth = 27;
	m_writeBufferListCtrl.InsertColumn(0, _T("0"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(1, _T("1"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(2, _T("2"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(3, _T("3"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(4, _T("4"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(5, _T("5"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(6, _T("6"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(7, _T("7"), LVCFMT_RIGHT, nHexWidth);

	m_writeBufferListCtrl.InsertItem(0, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 0, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 1, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 2, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 3, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 4, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 5, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 6, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 7, _T(""));
}

void CseDlg::EnableCombos(BOOL b)
{
	m_scopeKindChangeNotiCombo.EnableWindow(b);
	m_scopeOutChangeNotiCombo.EnableWindow(b);
	m_scopeOperationModeCombo.EnableWindow(b);
	m_scopeStateKindCombo.EnableWindow(b);
	m_scopeStateBatteryCombo.EnableWindow(b);
	m_scopeStateIrCombo.EnableWindow(b);
	m_scopeStateEoCombo.EnableWindow(b);
	m_scopeStateOutCombo.EnableWindow(b);
	m_scopeStateUsbDetectCombo.EnableWindow(b);
}

void CseDlg::OnBnClickedRawClearButton()
{
	m_log.ResetContent();
}

void CseDlg::OnBnClickedReadBufferCheck()
{
	m_bReadBuffer = !m_bReadBuffer;
	UpdateData(FALSE);
}


void CseDlg::OnBnClickedWriteBufferCheck()
{
	m_bWriteBuffer = !m_bWriteBuffer;
	UpdateData(FALSE);
}

BOOL CseDlg::COM_OpenPort()
{
	m_pCom = new CCom();
	ASSERT(m_pCom);

	CString strPort;
	m_portsCombo.GetLBText(m_portsCombo.GetCurSel(), strPort);

	HRESULT hr = m_pCom->OpenPort(strPort.GetString());
	if (!SUCCEEDED(hr)) {
		L(_T("COM OpenPort failed, HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	else {
		L(_T("COM OpenPort OK"));
		return TRUE;
	}
}

BOOL CseDlg::SetPowerVoltage()
{
	//Setup Power - "5.0V" and internal
	HRESULT hr = m_pCom->SetPowerVoltage(_T("5.0"));
	if (!SUCCEEDED(hr)) {
		L(_T("Setup Power error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Setup Power - 5.0V and internal done"));
	return TRUE;
}

BOOL CseDlg::PowerOn()
{
	HRESULT hr = m_pCom->PowerOn();
	if (!SUCCEEDED(hr)) {
		L(_T("Power On error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Power On"));
	return TRUE;
}

BOOL CseDlg::SetProtocol()
{
	//Set protocol, connector and frequency
	HRESULT hr = m_pCom->SetProtocol(enumInterfaces::I2C); //I2C-protocol
	if (!SUCCEEDED(hr)) {
		L(_T("SetProtocol error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Set protocol, connector and frequency"));
	return TRUE;
}

BOOL CseDlg::I2C_ResetBus()
{
	//Reset bus
	HRESULT hr = m_pCom->I2C_ResetBus();
	if (!SUCCEEDED(hr)) {
		L(_T("Reset bus error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Reset bus!"));
	return TRUE;
}

BOOL CseDlg::I2C_SetSpeed()
{
	//Set I2C speed
	HRESULT hr = m_pCom->I2C_SetSpeed((enumI2Cspeed)0x05);	//ppcom.h에는 CLK_1000K 0x05 정의가 안되어 있어서 강제로 타입변환
	if (!SUCCEEDED(hr)) {
		L(_T("Set speed: 1000K error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Set speed: 1000K"));
	return TRUE;
}

BOOL CseDlg::I2C_GetSpeed()
{
	//Get I2C speed
	long speed = 0;
	CString val = _T("");
	HRESULT hr = m_pCom->I2C_GetSpeed(speed);
	if (!SUCCEEDED(hr)) {
		L(_T("Get speed error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}

	if (speed == 1) {
		val = _T("100K");
	}
	else if (speed == 4) {
		val = _T("50K");
	}
	else if (speed == 2) {
		val = _T("400K");
	}
	L(_T("Get speed: %s"), val);
	return TRUE;
}

BOOL CseDlg::I2C_GetDeviceList()
{
	//Get device list
	HRESULT hr = m_pCom->I2C_GetDeviceList(m_devices);
	if (!SUCCEEDED(hr))
	{
		L(_T("Failed to enumerate I2C devices,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Enumerate I2C devices"));

	//Show devices
	if (m_devices.size() == 0)
	{
		L(_T("No devices found"));
		return FALSE;
	}

	L(_T("Devices list:  8bit  7bit"));
	for (size_t i = 0; i < m_devices.size(); i++) L(_T("     address:  %02x    %02x"), m_devices[i] << 1, m_devices[i]);
	return TRUE;
}

size_t CseDlg::Parse_I2C(std::vector<byte>& dataOUT, SCOPE_READ& sr)
{
	size_t index = 0;

	sr.command.scopeKindChangeNotify = dataOUT[0];	index++;
	sr.command.scopeOutChangeNotify = dataOUT[1];	index++;
	sr.command.scopeOperationMode = dataOUT[2];		index++;

	return index;
}

void CseDlg::UpdateScopeKindChangeNoti(byte kind)
{
	m_strScopeKindChangeNoti = _T("0.종류 변경요청: ");

	/*
	0x00 : 알림 없음/알림 반영 완료
	0x01 : 영상융합처리기->조준경 EO 변경 요청 알림
	0x02 : 영상융합처리기->조준경 IR 백상 변경 요청 알림
	0x03 : 영상융합처리기->조준경 IR 흑상 변경 요청 알림
	*/

	CString str;
	switch (kind) {
	case 0:
		str = _T("알림없음/알림반영완료(0)");
		break;
	case 1:
		str.Format(_T("EO 변경요청(%x)"), kind);
		break;
	case 2:
		str.Format(_T("IR 백상 변경요청(%x)"), kind);
		break;
	case 3:
		str.Format(_T("IR 흑상 변경요청(%x)"), kind);
		break;
	default:
		str.Format(_T("미정의(%x)"), kind);
		break;
	}
	m_strScopeKindChangeNoti += str;
}

void CseDlg::UpdateScopeOutChangeNoti(byte out)
{
	m_strScopeOutChangeNoti = _T("1.출력 변경요청: ");

	/*
	0x00 : 알림 없음/알림 반영 완료
	0x01 : 영상융합처리기 -> 조준경 영상 출력으로 변경 요청 알림
	0x02 : 영상융합처리기 -> 조준경 영상 미출력으로 변경 요청 알림
	*/

	CString str;
	switch (out) {
	case 0:
		str = _T("알림없음/알림반영완료(0)");
		break;
	case 1:
		str.Format(_T("영상 출력 변경요청(%x)"), out);
		break;
	case 2:
		str.Format(_T("영상 미출력 변경요청(%x)"), out);
		break;
	default:
		str.Format(_T("미정의(%x)"), out);
		break;
	}
	m_strScopeOutChangeNoti += str;
}

void CseDlg::UpdateScopeOperationMode(byte mode)
{
	m_strScopeOperationMode = _T("2.조준경 운용모드: ");

	/*
	0x00 : (default)
	0x01 : 운용모드
	0x02 : 대기모드
	0x04 : 절전모드
	*/

	CString str;
	switch (mode) {
	case 0:
		str = _T("(default)(0)");
		break;
	case 1:
		str.Format(_T("운용모드(%x)"), mode);
		break;
	case 2:
		str.Format(_T("대기모드(%x)"), mode);
		break;
	case 4:
		str.Format(_T("절전모드(%x)"), mode);
		break;
	default:
		str.Format(_T("미정의(%x)"), mode);
		break;
	}
	m_strScopeOperationMode += str;
}

CString CseDlg::DecStr(int val)
{
	CString str;
	str.Format(_T("%d"), val);
	return str;
}

CString CseDlg::HexStr(int val)
{
	CString str;
	str.Format(_T("%x"), val);
	return str;
}

void CseDlg::UpdateWriteBufferGUI(SCOPE_WRITE& sw)
{
	m_writeBufferListCtrl.SetItemText(0, 0, HexStr(sw.command.scopeKindChangeNotify));
	m_writeBufferListCtrl.SetItemText(0, 1, HexStr(sw.command.scopeOutChangeNotify));
	m_writeBufferListCtrl.SetItemText(0, 2, HexStr(sw.command.scopeOperationMode));
	m_writeBufferListCtrl.SetItemText(0, 3, HexStr(sw.state.kind));
	m_writeBufferListCtrl.SetItemText(0, 4, HexStr(sw.state.out));
	m_writeBufferListCtrl.SetItemText(0, 5, HexStr(sw.state.battery));
	m_writeBufferListCtrl.SetItemText(0, 6, HexStr(sw.state.ir));
	m_writeBufferListCtrl.SetItemText(0, 7, HexStr(sw.state.eo));
}

void CseDlg::UpdateGUI(SCOPE& s)
{
	UpdateScopeKindChangeNoti(s.read.command.scopeKindChangeNotify);
	UpdateScopeOutChangeNoti(s.read.command.scopeOutChangeNotify);
	UpdateScopeOperationMode(s.read.command.scopeOperationMode);
	UpdateWriteBufferGUI(s.write);
}

static unsigned long count = 0;

CString CseDlg::RawString(std::vector<byte>& dataOUT)
{
	CString str;
	str.Format(_T("%lu [%Iu] "), count++, dataOUT.size());
	for (size_t i = 0; i < dataOUT.size(); i++) {
		CString tmp;
		tmp.Format(_T("%02X "), dataOUT[i]);
		str += tmp;
	}
	return str;
}

HRESULT CseDlg::Send_I2C_WriteBuffer(int deviceAddress)
{
	std::vector<byte> dataIN;
	dataIN.resize(WRITE_BUFFER_SIZE);
	dataIN[0] = m_scope.write.command.scopeKindChangeNotify;
	dataIN[1] = m_scope.write.command.scopeOutChangeNotify;
	dataIN[2] = m_scope.write.command.scopeOperationMode;
	dataIN[3] = m_scope.write.state.kind;
	dataIN[4] = m_scope.write.state.out;
	dataIN[5] = m_scope.write.state.battery;
	dataIN[6] = m_scope.write.state.ir;
	dataIN[7] = m_scope.write.state.eo;

	HRESULT hr = m_pCom->writeI2C(deviceAddress, dataIN);
	if (!SUCCEEDED(hr)) {
		L(_T("Failed writeI2C,HRESULT: 0x%08X"), hr);
		return hr;
	}

	if (m_bWriteBuffer) {
		SCOPE_WRITE& s = m_scope.write;
		L(_T("I2C Write Buffer[%d]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x"),
			sizeof(SCOPE_WRITE), s.command.scopeKindChangeNotify,s.command.scopeOutChangeNotify,s.command.scopeOperationMode,s.state.kind,s.state.out,s.state.battery,s.state.ir,s.state.eo);
	}
	return S_OK;
}

void CseDlg::ResetWriteBufferList()
{
	memset(&m_scope.write, 0, sizeof(SCOPE_WRITE));
	CString str(_T("0"));
	for (int i = 0; i <= 7; i++) m_writeBufferListCtrl.SetItemText(0, i, str);
}

HRESULT CseDlg::Read_I2C_SCB_Slave(int deviceAddress)
{
	HRESULT hr;
	CString str;
	std::vector<byte> dataOUT;
	size_t index;
	while (1) {
		Sleep(100);
		hr = m_pCom->readI2C(deviceAddress, READ_BUFFER_SIZE, dataOUT);
		if (!SUCCEEDED(hr)) {
			L(_T("Failed readI2C,HRESULT: 0x%08X"), hr);
			return hr;
		}

		index = Parse_I2C(dataOUT, m_scope.read);
		ASSERT(index == READ_BUFFER_SIZE);
		UpdateGUI(m_scope);

		if (m_bReadBuffer) L(RawString(dataOUT));

		if (m_bSendWriteBuffer) {
			Send_I2C_WriteBuffer(deviceAddress);
			Sleep(1000);
			m_bSendWriteBuffer = FALSE;
		}

		if (bRead == FALSE) break;
	}
	return S_OK;
}

UINT CseDlg::I2C_Read(LPVOID pParam)
{
	CseDlg* pDlg = (CseDlg*)pParam;

	CComAuto ca(pDlg);

	if (pDlg->COM_OpenPort() != TRUE) goto cleanup;

	if (pDlg->SetPowerVoltage() != TRUE) goto cleanup;
	if (pDlg->PowerOn() != TRUE) goto cleanup;
	if (pDlg->SetProtocol() != TRUE) goto cleanup;
	if (pDlg->I2C_ResetBus() != TRUE) goto cleanup;

	//Sleep script for 100 milliseconds
	Sleep(100);

	if (pDlg->I2C_SetSpeed() != TRUE) goto cleanup;
	if (pDlg->I2C_GetSpeed() != TRUE) goto cleanup;

	if (pDlg->I2C_GetDeviceList() != TRUE) goto cleanup;

	pDlg->Read_I2C_SCB_Slave(pDlg->m_devices[0]);

cleanup:
	pDlg->ResetI2CReadButton();
	return 0;
}

void CseDlg::InitWriteBufferCombo(SCOPE& sc)
{
	m_scopeKindChangeNotiCombo.SetCurSel(sc.read.command.scopeKindChangeNotify);
	m_scopeOutChangeNotiCombo.SetCurSel(sc.read.command.scopeOutChangeNotify);
	m_scopeOperationModeCombo.SetCurSel(sc.read.command.scopeOperationMode);

	//읽기 버퍼가 없는 콤보들은 참고할 값이 없으므로 0으로 초기설정을 한다
	m_scopeStateKindCombo.SetCurSel(0);
	m_scopeStateBatteryCombo.SetCurSel(0);
	m_scopeStateIrCombo.SetCurSel(0);
	m_scopeStateEoCombo.SetCurSel(0);
	m_scopeStateOutCombo.SetCurSel(0);
}

void CseDlg::OnBnClickedI2cReadButton()
{
	if (bRead == FALSE) {
		bRead = TRUE;
		m_pReadThread = AfxBeginThread(I2C_Read, this);
		if (m_pReadThread == NULL) {
			L(_T("Read thread AfxBeginThread failed"));
			ResetI2CReadButton();
			return;
		}

		InitWriteBufferCombo(m_scope);

		GetDlgItem(IDC_I2C_READ_BUTTON)->SetWindowText(_T("Stop"));
		GetDlgItem(IDC_PORTS_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(TRUE);
		EnableCombos(TRUE);
	}
	else {
		ResetI2CReadButton();
	}
}

void CseDlg::OnBnClickedI2cWriteButton()
{
	m_bSendWriteBuffer = TRUE;
}


void CseDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	TRACE("UpdateData(FALSE)\n");

	CDialogEx::OnTimer(nIDEvent);
}


void CseDlg::OnCbnSelchangeScopeKindChangeNotiCombo()
{
	/*
	0x00 :알림 없음/알림 확인 완료
	0x04 : 조준경 -> 영상융합처리기 EO 변경 알림
	0x05 : 조준경 -> 영상융합처리기 IR 백상 변경 알림
	0x06 : 조준경 -> 영상융합처리기 IR 흑상 변경 알림
	*/

	int nSel = m_scopeKindChangeNotiCombo.GetCurSel();
	m_scope.write.command.scopeKindChangeNotify = nSel & 0xff;

	if (nSel > 0) m_scope.write.command.scopeKindChangeNotify += 3;

	CString str;
	str.Format(_T("%x"), m_scope.write.command.scopeKindChangeNotify);
	m_writeBufferListCtrl.SetItemText(0, 0, str);
}


void CseDlg::OnCbnSelchangeScopeOutChangeNotiCombo()
{
	/*
	0x00 :알림 없음/알림 확인 완료
	0x03 : 조준경 -> 영상융합처리기 영상 출력 변경 알림
	0x04 : 조준경 -> 영상융합처리기 영상 미출력 변경 알림
	*/
	int nSel = m_scopeOutChangeNotiCombo.GetCurSel();
	m_scope.write.command.scopeOutChangeNotify = nSel & 0xff;

	if (nSel > 0) m_scope.write.command.scopeOutChangeNotify += 2;

	CString str;
	str.Format(_T("%x"), m_scope.write.command.scopeOutChangeNotify);
	m_writeBufferListCtrl.SetItemText(0, 1, str);
}


void CseDlg::OnCbnSelchangeOpmodeScopeCombo()
{
	/*
	0x00 : (default)
	0x01 : 운용모드
	0x02 : 대기모드
	0x04 : 절전모드
	*/
	int nSel = m_scopeOperationModeCombo.GetCurSel();
	m_scope.write.command.scopeOperationMode = nSel & 0xff;

	if (nSel == 3) m_scope.write.command.scopeOperationMode = 4;

	CString str;
	str.Format(_T("%x"), m_scope.write.command.scopeOperationMode);
	m_writeBufferListCtrl.SetItemText(0, 2, str);
}


void CseDlg::OnCbnSelchangeScopeStateKindCombo()
{
	/*
	0x00 : 미정(규격에는 없으나 일관성을 위해 추가)
	0x01 : EO
	0x02 : IR 백상(Default)
	0x03 : IR 흑상
	*/
	int nSel = m_scopeStateKindCombo.GetCurSel();
	m_scope.write.state.kind = nSel & 0xff;

	CString str;
	str.Format(_T("%x"), m_scope.write.state.kind);
	m_writeBufferListCtrl.SetItemText(0, 3, str);
}


void CseDlg::OnCbnSelchangeScopeStateOutCombo()
{
	int nSel = m_scopeStateOutCombo.GetCurSel();
	m_scope.write.state.out = nSel & 0xff;

	CString str;
	str.Format(_T("%x"), m_scope.write.state.out);
	m_writeBufferListCtrl.SetItemText(0, 4, str);
}


void CseDlg::OnCbnSelchangeScopeStateBatteryCombo()
{
	int nSel = m_scopeStateBatteryCombo.GetCurSel();
	m_scope.write.state.battery = nSel & 0xff;

	CString str;
	str.Format(_T("%x"), m_scope.write.state.battery);
	m_writeBufferListCtrl.SetItemText(0, 5, str);
}


void CseDlg::OnCbnSelchangeScopeStateIrCombo()
{
	int nSel = m_scopeStateIrCombo.GetCurSel();
	m_scope.write.state.ir = nSel & 0xff;

	if (nSel == 1) m_scope.write.state.ir = 0xE1;	//0xE1 : 화기조준경 IR 모듈 이상

	CString str;
	str.Format(_T("%x"), m_scope.write.state.ir);
	m_writeBufferListCtrl.SetItemText(0, 6, str);
}


void CseDlg::OnCbnSelchangeScopeStateEoCombo()
{
	int nSel = m_scopeStateEoCombo.GetCurSel();
	m_scope.write.state.eo = nSel & 0xff;

	if (nSel == 1) m_scope.write.state.eo = 0xE2;	//0xE2 : 화기조준경 EO 모듈 이상

	CString str;
	str.Format(_T("%x"), m_scope.write.state.eo);
	m_writeBufferListCtrl.SetItemText(0, 7, str);
}


void CseDlg::OnCbnDropdownPortsCombo()
{
	FillPortsCombo();
}


void CseDlg::OnBnClickedI2cResetButton()
{
	ResetWriteBufferList();
}
