
// ziieDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ziie.h"
#include "ziieDlg.h"
#include "afxdialogex.h"
#include "com.h"
#include "util.h"
#include "version.h"
#include "git_describe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CZiieDlg::bRead = FALSE;

class CComAuto {
public:
	CComAuto(CZiieDlg* p) :m_p(p) { p->COM_Init(); }
	~CComAuto() { m_p->COM_UnInit(); }
private:
	CZiieDlg* m_p;
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


// CZiieDlg 대화 상자



CZiieDlg::CZiieDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZIIE_DIALOG, pParent), m_pCom(NULL), m_pReadThread(NULL)
	, m_strScopeKind(_T("0.종류:"))
	, m_strScopeOut(_T("1.출력:"))
	, m_strWirelessChannelMode(_T("2.모드:"))
	, m_strWirelessChannelInfo(_T("3.정보:"))
	, m_strOpmodeScope(_T("4.화기:"))
	, m_strOpmodeTx(_T("5.송신기:"))
	, m_strOpmodeRx(_T("6.수신기:"))
	, m_strTxImuType(_T("7.타입:"))
	, m_strTxImuCalib(_T("8.보정:"))
	, m_strRxImuType(_T("9.타입:"))
	, m_strRxImuCalib(_T("10.보정:"))
	, m_strScopeStateKind(_T("종류:"))
	, m_strScopeStateOut(_T("출력:"))
	, m_strScopeStateBattery(_T("배터리잔량:"))
	, m_strScopeStateIR(_T("IR상태:"))
	, m_strScopeStateEO(_T("EO상태:"))
	, m_strScopeDetect(_T("조준경:"))
	, m_strTxStateBattery(_T("배터리잔량:"))
	, m_strTxStateModem(_T("모뎀상태:"))
	, m_strTxStateImu(_T("IMU상태:"))
	, m_strRxStateModem(_T("모뎀상태:"))
	, m_strRxStateImu(_T("IMU상태:"))
	, m_strBleState(_T("BLE상태:"))
	, m_bReadBuffer(TRUE)
	, m_bWriteBuffer(TRUE)
	, m_bSendWriteBuffer(FALSE)
	, m_bSendPairingReset(FALSE)
	, m_strFwHbleVer(_T(""))
	, m_strFwDbleVer(_T(""))
	, m_strFwZxxVer(_T(""))
	, m_strFwZcdVer(_T(""))
	, m_zxxVerRawStr(_T("ZXX"))
	, m_strHbleBtAddress(_T(""))
	, m_strItfCriteriaValue(_T(""))
	, m_dwI2CWriteSleep(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZiieDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LIST, m_log);
	DDX_Control(pDX, IDC_ZCD_LIST, m_zcdListCtrl);
	DDX_Control(pDX, IDC_ZXX_LIST, m_zxxListCtrl);
	DDX_Control(pDX, IDC_DMU_LIST, m_dImuListCtrl);
	DDX_Control(pDX, IDC_HMU_LIST, m_hImuListCtrl);
	DDX_Control(pDX, IDC_PORTS_COMBO, m_portsCombo);
	DDX_Text(pDX, IDC_SCOPE_KIND_STATIC, m_strScopeKind);
	DDX_Text(pDX, IDC_SCOPE_OUT_STATIC, m_strScopeOut);
	DDX_Text(pDX, IDC_WIRELESS_CHANNEL_MODE_STATIC, m_strWirelessChannelMode);
	DDX_Text(pDX, IDC_WIRELESS_CHANNEL_INFO_STATIC, m_strWirelessChannelInfo);
	DDX_Text(pDX, IDC_OPMODE_SCOPE_STATIC, m_strOpmodeScope);
	DDX_Text(pDX, IDC_OPMODE_TX_STATIC, m_strOpmodeTx);
	DDX_Text(pDX, IDC_OPMODE_RX_STATIC, m_strOpmodeRx);
	DDX_Text(pDX, IDC_TX_IMU_TYPE_STATIC, m_strTxImuType);
	DDX_Text(pDX, IDC_TX_IMU_CALIB_STATIC, m_strTxImuCalib);
	DDX_Text(pDX, IDC_RX_IMU_TYPE_STATIC, m_strRxImuType);
	DDX_Text(pDX, IDC_RX_IMU_CALIB_STATIC, m_strRxImuCalib);
	DDX_Text(pDX, IDC_SCOPE_STATE_KIND_STATIC, m_strScopeStateKind);
	DDX_Text(pDX, IDC_SCOPE_STATE_OUT_STATIC, m_strScopeStateOut);
	DDX_Text(pDX, IDC_SCOPE_STATE_BATTERY_STATIC, m_strScopeStateBattery);
	DDX_Text(pDX, IDC_SCOPE_STATE_IR_STATIC, m_strScopeStateIR);
	DDX_Text(pDX, IDC_SCOPE_STATE_EO_STATIC, m_strScopeStateEO);
	DDX_Text(pDX, IDC_SCOPE_DETECT_STATIC, m_strScopeDetect);
	DDX_Text(pDX, IDC_TX_STATE_BATTERY_STATIC, m_strTxStateBattery);
	DDX_Text(pDX, IDC_TX_STATE_MODEM_STATIC, m_strTxStateModem);
	DDX_Text(pDX, IDC_TX_STATE_IMU_STATIC, m_strTxStateImu);
	DDX_Text(pDX, IDC_RX_STATE_MODEM_STATIC, m_strRxStateModem);
	DDX_Text(pDX, IDC_RX_STATE_IMU_STATIC, m_strRxStateImu);
	DDX_Text(pDX, IDC_BLE_STATE_STATIC, m_strBleState);
	DDX_Control(pDX, IDC_SCOPE_KIND_COMBO, m_scopeKindCombo);
	DDX_Control(pDX, IDC_SCOPE_OUT_COMBO, m_scopeOutCombo);
	DDX_Control(pDX, IDC_WIRELESS_CHANNEL_MODE_COMBO, m_wirelessChannelModeCombo);
	DDX_Control(pDX, IDC_WIRELESS_CHANNEL_INFO_COMBO, m_wirelessChannelInfoCombo);
	DDX_Control(pDX, IDC_OPMODE_SCOPE_COMBO, m_opmodeScopeCombo);
	DDX_Control(pDX, IDC_OPMODE_TX_COMBO, m_opmodeTxCombo);
	DDX_Control(pDX, IDC_OPMODE_RX_COMBO, m_opmodeRxCombo);
	DDX_Control(pDX, IDC_TX_IMU_TYPE_COMBO, m_txImuTypeCombo);
	DDX_Control(pDX, IDC_TX_IMU_CALIB_COMBO, m_txImuCalibCombo);
	DDX_Control(pDX, IDC_RX_IMU_TYPE_COMBO, m_rxImuTypeCombo);
	DDX_Control(pDX, IDC_RX_IMU_CALIB_COMBO, m_rxImuCalibCombo);
	DDX_Control(pDX, IDC_WRITE_BUFFER_LIST, m_writeBufferListCtrl);
	DDX_Control(pDX, IDC_BLE_STATUS_LED_STATIC, m_bleStateCtrl);
	DDX_Check(pDX, IDC_READ_BUFFER_CHECK, m_bReadBuffer);
	DDX_Check(pDX, IDC_WRITE_BUFFER_CHECK, m_bWriteBuffer);
	DDX_Text(pDX, IDC_HBLE_VERSION_STATIC, m_strFwHbleVer);
	DDX_Text(pDX, IDC_DBLE_VERSION_STATIC, m_strFwDbleVer);
	DDX_Text(pDX, IDC_ZXX_VERSION_STATIC, m_strFwZxxVer);
	DDX_Text(pDX, IDC_ZCD_VERSION_STATIC, m_strFwZcdVer);
	DDX_Control(pDX, IDC_ICD_LOG_LIST, m_icdLog);
	DDX_Control(pDX, IDC_ZXX_LOG_LIST, m_zxxLog);
	DDX_Control(pDX, IDC_ZCD_LOG_LIST, m_zcdLog);
	DDX_Control(pDX, IDC_DBLE_VER_LOG_LIST, m_dbleVerLog);
	DDX_Control(pDX, IDC_ZCD_VER_LOG_LIST, m_zcdVerLog);
	DDX_Control(pDX, IDC_HBLE_VER_LOG_LIST, m_hbleVerLog);
	DDX_Control(pDX, IDC_ZXX_VER_LOG_LIST, m_zxxVerLog);
	DDX_Text(pDX, IDC_ZXX_VERSION_RAW_STATIC, m_zxxVerRawStr);
	DDX_Text(pDX, IDC_HBLE_BT_ADDRESS_STATIC, m_strHbleBtAddress);
	DDX_Text(pDX, IDC_ITF_CRITERIA_VALUE_STATIC, m_strItfCriteriaValue);
	DDX_Control(pDX, IDC_ITF_CRITERIA_COMBO, m_itfCriteriaCombo);
	DDX_Control(pDX, IDC_I2C_WRITE_SLEEP_COMBO, m_i2cWriteSleepCombo);
}

