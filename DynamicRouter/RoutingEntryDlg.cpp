// RoutingEntryDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DynamicRouter.h"
#include "RoutingEntryDlg.h"

RoutingEntryDlg EntryDlg;
// RoutingEntryDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(RoutingEntryDlg, CDialog)

RoutingEntryDlg::RoutingEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RoutingEntryDlg::IDD, pParent)
	, m_metric(0)
{

}

RoutingEntryDlg::~RoutingEntryDlg()
{
}

void RoutingEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_dstip);
	DDX_Control(pDX, IDC_IPADDRESS2, m_netmask);
	DDX_Control(pDX, IDC_IPADDRESS3, m_gateway);
	DDX_Control(pDX, IDC_CHECK2, m_G);
	DDX_Control(pDX, IDC_CHECK3, m_H);
	DDX_Control(pDX, IDC_CHECK1, m_U);
	DDX_Control(pDX, IDC_COMBO1, m_interface);
	DDX_Text(pDX, IDC_EDIT1, m_metric);
}


BEGIN_MESSAGE_MAP(RoutingEntryDlg, CDialog)
	ON_BN_CLICKED(IDOK, &RoutingEntryDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL RoutingEntryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	pcap_if_t *alldevs;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs(&alldevs,errbuf);
	for(;alldevs;alldevs = alldevs->next)
		m_interface.AddString(alldevs->name);
	m_netmask.SetWindowTextA("255.255.255.0");
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
// RoutingEntryDlg �޽��� ó�����Դϴ�.

void RoutingEntryDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(true);
	unsigned char dstip[4];
	unsigned char Netmask[4];
	unsigned char Gateway[4];
	unsigned char dev_name[100];
	int flag = 0;
	if(m_U.GetCheck())
		flag += 1;
	if(m_G.GetCheck())
		flag += 2;
	if(m_H.GetCheck())
		flag += 4;
	m_dstip.GetAddress(dstip[0],dstip[1],dstip[2],dstip[3]);
	m_netmask.GetAddress(Netmask[0],Netmask[1],Netmask[2],Netmask[3]);
	m_gateway.GetAddress(Gateway[0],Gateway[1],Gateway[2],Gateway[3]);
	m_interface.GetLBText(m_interface.GetCurSel(),(LPTSTR)(LPCTSTR)dev_name);

	RouterDlg->addrouting_entry(dstip,Netmask,Gateway,flag,dev_name,m_metric);
	OnOK();
}
void RoutingEntryDlg::setDlg(CDynamicRouterDlg* dlg)
{
	RouterDlg = dlg;
}