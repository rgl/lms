/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2009-2023 Intel Corporation
 */
/*++

@file: UNSAlert.h

--*/

// UNSAlert.h : Declaration of the CUNSAlert

#pragma once
#include "resource.h"       // main symbols

#include "AMT_COM_Interface.h"
#include "_IUNSAlertEvents_CP.h"
#include "GmsService.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CUNSAlert

class ATL_NO_VTABLE CUNSAlert :
	public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>,
	public ATL::CComCoClass<CUNSAlert, &CLSID_UNSAlert>,
	public ATL::IConnectionPointContainerImpl<CUNSAlert>,
	public CProxy_IUNSAlertEvents<CUNSAlert>,
	public ATL::IDispatchImpl<IUNSAlert, &IID_IUNSAlert, &LIBID_AMT_COM_InterfaceLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CUNSAlert()
	{
		theService::instance()->inherit_log_msg_attributes();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_UNSALERT)


BEGIN_COM_MAP(CUNSAlert)
	COM_INTERFACE_ENTRY(IUNSAlert)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CUNSAlert)
	CONNECTION_POINT_ENTRY(__uuidof(_IUNSAlertEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_CLASSFACTORY_SINGLETON(CUNSAlert)
	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(RiseAlert)(USHORT category,
				ULONG id,
				BSTR message,
				BSTR messageArg,
				BSTR messageID, 
				BSTR dateTime);

	STDMETHOD(GetIMSSEventHistory)(BSTR* bstrEventHistory);
	
	STDMETHOD (ResetUNSstartedEvent)();
};

OBJECT_ENTRY_AUTO(__uuidof(UNSAlert), CUNSAlert)