BEGIN_MESSAGE_MAP(CZiieDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_I2C_READ_BUTTON, &CZiieDlg::OnBnClickedI2cReadButton)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_I2C_WRITE_BUTTON, &CZiieDlg::OnBnClickedI2cWriteButton)
	ON_CBN_SELCHANGE(IDC_SCOPE_KIND_COMBO, &CZiieDlg::OnSelchangeScopeKindCombo)
	ON_CBN_SELCHANGE(IDC_SCOPE_OUT_COMBO, &CZiieDlg::OnSelchangeScopeOutCombo)
	ON_CBN_SELCHANGE(IDC_WIRELESS_CHANNEL_MODE_COMBO, &CZiieDlg::OnSelchangeWirelessChannelModeCombo)
	ON_CBN_SELCHANGE(IDC_WIRELESS_CHANNEL_INFO_COMBO, &CZiieDlg::OnSelchangeWirelessChannelInfoCombo)
	ON_CBN_SELCHANGE(IDC_OPMODE_SCOPE_COMBO, &CZiieDlg::OnCbnSelchangeOpmodeScopeCombo)
	ON_CBN_SELCHANGE(IDC_OPMODE_TX_COMBO, &CZiieDlg::OnCbnSelchangeOpmodeTxCombo)
	ON_CBN_SELCHANGE(IDC_OPMODE_RX_COMBO, &CZiieDlg::OnCbnSelchangeOpmodeRxCombo)
	ON_CBN_SELCHANGE(IDC_TX_IMU_TYPE_COMBO, &CZiieDlg::OnCbnSelchangeTxImuTypeCombo)
	ON_CBN_SELCHANGE(IDC_TX_IMU_CALIB_COMBO, &CZiieDlg::OnCbnSelchangeTxImuCalibCombo)
	ON_CBN_SELCHANGE(IDC_RX_IMU_TYPE_COMBO, &CZiieDlg::OnCbnSelchangeRxImuTypeCombo)
	ON_CBN_SELCHANGE(IDC_RX_IMU_CALIB_COMBO, &CZiieDlg::OnCbnSelchangeRxImuCalibCombo)
	ON_BN_CLICKED(IDC_READ_BUFFER_CHECK, &CZiieDlg::OnBnClickedReadBufferCheck)
	ON_BN_CLICKED(IDC_WRITE_BUFFER_CHECK, &CZiieDlg::OnBnClickedWriteBufferCheck)
	ON_BN_CLICKED(IDC_RAW_CLEAR_BUTTON, &CZiieDlg::OnBnClickedRawClearButton)
	ON_CBN_DROPDOWN(IDC_PORTS_COMBO, &CZiieDlg::OnCbnDropdownPortsCombo)
	ON_BN_CLICKED(IDC_I2C_RESET_BUTTON, &CZiieDlg::OnBnClickedI2cResetButton)
	ON_BN_CLICKED(IDC_PAIRING_RESET_BUTTON, &CZiieDlg::OnBnClickedPairingResetButton)
	ON_CBN_SELCHANGE(IDC_ITF_CRITERIA_COMBO, &CZiieDlg::OnCbnSelchangeItfCriteriaCombo)
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

	m_bmpGreyCtrl.LoadBitmap(IDB_GREY_BITMAP);
	m_bmpGreenCtrl.LoadBitmap(IDB_GREEN_BITMAP);
	m_bmpYelloCtrl.LoadBitmap(IDB_YELLOW_BITMAP);
	m_bmpRedCtrl.LoadBitmap(IDB_RED_BITMAP);
	m_bleStateCtrl.ShowWindow(SW_HIDE);

	FillPortsCombo();
	CreateWriteBuffer();
	CreateColumnsIMU(m_hImuListCtrl);
	CreateColumnsIMU(m_dImuListCtrl);
	CreateColumnsZXX(m_zxxListCtrl);
	CreateColumnsZCD(m_zcdListCtrl);

	m_scopeKindCombo.AddString(_T("알림없음/알림반영"));
	m_scopeKindCombo.AddString(_T("조준경EO"));
	m_scopeKindCombo.AddString(_T("조준경IR 백상"));
	m_scopeKindCombo.AddString(_T("조준경IR 흑상"));

	m_scopeOutCombo.AddString(_T("알림없음/알림반영"));
	m_scopeOutCombo.AddString(_T("조준경 영상출력"));
	m_scopeOutCombo.AddString(_T("조준경 영상미출력"));

	m_wirelessChannelModeCombo.AddString(_T("자동(Default)"));
	m_wirelessChannelModeCombo.AddString(_T("수동"));

	m_wirelessChannelInfoCombo.AddString(_T("선택안함(Default)"));
	m_wirelessChannelInfoCombo.AddString(_T("수동1채널(L)"));
	m_wirelessChannelInfoCombo.AddString(_T("수동2채널(H)"));

	m_opmodeScopeCombo.AddString(_T("(default)"));
	m_opmodeScopeCombo.AddString(_T("운용모드"));
	m_opmodeScopeCombo.AddString(_T("대기모드"));
	m_opmodeScopeCombo.AddString(_T("절전모드"));

	m_opmodeTxCombo.AddString(_T("(default)"));
	m_opmodeTxCombo.AddString(_T("운용모드"));
	m_opmodeTxCombo.AddString(_T("대기모드"));
	m_opmodeTxCombo.AddString(_T("절전모드"));

	m_opmodeRxCombo.AddString(_T("(default)"));
	m_opmodeRxCombo.AddString(_T("운용모드"));
	m_opmodeRxCombo.AddString(_T("대기모드"));
	m_opmodeRxCombo.AddString(_T("절전모드"));

	m_txImuTypeCombo.AddString(_T("Euler Angle(Default)"));
	m_txImuTypeCombo.AddString(_T("Quaternion"));

	m_txImuCalibCombo.AddString(_T("(default)"));
	m_txImuCalibCombo.AddString(_T("자이로 보정"));
	m_txImuCalibCombo.AddString(_T("가속도 보정"));
	m_txImuCalibCombo.AddString(_T("지자계 보정"));
	m_txImuCalibCombo.AddString(_T("지자계 보정 종료"));
	m_txImuCalibCombo.AddString(_T("지자기Off"));
	m_txImuCalibCombo.AddString(_T("지자기On"));
	m_txImuCalibCombo.AddString(_T("지자기 능동형On"));

	m_rxImuTypeCombo.AddString(_T("Euler Angle(Default)"));
	m_rxImuTypeCombo.AddString(_T("Quaternion"));

	m_rxImuCalibCombo.AddString(_T("(default)"));
	m_rxImuCalibCombo.AddString(_T("자이로 보정"));
	m_rxImuCalibCombo.AddString(_T("가속도 보정"));
	m_rxImuCalibCombo.AddString(_T("지자계 보정"));
	m_rxImuCalibCombo.AddString(_T("지자계 보정 종료"));
	m_rxImuCalibCombo.AddString(_T("지자기Off"));
	m_rxImuCalibCombo.AddString(_T("지자기On"));
	m_rxImuCalibCombo.AddString(_T("지자기 능동형On"));

	for (int i = 1; i < 16; i++) {
		CString str;
		str.Format(_T("%d"), i);
		m_itfCriteriaCombo.AddString(str);
	}
	m_itfCriteriaCombo.SetCurSel(9);	//default 10

	int val = 0;
	for (int i = 0; i < 50; i++) {
		CString str;
		str.Format(_T("%d"), val);
		m_i2cWriteSleepCombo.AddString(str);
		val += 100;
	}
	m_i2cWriteSleepCombo.SetCurSel(0);

	GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(FALSE);
	EnableCombos(FALSE);

	SetTimer(1, 100, NULL);

	CString strDescribe(GIT_DESCRIBE);
	SetWindowText(_T("ziie ") + strDescribe);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CZiieDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CZiieDlg::EnableCombos(BOOL b)
{
	m_scopeKindCombo.EnableWindow(b);
	m_scopeOutCombo.EnableWindow(b);
	m_wirelessChannelModeCombo.EnableWindow(b);
	m_wirelessChannelInfoCombo.EnableWindow(b);
	m_opmodeScopeCombo.EnableWindow(b);
	m_opmodeTxCombo.EnableWindow(b);
	m_opmodeRxCombo.EnableWindow(b);
	m_txImuTypeCombo.EnableWindow(b);
	m_txImuCalibCombo.EnableWindow(b);
	m_rxImuTypeCombo.EnableWindow(b);
	m_rxImuCalibCombo.EnableWindow(b);
	m_itfCriteriaCombo.EnableWindow(b);
	m_i2cWriteSleepCombo.EnableWindow(b);
}

void CZiieDlg::FillPortsCombo()
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

