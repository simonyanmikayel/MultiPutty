#include "pch.h"
#include <Objbase.h>
#include "dia2.h"
#define stdlog Log

bool LoadDataFromPdb()
{
  IDiaDataSource *m_pDiaDataSource;
  IDiaSession *m_pDiaSession;
  IDiaSymbol *m_pGlobalSymbol;
  DWORD m_dwMachineType;

  wchar_t wszExt[MAX_PATH];
  wchar_t *wszSearchPath = L"SRV**\\\\symbols\\symbols"; // Alternate path to search for debug data
  DWORD dwMachType = 0;
  HRESULT hr;
  const wchar_t* szFilename = L"c:\\_temp_vs\\out\\ExtraReader\\ExtraReader\\x64\\Debug\\Appx\\Reader.dll";
  hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

  // Obtain access to the provider

  hr = CoCreateInstance(__uuidof(DiaSource),
    NULL,
    CLSCTX_INPROC_SERVER,
    __uuidof(IDiaDataSource),
    (void **)(&m_pDiaDataSource));

  if (FAILED(hr)) {
    stdlog("CoCreateInstance failed - HRESULT = %08X\n", hr);

    return false;
  }

  _wsplitpath_s(szFilename, NULL, 0, NULL, 0, NULL, 0, wszExt, MAX_PATH);

  if (!_wcsicmp(wszExt, L".pdb")) {
    // Open and prepare a program database (.pdb) file as a debug data source

    hr = (m_pDiaDataSource)->loadDataFromPdb(szFilename);

    if (FAILED(hr)) {
      stdlog("loadDataFromPdb failed - HRESULT = %08X\n", hr);

      return false;
    }
  }

  else {
    //CCallback callback; // Receives callbacks from the DIA symbol locating procedure,
    //                    // thus enabling a user interface to report on the progress of
    //                    // the location attempt. The client application may optionally
    //                    // provide a reference to its own implementation of this
    //                    // virtual base class to the IDiaDataSource::loadDataForExe method.
    //callback.AddRef();

    //// Open and prepare the debug data associated with the executable

    //hr = (m_pDiaDataSource)->loadDataForExe(szFilename, wszSearchPath, &callback);

    if (FAILED(hr)) {
      stdlog("loadDataForExe failed - HRESULT = %08X\n", hr);

      return false;
    }
  }

  // Open a session for querying symbols

  hr = (m_pDiaDataSource)->openSession(&m_pDiaSession);

  if (FAILED(hr)) {
    stdlog("openSession failed - HRESULT = %08X\n", hr);

    return false;
  }

  // Retrieve a reference to the global scope

  hr = (m_pDiaSession)->get_globalScope(&m_pGlobalSymbol);

  if (hr != S_OK) {
    stdlog("get_globalScope failed\n");

    return false;
  }

  // Set Machine type for getting correct register names

  if ((m_pGlobalSymbol)->get_machineType(&dwMachType) == S_OK) {
    switch (dwMachType) {
    case IMAGE_FILE_MACHINE_I386: m_dwMachineType = CV_CFL_80386; break;
    case IMAGE_FILE_MACHINE_IA64: m_dwMachineType = CV_CFL_IA64; break;
    case IMAGE_FILE_MACHINE_AMD64: m_dwMachineType = CV_CFL_AMD64; break;
    }
  }

  return true;
}

