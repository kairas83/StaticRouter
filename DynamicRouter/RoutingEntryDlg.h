#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "DynamicRouterDlg.h"

// RoutingEntryDlg ��ȭ �����Դϴ�.
class RoutingEntryDlg : public CDialog
{
	DECLARE_DYNAMIC(RoutingEntryDlg)

public:
	RoutingEntryDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~RoutingEntryDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ROUTINGENTRYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_dstip;
	CIPAddressCtrl m_netmask;
	CIPAddressCtrl m_gateway;
	CButton m_G;
	CButton m_H;
	CButton m_U;
	CComboBox m_interface;
	afx_msg void OnBnClickedOk();

private:
	CDynamicRouterDlg*	RouterDlg;
public:
	void	setDlg(CDynamicRouterDlg* dlg);
	int		m_metric;
	virtual BOOL OnInitDialog();
};