void CZiieDlg::CreateWriteBuffer()
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
	m_writeBufferListCtrl.InsertColumn(8, _T("8"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(9, _T("9"), LVCFMT_RIGHT, nHexWidth);
	m_writeBufferListCtrl.InsertColumn(10, _T("10"), LVCFMT_RIGHT, nHexWidth);

	m_writeBufferListCtrl.InsertItem(0, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 0, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 1, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 2, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 3, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 4, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 5, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 6, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 7, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 8, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 9, _T(""));
	m_writeBufferListCtrl.SetItemText(0, 10, _T(""));
}

void CZiieDlg::CreateColumnsIMU(CListCtrl& listCtrl)
{
	listCtrl.InsertColumn(0, _T("IMU Data1"), LVCFMT_RIGHT, 70);
	listCtrl.InsertColumn(1, _T("IMU Data2"), LVCFMT_RIGHT, 70);
	listCtrl.InsertColumn(2, _T("IMU Data3"), LVCFMT_RIGHT, 70);
	listCtrl.InsertColumn(3, _T("IMU Data4"), LVCFMT_RIGHT, 70);
	listCtrl.InsertColumn(4, _T("IMU Data5"), LVCFMT_RIGHT, 70);
	listCtrl.InsertColumn(5, _T("Checksum"), LVCFMT_RIGHT, 70);
}

void CZiieDlg::CreateColumnsZXX(CListCtrl& listCtrl)
{
	listCtrl.InsertColumn(0, _T("Kind"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(1, _T("USB"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(2, _T("VND"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(3, _T("PRD"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(4, _T("BND"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(5, _T("PPID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(6, _T("DeviceID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(7, _T("TRT"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(8, _T("ACK"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(9, _T("PPC"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(10, _T("TXID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(11, _T("RXID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(12, _T("RUN"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(13, _T("CNT"), LVCFMT_RIGHT, 85);
}

void CZiieDlg::CreateColumnsZCD(CListCtrl& listCtrl)
{
	listCtrl.InsertColumn(0, _T("Kind"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(1, _T("USB"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(2, _T("PPID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(3, _T("DeviceID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(4, _T("FMT"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(5, _T("IDX"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(6, _T("FPS"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(7, _T("TRT"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(8, _T("ACK"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(9, _T("PPC"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(10, _T("RUN"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(11, _T("ITF"), LVCFMT_RIGHT, 35);
	listCtrl.InsertColumn(12, _T("TXID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(13, _T("RXID"), LVCFMT_RIGHT, 85);
	listCtrl.InsertColumn(14, _T("DestID_ERR_CNT"), LVCFMT_RIGHT, 120);
	listCtrl.InsertColumn(15, _T("PHY_RX_FRAME_CNT"), LVCFMT_RIGHT, 130);
	listCtrl.InsertColumn(16, _T("POS"), LVCFMT_RIGHT, 65);
	listCtrl.InsertColumn(17, _T("CNT"), LVCFMT_RIGHT, 85);
}

BOOL CZiieDlg::COM_Init(BOOL bSilent)
{
	if (!bSilent) L(_T("Initializing COM"));
	if (FAILED(CoInitialize(NULL)))
	{
		L(_T("Unable to initialize COM"));
		return FALSE;
	}
	if (!bSilent) L(_T("COM Initialized"));
	return TRUE;
}

void CZiieDlg::COM_UnInit(BOOL bSilent)
{
	CoUninitialize();
	if(!bSilent) L(_T("COM Uninitialized"));
}

void CZiieDlg::ResetI2CReadButton()
{
	bRead = FALSE;
	GetDlgItem(IDC_I2C_READ_BUTTON)->SetWindowText(_T("Read"));
	GetDlgItem(IDC_PORTS_COMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(FALSE);
	EnableCombos(FALSE);
	m_bleStateCtrl.ShowWindow(SW_HIDE);
}

BOOL CZiieDlg::COM_OpenPort()
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

	m_pCom->ClosePort();
	CoUninitialize();
	delete m_pCom;
}

BOOL CZiieDlg::SetPowerVoltage()
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

BOOL CZiieDlg::PowerOn()
{
	HRESULT hr = m_pCom->PowerOn();
	if (!SUCCEEDED(hr)) {
		L(_T("Power On error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Power On"));
	return TRUE;
}

BOOL CZiieDlg::SetProtocol()
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

BOOL CZiieDlg::I2C_ResetBus()
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

BOOL CZiieDlg::I2C_SetSpeed()
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

BOOL CZiieDlg::I2C_GetSpeed()
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

BOOL CZiieDlg::I2C_GetDeviceList()
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

HRESULT CZiieDlg::Control_I2C_SCB_Slave(int deviceAddress)
{
	HRESULT hr;
	std::vector<byte> dataIN;
	dataIN.resize(11);
	hr = m_pCom->writeI2C(deviceAddress, dataIN);
	if (!SUCCEEDED(hr)) {
		L(_T("Failed writeI2C,HRESULT: 0x%08X"), hr);
		return hr;
	}
	L(_T("Write %dbytes to device"), dataIN.size());

	Read_I2C_SCB_Slave(deviceAddress);

	return S_OK;
}

int CZiieDlg::InsertItem(CListCtrl& listCtrl, const CString& newItem)
{
	int itemCount = listCtrl.GetItemCount();
	if (itemCount >= MAX_LIST_ITEMS) listCtrl.DeleteItem(0);	// Remove the first item (FIFO behavior)

	int index = listCtrl.InsertItem(itemCount, newItem); // Add new item at the end

	// Ensure the new item is visible
	listCtrl.EnsureVisible(index, FALSE);
	return index;
}

CString CZiieDlg::DecStr(int val)
{
	CString str;
	str.Format(_T("%d"), val);
	return str;
}

CString CZiieDlg::HexStr(int val)
{
	CString str;
	str.Format(_T("%02X"), val);
	return str;
}

CString CZiieDlg::CharStr(char c)
{
	CString str;
	str.Format(_T("%c"), c);
	return str;
}

CString CZiieDlg::IntHexStr(int val)
{
	CString str;
	str.Format(_T("0x%0X"), val);
	return str;
}

BOOL CZiieDlg::IsNULL(ZING_BASE& z)
{
	if (z.kind != 0) return FALSE;
	if (z.usb != 0) return FALSE;
	if (z.bnd != 0x20 && z.bnd != 0x0) return FALSE;		//space or null
	if (z.ppid != 0) return FALSE;
	if (z.devid != 0) return FALSE;
	if (z.fmt != 0) return FALSE;
	if (z.idx != 0) return FALSE;
	if (z.trt != 0x20 && z.trt != 0x0) return FALSE;		//space or null
	if (z.ack != 0x20 && z.ack != 0x0) return FALSE;		//space or null
	if (z.ppc != 0x20 && z.ppc != 0x0) return FALSE;		//space or null
	if (z.run != 0x20 && z.run != 0x0) return FALSE;		//space or null
	if (z.txid != 0) return FALSE;
	if (z.rxid != 0) return FALSE;
	if (z.cnt != 0) return FALSE;
	return TRUE;
}

BOOL CZiieDlg::IsNULL(ZXX& z)
{
	if (z.prd != 0) return FALSE;
	if (z.vnd != 0) return FALSE;
	if (IsNULL(z.zb) != TRUE) return FALSE;
	return TRUE;
}

void CZiieDlg::UpdateZxxGUI(ZXX& z)
{
	int nItem = InsertItem(m_zxxListCtrl, _T("ZXX"));

	CString strNULL(_T("-")),strKind, strUSB, strBND,strPPID, strDeviceID, strFMT, strIDX, strTrt, strAck, strPpc, \
			strRun, strTxid, strRxid, strCnt, strPos, strVND, strPRD;

	if (IsNULL(z)) {
		strKind = strUSB = strBND = strPPID = strDeviceID = strFMT = strIDX = strTrt = strAck = strPpc \
			= strRun = strTxid = strRxid = strCnt = strPos = strVND = strPRD = strNULL;
	}
	else {
		strKind = DecStr(z.zb.kind);
		strUSB = DecStr(z.zb.usb);
		strBND = CharStr(z.zb.bnd);
		strPPID = IntHexStr(z.zb.ppid);
		strDeviceID = IntHexStr(z.zb.devid);
		strFMT = DecStr(z.zb.fmt);
		strIDX = DecStr(z.zb.idx);
		strTrt = CharStr(z.zb.trt);
		strAck = CharStr(z.zb.ack);
		strPpc = CharStr(z.zb.ppc);
		strRun = CharStr(z.zb.run);
		strTxid = IntHexStr(z.zb.txid);
		strRxid = IntHexStr(z.zb.rxid);
		strCnt = DecStr(z.zb.cnt);
		strPos = IntHexStr(z.zb.pos);
		strVND = IntHexStr(z.vnd);
		strPRD = IntHexStr(z.prd);
	}

	m_zxxListCtrl.SetItemText(nItem, 0, strKind);
	m_zxxListCtrl.SetItemText(nItem, 1, strUSB);
	m_zxxListCtrl.SetItemText(nItem, 4, strBND);
	m_zxxListCtrl.SetItemText(nItem, 5, strPPID);
	m_zxxListCtrl.SetItemText(nItem, 6, strDeviceID);
	m_zxxListCtrl.SetItemText(nItem, 7, strTrt);
	m_zxxListCtrl.SetItemText(nItem, 8, strAck);
	m_zxxListCtrl.SetItemText(nItem, 9, strPpc);
	m_zxxListCtrl.SetItemText(nItem, 10, strTxid);
	m_zxxListCtrl.SetItemText(nItem, 11, strRxid);
	m_zxxListCtrl.SetItemText(nItem, 12, strRun);
	m_zxxListCtrl.SetItemText(nItem, 13, strCnt);

	if (strKind == _T("1")) {	//ZED
		GetDlgItem(IDC_ZXX_STATIC)->SetWindowText(_T("ZED"));
		m_zxxListCtrl.SetItemText(nItem, 2, _T("-"));
		m_zxxListCtrl.SetItemText(nItem, 3, _T("-"));
	}
	else if (strKind == _T("2")) {	//ZCH
		GetDlgItem(IDC_ZXX_STATIC)->SetWindowText(_T("ZCH"));
		m_zxxListCtrl.SetItemText(nItem, 2, strVND);
		m_zxxListCtrl.SetItemText(nItem, 3, strPRD);
	}
	else {
		GetDlgItem(IDC_ZXX_STATIC)->SetWindowText(_T("ZXX"));
		m_zxxListCtrl.SetItemText(nItem, 2, _T(""));
		m_zxxListCtrl.SetItemText(nItem, 3, _T(""));
	}
	CString str;
	GetDlgItem(IDC_ZXX_STATIC)->GetWindowText(str);
	m_zxxVerRawStr = str;
}

BOOL CZiieDlg::IsNULL(ZCD& z)
{
	if (z.fps != 0) return FALSE;
	if (z.itf != 0x20 && z.itf != 0x0) return FALSE;	//space or null
	if (z.destIdErrCnt != 0) return FALSE;
	if (z.destIdDiff != 0) return FALSE;
	if (z.phyRxFrameCnt != 0) return FALSE;
	if (z.frameDiff != 0) return FALSE;
	if (IsNULL(z.zb) != TRUE) return FALSE;
	return TRUE;
}

void CZiieDlg::UpdateZcdGUI(ZCD& z)
{
	int nItem = InsertItem(m_zcdListCtrl, _T("ZCD"));

	CString strNULL(_T("-")), strKind, strUSB, strBND, strPPID, strDeviceID, strFMT, strIDX, strTrt, strAck, strPpc, \
		strRun, strTxid, strRxid, strCnt, strPos, strVND, strPRD, strFps, strItf, strDestErr, strDestDif, strPhyRx, strFrameDif;

	if (IsNULL(z)) {
		strKind = strUSB = strBND = strPPID = strDeviceID = strFMT = strIDX = strTrt = strAck = strPpc \
			= strRun = strTxid = strRxid = strCnt = strPos = strVND = strPRD = strFps = strItf = strDestErr \
			= strDestDif = strPhyRx = strFrameDif = strNULL;
	}
	else {
		strKind = DecStr(z.zb.kind);
		strUSB = DecStr(z.zb.usb);
		strBND = CharStr(z.zb.bnd);
		strPPID = IntHexStr(z.zb.ppid);
		strDeviceID = IntHexStr(z.zb.devid);
		strFMT = DecStr(z.zb.fmt);
		strIDX = DecStr(z.zb.idx);
		strTrt = CharStr(z.zb.trt);
		strAck = CharStr(z.zb.ack);
		strPpc = CharStr(z.zb.ppc);
		strRun = CharStr(z.zb.run);
		strTxid = IntHexStr(z.zb.txid);
		strRxid = IntHexStr(z.zb.rxid);
		strCnt = DecStr(z.zb.cnt);
		strPos = IntHexStr(z.zb.pos);
		strFps = IntHexStr(z.fps);
		strItf = CharStr(z.itf);
		strDestErr = DecStr(z.destIdErrCnt);
		strDestDif = DecStr(z.destIdDiff);
		strPhyRx = DecStr(z.phyRxFrameCnt);
		strFrameDif = DecStr(z.frameDiff);
	};

	m_zcdListCtrl.SetItemText(nItem, 0, strKind);
	m_zcdListCtrl.SetItemText(nItem, 1, strUSB);
	m_zcdListCtrl.SetItemText(nItem, 2, strPPID);
	m_zcdListCtrl.SetItemText(nItem, 3, strDeviceID);
	m_zcdListCtrl.SetItemText(nItem, 4, strFMT);
	m_zcdListCtrl.SetItemText(nItem, 5, strIDX);
	m_zcdListCtrl.SetItemText(nItem, 6, strFps);
	m_zcdListCtrl.SetItemText(nItem, 7, strTrt);
	m_zcdListCtrl.SetItemText(nItem, 8, strAck);
	m_zcdListCtrl.SetItemText(nItem, 9, strPpc);
	m_zcdListCtrl.SetItemText(nItem, 10, strRun);
	m_zcdListCtrl.SetItemText(nItem, 11, strItf);
	m_zcdListCtrl.SetItemText(nItem, 12, strTxid);
	m_zcdListCtrl.SetItemText(nItem, 13, strRxid);
	m_zcdListCtrl.SetItemText(nItem, 14, strDestErr);
	m_zcdListCtrl.SetItemText(nItem, 15, strPhyRx);
	m_zcdListCtrl.SetItemText(nItem, 16, strPos);
	m_zcdListCtrl.SetItemText(nItem, 17, strCnt);
}

void CZiieDlg::UpdateWriteBufferGUI(I2C_IVF_COMMAND& ic)
{
	m_writeBufferListCtrl.SetItemText(0, 0, HexStr(ic.ScopeKind));
	m_writeBufferListCtrl.SetItemText(0, 1, HexStr(ic.ScopeOut));
	m_writeBufferListCtrl.SetItemText(0, 2, HexStr(ic.WirelessChannelMode));
	m_writeBufferListCtrl.SetItemText(0, 3, HexStr(ic.WirelessChannelInfo));
	m_writeBufferListCtrl.SetItemText(0, 4, HexStr(ic.OpmodeScope));
	m_writeBufferListCtrl.SetItemText(0, 5, HexStr(ic.OpmodeTx));
	m_writeBufferListCtrl.SetItemText(0, 6, HexStr(ic.OpmodeRx));
	m_writeBufferListCtrl.SetItemText(0, 7, HexStr(ic.TxImuType));
	m_writeBufferListCtrl.SetItemText(0, 8, HexStr(ic.TxImuCalib));
	m_writeBufferListCtrl.SetItemText(0, 9, HexStr(ic.RxImuType));
	m_writeBufferListCtrl.SetItemText(0,10, HexStr(ic.RxImuCalib));
}

void CZiieDlg::UpdateScopeKind(byte kind)
{
	m_strScopeKind = _T("0.종류: ");

	CString str;
	switch (kind) {
	case 0:
		str = _T("(0)");
		break;
	case 4:
		str.Format(_T("EO(%x)"), kind);
		break;
	case 5:
		str.Format(_T("IR백상(%x)"), kind);
		break;
	case 6:
		str.Format(_T("IR흑상(%x)"), kind);
		break;
	default:
		str.Format(_T("미정의(%x)"), kind);
		break;
	}
	m_strScopeKind += str;
}

void CZiieDlg::UpdateScopeOut(byte out)
{
	m_strScopeOut = _T("1.출력: ");

	CString str;
	switch (out) {
	case 0:
		str = _T("(0)");
		break;
	case 3:
		str.Format(_T("출력(%x)"), out);
		break;
	case 4:
		str.Format(_T("미출력(%x)"), out);
		break;
	default:
		str.Format(_T("미정의(%x)"), out);
		break;
	}
	m_strScopeOut += str;
}

void CZiieDlg::UpdateScope(byte scopeKind, byte scopeOut)
{
	UpdateScopeKind(scopeKind);
	UpdateScopeOut(scopeOut);
}

void CZiieDlg::UpdateWirelessChannel(byte mode, byte info)
{
	switch (mode) {
	case 1:
		m_strWirelessChannelMode.Format(_T("2.모드: 자동(Default)(%x)"), mode);
		break;
	case 2:
		m_strWirelessChannelMode .Format(_T("2.모드: 수동(%x)"), mode);
		break;
	default:
		m_strWirelessChannelMode.Format(_T("2.모드: 미정의(%x)"), mode);
		break;
	}

	switch (info) {
	case 0:
		m_strWirelessChannelInfo.Format(_T("3.정보: 선택 안함(Default)(%x)"), info);
		break;
	case 1:
		m_strWirelessChannelInfo.Format(_T("3.정보: 수동1채널(L:%x)"), info);
		break;
	case 2:
		m_strWirelessChannelInfo.Format(_T("3.정보: 수동2채널(H:%x)"), info);
		break;
	default:
		m_strWirelessChannelInfo.Format(_T("3.정보: 미정의(%x)"), info);
		break;
	}
}

CString CZiieDlg::Opmode(byte mode)
{
	CString str;
	switch (mode) {
	case 0:
		str.Format(_T("(%x)"), mode);
		break;
	case 1:
		str.Format(_T("운용(%x)"), mode);
		break;
	case 2:
		str.Format(_T("대기(%x)"), mode);
		break;
	case 4:
		str.Format(_T("절전(%x)"), mode);
		break;
	default:
		str.Format(_T("미정의(%x)"), mode);
		break;
	}
	return str;
}

void CZiieDlg::UpdateOpmode(byte scope, byte tx, byte rx)
{
	m_strOpmodeScope = _T("4.화기:") + Opmode(scope);

	m_strOpmodeTx = _T("5.송신기:") + Opmode(tx);

	m_strOpmodeRx = _T("6.수신기:") + Opmode(rx);
}

void CZiieDlg::UpdateXIMU(BOOL bTx, CString& strImuType, CString& strCalib, byte type, byte calib)
{
	int refNum = 7;
	if (!bTx) refNum = 9;

	switch (type) {
	case 0:
		strImuType.Format(_T("%d.타입:*Euler(%x)"), refNum, type);
		break;
	case 1:
		strImuType.Format(_T("%d.타입:Quaternion(%x)"), refNum, type);
		break;
	default:
		strImuType.Format(_T("%d.타입:미정의(%x)"), refNum, type);
		break;
	}

	switch (calib)
	{
	case 0:
		strCalib.Format(_T("%d.보정:*(%x)"), refNum+1, calib);
		break;
	case 1:
		strCalib.Format(_T("%d.보정:자이로(%x)"), refNum+1, calib);
		break;
	case 2:
		strCalib.Format(_T("%d.보정:가속도(%x)"), refNum+1, calib);
		break;
	case 3:
		strCalib.Format(_T("%d.보정:지자계S(%x)"), refNum+1, calib);
		break;
	case 4:
		strCalib.Format(_T("%d.보정:지자계E(%x)"), refNum+1, calib);
		break;
	default:
		break;
	}
}

void CZiieDlg::UpdateScopeStateKind(byte kind)
{
	m_strScopeStateKind = _T("종류: ");

	CString str;
	switch (kind) {
	case 0:
		str.Format(_T("None(Default)(%x)"), kind);
		break;
	case 1:
		str.Format(_T("EO(%x)"), kind);
		break;
	case 2:
		str.Format(_T("IR백상(%x)"), kind);
		break;
	case 3:
		str.Format(_T("IR흑상(%x)"), kind);
		break;
	case 4:
		str.Format(_T("경II(%x)"), kind);
		break;
	default:
		str.Format(_T("미정의(%x)"), kind);
		break;
	}
	m_strScopeStateKind += str;
}

void CZiieDlg::UpdateScopeStateOut(byte out)
{
	m_strScopeStateOut = _T("출력: ");

	CString str;
	switch (out) {
	case 0:
		str.Format(_T("출력(%x)"), out);
		break;
	case 1:
		str.Format(_T("미출력(%x)"), out);
		break;
	default:
		str.Format(_T("미정의(%x)"), out);
		break;
	}
	m_strScopeStateOut += str;
}

void CZiieDlg::UpdateScopeDetect(byte det)
{
	m_strScopeDetect = _T("조준경: ");

	CString str;
	switch (det) {
	case 0:
		str.Format(_T("인식안됨(%x)"), det);
		break;
	case 1:
	case 2:
		str.Format(_T("인식성공(%x)"), det);
		break;
	default:
		str.Format(_T("미정의(%x)"), det);
		break;
	}
	m_strScopeDetect += str;
}

void CZiieDlg::BatteryInfo(CString& str, byte val)
{
	if (val < 0 || val>100) {
		str.Format(_T("배터리잔량: 범위밖(%d)"), val);
		return;
	}

	str.Format(_T("배터리잔량: %d%%"), val);
}

void CZiieDlg::UpdateScopeStateBattery(byte val)
{
	BatteryInfo(m_strScopeStateBattery, val);
}

CString CZiieDlg::ModuleSanity(CString strName, byte code, byte errCode)
{
	CString strResult(strName);

	CString str;
	if (code == 0) {
		str.Format(_T("정상(%x)"), code);
	}
	else if (code == errCode) {
		str.Format(_T("모듈이상(%x)"), code);
	}
	else {
		str.Format(_T("미정의(%x)"), code);
	}
	return strResult + str;
}

void CZiieDlg::UpdateScopeStateIR(byte code)
{
	/*
	0x00 : 정상
	0xE1 : 화기조준경 IR 모듈 이상
	0xff : unkown
	*/
	m_strScopeStateIR = ModuleSanity(_T("IR상태: "), code, 0xE1);
}

void CZiieDlg::UpdateScopeStateEO(byte code)
{
	/*
	0x00 : 정상
	0xE2 : 화기조준경 EO 모듈 이상
	0xff : unkown
	*/
	m_strScopeStateEO = ModuleSanity(_T("EO상태: "), code, 0xE2);
}

void CZiieDlg::UpdateScopeState(byte kind, byte out, byte det, byte val, byte irCode, byte eoCode)
{
	UpdateScopeStateKind(kind);
	UpdateScopeStateOut(out);
	UpdateScopeDetect(det);
	UpdateScopeStateBattery(val);
	UpdateScopeStateIR(irCode);
	UpdateScopeStateEO(eoCode);
}

void CZiieDlg::UpdateTxState(byte val, byte modemCode, byte imuCode)
{
	BatteryInfo(m_strTxStateBattery, val);
	m_strTxStateModem = ModuleSanity(_T("모뎀상태: "), modemCode, 0xE3);
	m_strTxStateImu = ModuleSanity(_T("IMU상태: "), imuCode, 0xE5);
}

void CZiieDlg::UpdateRxState(byte modemCode, byte imuCode)
{
	m_strRxStateModem = ModuleSanity(_T("모뎀상태: "), modemCode, 0xE4);
	m_strRxStateImu = ModuleSanity(_T("IMU상태: "), imuCode, 0xE6);
}

void CZiieDlg::UpdateBleState(byte dat)
{
	/*
	0x00 : 대기
	0x01 : 패어링 중
	0x02 : 패어링 완료
	*/

	m_strBleState = _T("BLE상태:");
	CString str;
	switch (dat)
	{
	case 0:
		str.Format(_T("대기(%x)"), dat);
		m_bleStateCtrl.SetBitmap(m_bmpGreyCtrl);
		break;
	case 1:
		str.Format(_T("페어링중(%x)"), dat);
		m_bleStateCtrl.SetBitmap(m_bmpYelloCtrl);
		break;
	case 2:
		str.Format(_T("페어링 완료(%x)"), dat);
		m_bleStateCtrl.SetBitmap(m_bmpGreenCtrl);
		break;
	default:
		str.Format(_T("미정의(%x)"), dat);
		m_bleStateCtrl.SetBitmap(m_bmpRedCtrl);
		break;
	}
	m_strBleState += str;
}

size_t CZiieDlg::ParseCommandData(std::vector<byte>& dataOUT, size_t index, I2C_IVF_COMMAND& ic)
{
	ic.ScopeKind = dataOUT[index++];
	ic.ScopeOut = dataOUT[index++];
	ic.WirelessChannelMode = dataOUT[index++];
	ic.WirelessChannelInfo = dataOUT[index++];
	ic.OpmodeScope = dataOUT[index++];
	ic.OpmodeTx = dataOUT[index++];
	ic.OpmodeRx = dataOUT[index++];
	ic.TxImuType = dataOUT[index++];
	ic.TxImuCalib = dataOUT[index++];
	ic.RxImuType = dataOUT[index++];
	ic.RxImuCalib = dataOUT[index++];
	return index;
}

void CZiieDlg::UpdateCommandGUI(I2C_IVF_COMMAND& ic)
{
	UpdateScope(ic.ScopeKind, ic.ScopeOut);
	UpdateWirelessChannel(ic.WirelessChannelMode, ic.WirelessChannelInfo);
	UpdateOpmode(ic.OpmodeScope, ic.OpmodeTx, ic.OpmodeRx);

	UpdateXIMU(TRUE, m_strTxImuType, m_strTxImuCalib, ic.TxImuType, ic.TxImuCalib);
	UpdateXIMU(FALSE, m_strRxImuType, m_strRxImuCalib, ic.RxImuType, ic.RxImuCalib);
}

size_t CZiieDlg::ParseStateData(std::vector<byte>& dataOUT, size_t index, I2C_STATE& is)
{
	is.ScopeStateKind = dataOUT[index++];
	is.ScopeStateOut = dataOUT[index++];
	is.TxStateLmscopeDetect = dataOUT[index++];
	is.ScopeStateBattery = dataOUT[index++];
	is.TxStateBattery = dataOUT[index++];
	is.ScopeStateIR = dataOUT[index++];
	is.ScopeStateEO = dataOUT[index++];
	is.TxStateModem = dataOUT[index++];
	is.RxStateModem = dataOUT[index++];
	is.TxStateImu = dataOUT[index++];
	is.RxStateImu = dataOUT[index++];
	is.BleState = dataOUT[index++];
	return index;
}

void CZiieDlg::UpdateStateGUI(I2C_STATE& is)
{
	if (is.BleState == 2) {
		UpdateScopeState(is.ScopeStateKind, is.ScopeStateOut, is.TxStateLmscopeDetect, is.ScopeStateBattery, is.ScopeStateIR, is.ScopeStateEO);
		UpdateTxState(is.TxStateBattery, is.TxStateModem, is.TxStateImu);
	}else{
		m_strScopeStateKind.Format(_T("종류: -(%x)"), is.ScopeStateKind);
		m_strScopeStateOut.Format(_T("출력: -(%x)"),is.ScopeStateOut);
		m_strScopeStateBattery.Format(_T("배터리잔량: -(%x)"),is.ScopeStateBattery);
		m_strScopeStateIR.Format(_T("IR상태: -(%x)"),is.ScopeStateIR);
		m_strScopeStateEO.Format(_T("EO상태: -(%x)"),is.ScopeStateEO);
		m_strScopeDetect.Format(_T("조준경: -(%x)"), is.TxStateLmscopeDetect);
		m_strTxStateBattery.Format(_T("배터리잔량: -(%x)"), is.TxStateBattery);
		m_strTxStateModem.Format(_T("모뎀상태: -(%x)"), is.TxStateModem);
		m_strTxStateImu.Format(_T("IMU상태: -(%x)"), is.TxStateImu);
	}
	UpdateRxState(is.RxStateModem, is.RxStateImu);
	UpdateBleState(is.BleState);
}

CString CZiieDlg::RawString(std::vector<byte>& dataOUT, size_t len)
{
	CString str;
	str.Format(_T("%Iu. "), len);
	for (size_t i = 0; i < len; i++) {
		CString tmp;
		tmp.Format(_T("%02X "), dataOUT[i]);
		str += tmp;
	}
	return str;
}

short CZiieDlg::ToShort(byte high, byte low)
{
	return (high << 8) | low;
}

size_t CZiieDlg::ParseImuData(std::vector<byte>& dataOUT, size_t index, IMU& imu)
{
	imu.data1 = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	imu.data2 = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	imu.data3 = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	imu.data4 = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	imu.data5 = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	imu.checksum = ToShort(dataOUT[index + 1], dataOUT[index]); index += 2;
	return index;
}

size_t CZiieDlg::ParseZxxData(std::vector<byte>& dataOUT, size_t index, ZXX& zxx)
{
	zxx.zb.kind = ToInt(dataOUT,index);		index += 4;
	zxx.zb.usb = ToInt(dataOUT,index);		index += 4;
	zxx.zb.bnd = dataOUT[index];			index += 1;
	zxx.zb.ppid = ToInt(dataOUT,index);		index += 4;
	zxx.zb.devid = ToInt(dataOUT,index);	index += 4;
	zxx.zb.fmt = ToInt(dataOUT,index);		index += 4;
	zxx.zb.idx = ToInt(dataOUT,index);		index += 4;
	zxx.zb.trt = dataOUT[index];			index += 1;
	zxx.zb.ack = dataOUT[index];			index += 1;
	zxx.zb.ppc = dataOUT[index];			index += 1;
	zxx.zb.run = dataOUT[index];			index += 1;
	zxx.zb.txid = ToInt(dataOUT,index);		index += 4;
	zxx.zb.rxid = ToInt(dataOUT,index);		index += 4;
	zxx.zb.cnt = ToInt(dataOUT,index);		index += 4;
	zxx.zb.pos = ToInt(dataOUT,index);		index += 4;

	zxx.vnd = ToInt(dataOUT,index);			index += 4;
	zxx.prd = ToInt(dataOUT,index);			index += 4;

	return index;
}

size_t CZiieDlg::ParseZcdData(std::vector<byte>& dataOUT, size_t index, ZCD& zcd)
{
	zcd.zb.kind = ToInt(dataOUT, index);		index += 4;
	zcd.zb.usb = ToInt(dataOUT, index);			index += 4;
	zcd.zb.bnd = dataOUT[index];				index += 1;
	zcd.zb.ppid = ToInt(dataOUT, index);		index += 4;
	zcd.zb.devid = ToInt(dataOUT, index);		index += 4;
	zcd.zb.fmt = ToInt(dataOUT, index);			index += 4;
	zcd.zb.idx = ToInt(dataOUT, index);			index += 4;
	zcd.zb.trt = dataOUT[index];				index += 1;
	zcd.zb.ack = dataOUT[index];				index += 1;
	zcd.zb.ppc = dataOUT[index];				index += 1;
	zcd.zb.run = dataOUT[index];				index += 1;
	zcd.zb.txid = ToInt(dataOUT, index);		index += 4;
	zcd.zb.rxid = ToInt(dataOUT, index);		index += 4;
	zcd.zb.cnt = ToInt(dataOUT, index);			index += 4;
	zcd.zb.pos = ToInt(dataOUT, index);			index += 4;

	zcd.fps = ToInt(dataOUT, index);			index += 4;
	zcd.itf = dataOUT[index];					index += 1;
	zcd.destIdErrCnt = ToInt(dataOUT, index);	index += 4;
	zcd.destIdDiff = ToInt(dataOUT, index);		index += 4;
	zcd.phyRxFrameCnt = ToInt(dataOUT, index);	index += 4;
	zcd.frameDiff = ToInt(dataOUT, index);		index += 4;

	return index;
}

size_t CZiieDlg::Parse_I2C(std::vector<byte>& dataOUT, IVF& ivf)
{
	size_t index = 0;
	index = ParseCommandData(dataOUT, index, ivf.read);
	ASSERT(index == ICD_COMMAND_SIZE);

	index = ParseStateData(dataOUT, index, ivf.state);
	ASSERT(index == ICD_SIZE);

	index = ParseImuData(dataOUT, index, ivf.txImu);
	ASSERT(index == IMU_RX_OFFSET);

	index = ParseImuData(dataOUT, index, ivf.rxImu);
	ASSERT(index == ICD_IVF_SIZE);

	index = ParseZxxData(dataOUT, index, ivf.zxx);
	ASSERT(index == (ICD_IVF_SIZE + ZING_ZXX_SIZE));

	index = ParseZcdData(dataOUT, index, ivf.zcd);
	ASSERT(index == (ICD_IVF_SIZE + ZING_ZXX_SIZE + ZING_ZCD_SIZE));

	return index;
}

CString CZiieDlg::ShortToStr(short val)
{
	CString str;
	byte high = (val & 0xff00) >> 8;
	byte low = val & 0xff;
	str.Format(_T("0x%02X%02X"), low, high);
	return str;
}

void CZiieDlg::UpdateImuGUI(CListCtrl& listCtrl, IMU& i)
{
	int nItem = InsertItem(listCtrl, _T(""));

	if (i.data1 == 0 && i.data2 == 0 && i.data3 == 0 && i.data4 == 0 && i.data5 == 0 && i.checksum == 0)
	{
		CString str(_T("-"));
		listCtrl.SetItemText(nItem, 0, str);
		listCtrl.SetItemText(nItem, 1, str);
		listCtrl.SetItemText(nItem, 2, str);
		listCtrl.SetItemText(nItem, 3, str);
		listCtrl.SetItemText(nItem, 4, str);
		listCtrl.SetItemText(nItem, 5, str);
		return;
	}

	listCtrl.SetItemText(nItem, 0, ShortToStr(i.data1));
	listCtrl.SetItemText(nItem, 1, ShortToStr(i.data2));
	listCtrl.SetItemText(nItem, 2, ShortToStr(i.data3));
	listCtrl.SetItemText(nItem, 3, ShortToStr(i.data4));
	listCtrl.SetItemText(nItem, 4, ShortToStr(i.data5));
	listCtrl.SetItemText(nItem, 5, ShortToStr(i.checksum));
}

void CZiieDlg::UpdateGUI(IVF& ivf)
{
	UpdateCommandGUI(ivf.read);
	UpdateStateGUI(ivf.state);
	UpdateImuGUI(m_hImuListCtrl, ivf.txImu);
	UpdateImuGUI(m_dImuListCtrl, ivf.rxImu);
	UpdateZxxGUI(ivf.zxx);
	UpdateZcdGUI(ivf.zcd);
	UpdateWriteBufferGUI(ivf.write);
}

HRESULT CZiieDlg::Send_I2C_Buffer(std::vector<byte> &dataIN, int deviceAddress)
{
	HRESULT hr = m_pCom->writeI2C(deviceAddress, dataIN);
	if (!SUCCEEDED(hr)) {
		L(_T("Failed writeI2C,HRESULT: 0x%08X"), hr);
		return hr;
	}

	if (m_bWriteBuffer) {
		I2C_IVF_COMMAND& i = m_ivf.write;
		L(_T("I2C Write Buffer[%d]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x"),
			sizeof(I2C_IVF_COMMAND), i.ScopeKind, i.ScopeOut, i.WirelessChannelMode, i.WirelessChannelInfo, i.OpmodeScope, i.OpmodeTx, i.OpmodeRx, i.TxImuType, i.TxImuCalib, i.RxImuType, i.RxImuCalib);
	}
	return S_OK;
}

HRESULT CZiieDlg::Send_I2C_WriteBuffer(int deviceAddress)
{
	std::vector<byte> dataIN;
	dataIN.resize(WRITE_BUFFER_SIZE);
	dataIN[0] = m_ivf.write.ScopeKind;
	dataIN[1] = m_ivf.write.ScopeOut;
	dataIN[2] = m_ivf.write.WirelessChannelMode;
	dataIN[3] = m_ivf.write.WirelessChannelInfo;
	dataIN[4] = m_ivf.write.OpmodeScope;
	dataIN[5] = m_ivf.write.OpmodeTx;
	dataIN[6] = m_ivf.write.OpmodeRx;
	dataIN[7] = m_ivf.write.TxImuType;
	dataIN[8] = m_ivf.write.TxImuCalib;
	dataIN[9] = m_ivf.write.RxImuType;
	dataIN[10] = m_ivf.write.RxImuCalib;

	return Send_I2C_Buffer(dataIN, deviceAddress);
}

HRESULT CZiieDlg::Send_ParingReset_Command(int deviceAddress)
{
	std::vector<byte> dataIN;
	dataIN.resize(WRITE_BUFFER_SIZE);
	dataIN[0] = 0x1F;
	return Send_I2C_Buffer(dataIN, deviceAddress);
}

BOOL CZiieDlg::AllValues(std::vector<byte> &dataOUT, byte value)
{
	bool all = true;
	for (size_t i = 0; i < dataOUT.size(); ++i) {
		if (dataOUT[i] != value) {
			all = false;
			break;
		}
	}
	return all;
}

void CZiieDlg::ResetWriteBufferList()
{
	memset(&m_ivf.write, 0, sizeof(I2C_IVF_COMMAND));
	CString str(_T("00"));
	for (int i = 0; i <= 10; i++) m_writeBufferListCtrl.SetItemText(0, i, str);
}

void CZiieDlg::ConvertVectorToCString(const std::vector<byte>& data, CString& result)
{
	// Ensure the vector is not empty
	if (data.empty()) {
		result = _T(""); // Assign an empty CString
		return;
	}

	// Convert ASCII data to wide characters for Unicode CString
	int length = static_cast<int>(data.size());
	std::vector<wchar_t> wideData(length);
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<const char*>(data.data()), length, wideData.data(), length);

	// Assign to CString
	result = CString(wideData.data(), length);
}

void CZiieDlg::GetBccVersion(std::vector<byte>& data)
{
	CString str;
	ConvertVectorToCString(data, str);

	m_strFwDbleVer = _T("DBLE: ");
	m_strFwDbleVer += str;
}

void CZiieDlg::GetZcdVersion(std::vector<byte>& data)
{
	CString str;
	ConvertVectorToCString(data, str);

	m_strFwZcdVer = _T("ZCD: ");
	m_strFwZcdVer += str;
}

void CZiieDlg::GetBpsVersion(std::vector<byte>& data)
{
	CString str;
	ConvertVectorToCString(data, str);

	m_strFwHbleVer = _T("HBLE: ");
	m_strFwHbleVer += str;
}

void CZiieDlg::GetZxxVersion(std::vector<byte>& data)
{
	CString str;
	ConvertVectorToCString(data, str);

    CString strZxx;
    GetDlgItemText(IDC_ZXX_STATIC, strZxx);

	m_strFwZxxVer = strZxx + _T(": ");
	m_strFwZxxVer += str;
}

void CZiieDlg::GetBtAddress(std::vector<byte>& data)
{
	m_strHbleBtAddress.Empty();
	if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0 && data[4] == 0 && data[5] == 0) return;

	m_strHbleBtAddress.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"), data[5], data[4], data[3], data[2], data[1], data[0]);

	if (data[6] == 0x0)
		m_strHbleBtAddress += _T(" public");
	else
		m_strHbleBtAddress += _T(" random");
}

void CZiieDlg::GetItfCriteria(std::vector<byte>& data)
{
	m_itfCriteria = data[0];
	m_strItfCriteriaValue.Format(_T("간섭민감도: %d"), m_itfCriteria);
}

HRESULT CZiieDlg::Read_I2C_SCB_Slave(int deviceAddress)
{
	HRESULT hr;
	CString str;
	std::vector<byte> dataOUT;
	size_t index;
	while (1) {
		hr = m_pCom->readI2C(deviceAddress, READ_BUFFER_SIZE + 4 * VERSION_SIZE + BT_ADDRESS_SIZE + ITF_CRITERIA_SIZE, dataOUT);
		if (!SUCCEEDED(hr)) {
			L(_T("Failed readI2C,HRESULT: 0x%08X"), hr);
			return hr;
		}

		std::vector<byte> bccVer(dataOUT.begin() + READ_BUFFER_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + VERSION_SIZE);
		GetBccVersion(bccVer);
		std::vector<byte> zcdVer(dataOUT.begin() + READ_BUFFER_SIZE + VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 2 * VERSION_SIZE);
		GetZcdVersion(zcdVer);
		std::vector<byte> bpsVer(dataOUT.begin() + READ_BUFFER_SIZE + 2 * VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 3 * VERSION_SIZE);
		GetBpsVersion(bpsVer);
		std::vector<byte> zxxVer(dataOUT.begin() + READ_BUFFER_SIZE + 3 * VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE);
		GetZxxVersion(zxxVer);
		std::vector<byte> btAddr(dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE + BT_ADDRESS_SIZE);
		GetBtAddress(btAddr);
		std::vector<byte> itfCriteia(dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE + BT_ADDRESS_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE + BT_ADDRESS_SIZE + ITF_CRITERIA_SIZE);
		GetItfCriteria(itfCriteia);

		if (!AllValues(dataOUT, 0xFF)) {
			index = Parse_I2C(dataOUT, m_ivf);
			ASSERT(index == READ_BUFFER_SIZE);
			UpdateGUI(m_ivf);
		}

		if (m_bReadBuffer) {
			L(RawString(dataOUT, READ_BUFFER_SIZE));

			m_icdLog.AddString(RawString(dataOUT, ICD_IVF_SIZE));	//ICD_IVF_SIZE
			m_icdLog.SetTopIndex(m_icdLog.GetCount() - 1);

			std::vector<byte> zxx(dataOUT.begin() + ICD_IVF_SIZE, dataOUT.begin() + ICD_IVF_SIZE + ZING_ZXX_SIZE);	//ZING_ZXX_SIZE
			m_zxxLog.AddString(RawString(zxx, ZING_ZXX_SIZE));
			m_zxxLog.SetTopIndex(m_zxxLog.GetCount() - 1);

			std::vector<byte> zcd(dataOUT.begin() + ICD_IVF_SIZE + ZING_ZXX_SIZE, dataOUT.begin() + ICD_IVF_SIZE + ZING_ZXX_SIZE + ZING_ZCD_SIZE);	//ZING_ZCD_SIZE
			m_zcdLog.AddString(RawString(zcd, ZING_ZCD_SIZE));
			m_zcdLog.SetTopIndex(m_zcdLog.GetCount() - 1);

			std::vector<byte> dblev(dataOUT.begin() + READ_BUFFER_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + VERSION_SIZE);
			m_dbleVerLog.AddString(RawString(dblev, VERSION_SIZE));
			m_dbleVerLog.SetTopIndex(m_dbleVerLog.GetCount() - 1);

			std::vector<byte> zcdv(dataOUT.begin() + READ_BUFFER_SIZE + VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 2 * VERSION_SIZE);
			m_zcdVerLog.AddString(RawString(zcdv, VERSION_SIZE));
			m_zcdVerLog.SetTopIndex(m_zcdVerLog.GetCount() - 1);

			std::vector<byte> hblev(dataOUT.begin() + READ_BUFFER_SIZE + 2 * VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 3 * VERSION_SIZE);
			m_hbleVerLog.AddString(RawString(hblev, VERSION_SIZE));
			m_hbleVerLog.SetTopIndex(m_hbleVerLog.GetCount() - 1);

			std::vector<byte> zxxv(dataOUT.begin() + READ_BUFFER_SIZE + 3 * VERSION_SIZE, dataOUT.begin() + READ_BUFFER_SIZE + 4 * VERSION_SIZE);
			m_zxxVerLog.AddString(RawString(zxxv, VERSION_SIZE));
			m_zxxVerLog.SetTopIndex(m_zxxVerLog.GetCount() - 1);
		}

		if (m_bSendWriteBuffer) {
			Send_I2C_WriteBuffer(deviceAddress);
			L(_T("Waiting..."));
			Sleep(m_dwI2CWriteSleep);
			m_bSendWriteBuffer = FALSE;
		}

		if (m_bSendPairingReset) {
			Send_ParingReset_Command(deviceAddress);
			L(_T("Pairing Reset..."));
			m_bSendPairingReset = FALSE;
		}

		if (bRead == FALSE) break;
	}
	return S_OK;
}

UINT CZiieDlg::I2C_Read(LPVOID pParam)
{
	CZiieDlg* pDlg = (CZiieDlg*)pParam;

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

void CZiieDlg::InitWriteBufferCombo(I2C_IVF_COMMAND& ic)
{
	m_scopeKindCombo.SetCurSel(ic.ScopeKind);
	m_scopeOutCombo.SetCurSel(ic.ScopeOut);
	m_wirelessChannelModeCombo.SetCurSel(ic.WirelessChannelMode);
	m_wirelessChannelInfoCombo.SetCurSel(ic.WirelessChannelInfo);
	m_opmodeScopeCombo.SetCurSel(ic.OpmodeScope);
	m_opmodeTxCombo.SetCurSel(ic.OpmodeTx);
	m_opmodeRxCombo.SetCurSel(ic.OpmodeRx);
	m_txImuTypeCombo.SetCurSel(ic.TxImuType);
	m_txImuCalibCombo.SetCurSel(ic.TxImuCalib);
	m_rxImuTypeCombo.SetCurSel(ic.RxImuType);
	m_rxImuCalibCombo.SetCurSel(ic.RxImuCalib);
}

void CZiieDlg::OnBnClickedI2cReadButton()
{
	if (bRead == FALSE) {
		bRead = TRUE;
		m_pReadThread = AfxBeginThread(I2C_Read, this);
		if (m_pReadThread == NULL) {
			L(_T("Read thread AfxBeginThread failed"));
			ResetI2CReadButton();
			return;
		}

		InitWriteBufferCombo(m_ivf.read);

		GetDlgItem(IDC_I2C_READ_BUTTON)->SetWindowText(_T("Stop"));
		GetDlgItem(IDC_PORTS_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_I2C_RESET_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_I2C_WRITE_BUTTON)->EnableWindow(TRUE);
		EnableCombos(TRUE);
		m_bleStateCtrl.ShowWindow(SW_SHOW);
	}
	else {
		ResetI2CReadButton();
	}
}


void CZiieDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	TRACE("UpdateData(FALSE)\n");

	CDialogEx::OnTimer(nIDEvent);
}


void CZiieDlg::OnBnClickedI2cWriteButton()
{
	m_bSendWriteBuffer = TRUE;
}


void CZiieDlg::OnSelchangeScopeKindCombo()
{
	int nSel = m_scopeKindCombo.GetCurSel();
	byte high = m_ivf.write.ScopeKind & 0xf0;       //상위4비트 추출
	m_ivf.write.ScopeKind = high | (nSel & 0xf);    //이전의 상위1비트에 콤보에서 읽은 하위4비트 결합

	CString str;
	str.Format(_T("%02X"), m_ivf.write.ScopeKind);
	m_writeBufferListCtrl.SetItemText(0, 0, str);
}


void CZiieDlg::OnSelchangeScopeOutCombo()
{
	int nSel = m_scopeOutCombo.GetCurSel();
	m_ivf.write.ScopeOut = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.ScopeOut);
	m_writeBufferListCtrl.SetItemText(0, 1, str);
}


void CZiieDlg::OnSelchangeWirelessChannelModeCombo()
{
	int nSel = m_wirelessChannelModeCombo.GetCurSel() + 1;

	m_ivf.write.WirelessChannelMode = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.WirelessChannelMode);
	m_writeBufferListCtrl.SetItemText(0, 2, str);
}


void CZiieDlg::OnSelchangeWirelessChannelInfoCombo()
{
	int nSel = m_wirelessChannelInfoCombo.GetCurSel();	//0x00 : 선택 안함(Default) 0x01 : 수동 1채널 0x02 : 수동 2채널

	m_ivf.write.WirelessChannelInfo = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.WirelessChannelInfo);
	m_writeBufferListCtrl.SetItemText(0, 3, str);
}


void CZiieDlg::OnCbnSelchangeOpmodeScopeCombo()
{
	int nSel = m_opmodeScopeCombo.GetCurSel();	//0x00 : (default) 0x01 : 운용모드 0x02 : 대기모드 0x04 : 절전모드
	
	m_ivf.write.OpmodeScope = nSel & 0xff;
	if (nSel == 3) m_ivf.write.OpmodeScope++;	//세번째 선택은 0x04 절전모드로 정의되어 값을 증가 시킴

	CString str;
	str.Format(_T("%02X"), m_ivf.write.OpmodeScope);
	m_writeBufferListCtrl.SetItemText(0, 4, str);
}


void CZiieDlg::OnCbnSelchangeOpmodeTxCombo()
{
	int nSel = m_opmodeTxCombo.GetCurSel();	//0x00 : (default) 0x01 : 운용모드 0x02 : 대기모드 0x04 : 절전모드

	m_ivf.write.OpmodeTx = nSel & 0xff;
	if (nSel == 3) m_ivf.write.OpmodeTx++;	//세번째 선택은 0x04 절전모드로 정의되어 값을 증가 시킴

	CString str;
	str.Format(_T("%02X"), m_ivf.write.OpmodeTx);
	m_writeBufferListCtrl.SetItemText(0, 5, str);
}


void CZiieDlg::OnCbnSelchangeOpmodeRxCombo()
{
	int nSel = m_opmodeRxCombo.GetCurSel();	//0x00 : (default) 0x01 : 운용모드 0x02 : 대기모드 0x04 : 절전모드

	m_ivf.write.OpmodeRx = nSel & 0xff;
	if (nSel == 3) m_ivf.write.OpmodeRx++;	//세번째 선택은 0x04 절전모드로 정의되어 값을 증가 시킴

	CString str;
	str.Format(_T("%02X"), m_ivf.write.OpmodeRx);
	m_writeBufferListCtrl.SetItemText(0, 6, str);
}


void CZiieDlg::OnCbnSelchangeTxImuTypeCombo()
{
	int nSel = m_txImuTypeCombo.GetCurSel();	//0x00 : Euler Angle (Default) 0x01 : Quaternion

	m_ivf.write.TxImuType = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.TxImuType);
	m_writeBufferListCtrl.SetItemText(0, 7, str);
}


void CZiieDlg::OnCbnSelchangeTxImuCalibCombo()
{
	int nSel = m_txImuCalibCombo.GetCurSel();	//0x00 : (default) 0x01 : 자이로 보정 0x02 : 가속도 보정 0x03 : 지자계 보정 0x04 : 지자계 보정 종료 0x05 : 지자기Off 0x06 : 지자기On 0x07 : 지자기 능동형On

	m_ivf.write.TxImuCalib = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.TxImuCalib);
	m_writeBufferListCtrl.SetItemText(0, 8, str);
}


void CZiieDlg::OnCbnSelchangeRxImuTypeCombo()
{
	int nSel = m_rxImuTypeCombo.GetCurSel();	//0x00 : Euler Angle (Default) 0x01 : Quaternion

	m_ivf.write.RxImuType = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.RxImuType);
	m_writeBufferListCtrl.SetItemText(0, 9, str);
}


void CZiieDlg::OnCbnSelchangeRxImuCalibCombo()
{
	int nSel = m_rxImuCalibCombo.GetCurSel();	//0x00 : (default) 0x01 : 자이로 보정 0x02 : 가속도 보정 0x03 : 지자계 보정 0x04 : 지자계 보정 종료 0x05 : 지자기Off 0x06 : 지자기On 0x07 : 지자기 능동형On

	m_ivf.write.RxImuCalib = nSel & 0xff;

	CString str;
	str.Format(_T("%02X"), m_ivf.write.RxImuCalib);
	m_writeBufferListCtrl.SetItemText(0, 10, str);
}


void CZiieDlg::OnBnClickedReadBufferCheck()
{
	m_bReadBuffer = !m_bReadBuffer;
	UpdateData(FALSE);
}


void CZiieDlg::OnBnClickedWriteBufferCheck()
{
	m_bWriteBuffer = !m_bWriteBuffer;
	UpdateData(FALSE);
}


void CZiieDlg::OnBnClickedRawClearButton()
{
	m_log.ResetContent();
}


void CZiieDlg::OnCbnDropdownPortsCombo()
{
	FillPortsCombo();
}


void CZiieDlg::OnBnClickedI2cResetButton()
{
	ResetWriteBufferList();
}

void CZiieDlg::OnBnClickedPairingResetButton()
{
	m_bSendPairingReset = TRUE;
}

void CZiieDlg::OnCbnSelchangeItfCriteriaCombo()
{
	int nVal = m_itfCriteriaCombo.GetCurSel() + 1;

	if (nVal == m_itfCriteria) {
		byte low = m_ivf.write.ScopeKind & 0x0f;    //Bit[5] SET_ITF_CRITERIA=0
		m_ivf.write.ScopeKind = low;
		m_ivf.write.ScopeOut = m_ivf.write.ScopeOut & 0x0f;
	}
	else {
		byte high = 0x2 << 4;    //Bit[5] SET_ITF_CRITERIA=1
		byte low = m_ivf.write.ScopeKind & 0x0f;
		m_ivf.write.ScopeKind = high | low;

		low = m_ivf.write.ScopeOut & 0x0f;
		high = nVal << 4;
		m_ivf.write.ScopeOut = high | low;
	}

	CString str;
	str.Format(_T("%02X"), m_ivf.write.ScopeKind);
	m_writeBufferListCtrl.SetItemText(0, 0, str);

	str.Format(_T("%02X"), m_ivf.write.ScopeOut);
	m_writeBufferListCtrl.SetItemText(0, 1, str);
}
