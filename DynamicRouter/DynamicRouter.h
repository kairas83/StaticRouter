// DynamicRouter.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDynamicRouterApp:
// �� Ŭ������ ������ ���ؼ��� DynamicRouter.cpp�� �����Ͻʽÿ�.
//

class CDynamicRouterApp : public CWinApp
{
public:
	CDynamicRouterApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CDynamicRouterApp theApp;