//-----------------------------------------------------------------------------
// File: GetDXVer.cpp
//
// Desc: Demonstrates how applications can detect what version of DirectX
//       is installed.
//
// (C) Copyright Microsoft Corp.  All rights reserved.
//-----------------------------------------------------------------------------

#define INITGUID
#include <first_header.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dxdiag.h>

static HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
static HRESULT GetDirectXVerionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
static HRESULT GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion );
static ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d );
static int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 );

//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version.
// Returns: 
//              0x00000000 = No DirectX installed
//              0x00010000 = DirectX 1.0 installed
//              0x00020000 = DirectX 2.0 installed
//              0x00030000 = DirectX 3.0 installed
//              0x00030001 = DirectX 3.0a installed
//              0x00050000 = DirectX 5.0 installed
//              0x00060000 = DirectX 6.0 installed
//              0x00060100 = DirectX 6.1 installed
//              0x00060101 = DirectX 6.1a installed
//              0x00070000 = DirectX 7.0 installed
//              0x00070001 = DirectX 7.0a installed
//              0x00080000 = DirectX 8.0 installed
//              0x00080100 = DirectX 8.1 installed
//              0x00080101 = DirectX 8.1a installed
//              0x00080102 = DirectX 8.1b installed
//              0x00080200 = DirectX 8.2 installed
//              0x00090000 = DirectX 9.0 installed
//
// Please note that this code is intended as a general guideline. Your
// app will probably be able to simply query for functionality (via
// QueryInterface) for one or two components.
//
// Also please ensure your app will run on future releases of DirectX.
// For example:
//     "if( dwDirectXVersion != 0x00080100 ) return false;" is VERY BAD. 
//     "if( dwDirectXVersion < 0x00080100 ) return false;" is MUCH BETTER.
//-----------------------------------------------------------------------------

DWORD GetDXVersion ()
{
    DWORD version = 0;
    bool bGotDirectXVersion = false;  

    // Init values to unknown
    DWORD dwDirectXVersionMajor = 0;
    DWORD dwDirectXVersionMinor = 0;
    TCHAR cDirectXVersionLetter = ' ';

    // First, try to use dxdiag's COM interface to get the DirectX version.  
    // The only downside is this will only work on DX9 or later.
    if ( SUCCEEDED( GetDirectXVersionViaDxDiag( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
        bGotDirectXVersion = true;
  
    if ( !bGotDirectXVersion ) {
        // Getting the DirectX version info from DxDiag failed, 
        // so most likely we are on DX8.x or earlier
        if( SUCCEEDED( GetDirectXVerionViaFileVersions( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
            bGotDirectXVersion = true;
    }

    // If both techniques failed, then return error
    if (bGotDirectXVersion) {
          
      // Set the output values to what we got and return       
      cDirectXVersionLetter = (char)tolower(cDirectXVersionLetter);
      
      // Set version to something like 0x00080102 which would represent DX8.1b
      version = dwDirectXVersionMajor;
      version <<= 8;
      version += dwDirectXVersionMinor;
      version <<= 8;
      if( cDirectXVersionLetter >= 'a' && cDirectXVersionLetter <= 'z' )
        version += (cDirectXVersionLetter - 'a') + 1;
    }

    
   return (version);
}

//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaDxDiag()
// Desc: Tries to get the DirectX version from DxDiag's COM interface
//-----------------------------------------------------------------------------
static HRESULT GetDirectXVersionViaDxDiag (DWORD* pdwDirectXVersionMajor, 
                                           DWORD* pdwDirectXVersionMinor, 
                                           TCHAR* pcDirectXVersionLetter )
{
    HRESULT hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingMajor = false;
    bool bSuccessGettingMinor = false;
    bool bSuccessGettingLetter = false;
    
    // Init COM.  COM may fail if its already been inited with a different 
    // concurrency model.  And if it fails you shouldn't release it.
    hr = CoInitialize(NULL);
    bCleanupCOM = SUCCEEDED(hr);

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           (LPVOID*) &pDxDiagProvider );
    if( SUCCEEDED(hr) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks        = false;
        dxDiagInitParam.pReserved               = NULL;

        // Init the m_pDxDiagProvider
        hr = pDxDiagProvider->Initialize( &dxDiagInitParam ); 
        if( SUCCEEDED(hr) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED(hr) ) 
            {
                // Get the object called DxDiag_SystemInfo
                hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED(hr) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMajor )
                            *pdwDirectXVersionMajor = var.ulVal; 
                        bSuccessGettingMajor = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMinor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMinor )
                            *pdwDirectXVersionMinor = var.ulVal; 
                        bSuccessGettingMinor = true;
                    }
                    VariantClear( &var );

                    // Get the "szDirectXVersionLetter" property
                    hr = pDxDiagSystemInfo->GetProp( L"szDirectXVersionLetter", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL )
                    {
#ifdef UNICODE
                        *pcDirectXVersionLetter = var.bstrVal[0]; 
#else
                        char strDestination[10];
                        WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, strDestination, 10*sizeof(CHAR), NULL, NULL );
                        if( pcDirectXVersionLetter )
                            *pcDirectXVersionLetter = strDestination[0]; 
#endif
                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        pDxDiagProvider->Release();
    }

    if( bCleanupCOM )
        CoUninitialize();
        
    if( bGotDirectXVersion )
        return S_OK;
    else
        return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: GetDirectXVerionViaFileVersions()
// Desc: Tries to get the DirectX version by looking at DirectX file versions
//-----------------------------------------------------------------------------
static HRESULT GetDirectXVerionViaFileVersions( DWORD* pdwDirectXVersionMajor, 
                                                DWORD* pdwDirectXVersionMinor, 
                                                TCHAR* pcDirectXVersionLetter )
{       
    ULARGE_INTEGER llFileVersion;  
    TCHAR szPath[512];
    TCHAR szFile[512];
    BOOL bFound = false;

    if( GetSystemDirectory( szPath, MAX_PATH ) != 0 )
    {
        szPath[MAX_PATH-1]=0;
            
        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 2, 0, 95 ) ) >= 0 ) // Win9x version
            {                    
                // flle is >= DX1.0 version, so we must be at least DX1.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 1;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 3, 0, 1096 ) ) >= 0 ) // Win9x version
            {                    
                // flle is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 2;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 68 ) ) >= 0 ) // Win9x version
            {                    
                // flle is is >= DX3.0 version, so we must be at least DX3.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }
        
        // Switch off the d3drg8x.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3drg8x.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 70 ) ) >= 0 ) // Win9x version
            {                    
                // d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b  (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }       

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 5, 0, 155 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 5;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 318 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.0 version, so we must be at least DX6.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 436 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dplayx.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dplayx.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 3, 518 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 700 ) ) >= 0 ) // Win9x version
            {                    
                // TODO: find win2k version
                
                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dinput version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dinput.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 716 ) ) >= 0 ) // Win9x version
            {                    
                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 0, 400 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2258, 400 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or DX8.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3d8.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 881 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 881 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // d3d8.dll is the DX8.1 version, so we must be at least DX8.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 901 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 901 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // d3d8.dll is the DX8.1a version, so we must be at least DX8.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\mpg2splt.ax"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 6, 3, 1, 885 ) ) >= 0 ) // Win9x/Win2k/WinXP version
            {                    
                // quartz.dll is the DX8.1b version, so we must be at least DX8.1b
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('b');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\dpnet.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 9, 0, 134 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 2, 3677, 134 ) ) >= 0) ) // Win2k/WinXP version
            {                    
                // dpnet.dll is the DX8.2 version, so we must be at least DX8.2
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 2;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT("\\d3d9.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            // File exists, but be at least DX9
            if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 9;
            if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
            if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
            bFound = true;
        }
    }

    if( !bFound )
    {
        // No DirectX installed
        if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 0;
        if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
        if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFileVersion()
// Desc: Returns ULARGE_INTEGER with a file version of a file, or a failure code.
//-----------------------------------------------------------------------------
static HRESULT GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion )
{   
    if( szPath == NULL || pllFileVersion == NULL )
        return E_INVALIDARG;

    DWORD dwHandle;
    UINT  cb;
    cb = GetFileVersionInfoSize( szPath, &dwHandle );
    if (cb > 0)
    {
        BYTE* pFileVersionBuffer = new BYTE[cb];
        if( pFileVersionBuffer == NULL )
            return E_OUTOFMEMORY;

        if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
        {
            VS_FIXEDFILEINFO* pVersion = NULL;
            if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) && 
                pVersion != NULL) 
            {
                pllFileVersion->HighPart = pVersion->dwFileVersionMS;
                pllFileVersion->LowPart  = pVersion->dwFileVersionLS;
                delete[] pFileVersionBuffer;
                return S_OK;
            }
        }

        delete[] pFileVersionBuffer;
    }

    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: MakeInt64()
// Desc: Returns a ULARGE_INTEGER where a<<48|b<<32|c<<16|d<<0
//-----------------------------------------------------------------------------
static ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d )
{
    ULARGE_INTEGER ull;
    ull.HighPart = MAKELONG(b,a);
    ull.LowPart = MAKELONG(d,c);
    return ull;
}




//-----------------------------------------------------------------------------
// Name: CompareLargeInts()
// Desc: Returns 1 if ullParam1 > ullParam2
//       Returns 0 if ullParam1 = ullParam2
//       Returns -1 if ullParam1 < ullParam2
//-----------------------------------------------------------------------------
static int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 )
{
    if( ullParam1.HighPart > ullParam2.HighPart )
        return 1;
    if( ullParam1.HighPart < ullParam2.HighPart )
        return -1;

    if( ullParam1.LowPart > ullParam2.LowPart )
        return 1;
    if( ullParam1.LowPart < ullParam2.LowPart )
        return -1;

    return 0;
}


////-----------------------------------------------------------------------------
//// File: GetDXVer.cpp
////
//// Desc: Demonstrates how applications can detect what version of DirectX
////       is installed.
////
//// (C) Copyright 1995-2000 Microsoft Corp.  All rights reserved.
////-----------------------------------------------------------------------------
//
////#include <windows.h>
////#include <windowsx.h>
//#include <AfxWin.h>
//#include <AfxExt.h>
//#include <AfxTempl.h>
//#include <AfxMt.h>
//
////#include <basetsd.h>
//#include <ddraw.h>
//#include <dinput.h>
//#include <dmusici.h>
//
//#include "getdxver.h"
//
//typedef HRESULT(WINAPI * DIRECTDRAWCREATE)( GUID*, LPDIRECTDRAW*, IUnknown* );
//typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );
//typedef HRESULT(WINAPI * DIRECTINPUTCREATE)( HINSTANCE, DWORD, LPDIRECTINPUT*,
//                                             IUnknown* );
//
////-----------------------------------------------------------------------------
//// Name: GetDXVersion()
//// Desc: This function returns the DirectX version number as follows:
////          0x0000 = No DirectX installed
////          0x0100 = DirectX version 1 installed
////          0x0200 = DirectX 2 installed
////          0x0300 = DirectX 3 installed
////          0x0500 = At least DirectX 5 installed.
////          0x0600 = At least DirectX 6 installed.
////          0x0601 = At least DirectX 6.1 installed.
////          0x0700 = At least DirectX 7 installed.
////          0x0800 = At least DirectX 8 installed.
//// 
////       Please note that this code is intended as a general guideline. Your
////       app will probably be able to simply query for functionality (via
////       QueryInterface) for one or two components.
////
////       Please also note:
////          "if( dwDXVersion != 0x500 ) return FALSE;" is VERY BAD. 
////          "if( dwDXVersion <  0x500 ) return FALSE;" is MUCH BETTER.
////       to ensure your app will run on future releases of DirectX.
////-----------------------------------------------------------------------------
//DWORD GetDXVersion()
//{
//    DIRECTDRAWCREATE     DirectDrawCreate   = NULL;
//    DIRECTDRAWCREATEEX   DirectDrawCreateEx = NULL;
//    DIRECTINPUTCREATE    DirectInputCreate  = NULL;
//    HINSTANCE            hDDrawDLL          = NULL;
//    HINSTANCE            hDInputDLL         = NULL;
//    HINSTANCE            hD3D8DLL           = NULL;
//    LPDIRECTDRAW         pDDraw             = NULL;
//    LPDIRECTDRAW2        pDDraw2            = NULL;
//    LPDIRECTDRAWSURFACE  pSurf              = NULL;
//    LPDIRECTDRAWSURFACE3 pSurf3             = NULL;
//    LPDIRECTDRAWSURFACE4 pSurf4             = NULL;
//    DWORD                dwDXVersion        = 0;
//    HRESULT              hr;
//
//    // First see if DDRAW.DLL even exists.
//    hDDrawDLL = LoadLibrary( "DDRAW.DLL" );
//    if( hDDrawDLL == NULL )
//    {
//        dwDXVersion = 0;
//        return dwDXVersion;
//    }
//
//    // See if we can create the DirectDraw object.
//    DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress( hDDrawDLL, "DirectDrawCreate" );
//    if( DirectDrawCreate == NULL )
//    {
//        dwDXVersion = 0;
//        FreeLibrary( hDDrawDLL );
//        OutputDebugString( "Couldn't LoadLibrary DDraw\r\n" );
//        return dwDXVersion;
//    }
//
//    hr = DirectDrawCreate( NULL, &pDDraw, NULL );
//    if( FAILED(hr) )
//    {
//        dwDXVersion = 0;
//        FreeLibrary( hDDrawDLL );
//        OutputDebugString( "Couldn't create DDraw\r\n" );
//        return dwDXVersion;
//    }
//
//    // So DirectDraw exists.  We are at least DX1.
//    dwDXVersion = 0x100;
//
//    // Let's see if IID_IDirectDraw2 exists.
//    hr = pDDraw->QueryInterface( IID_IDirectDraw2, (VOID**)&pDDraw2 );
//    if( FAILED(hr) )
//    {
//        // No IDirectDraw2 exists... must be DX1
//        pDDraw->Release();
//        FreeLibrary( hDDrawDLL );
//        OutputDebugString( "Couldn't QI DDraw2\r\n" );
//        return dwDXVersion;
//    }
//
//    // IDirectDraw2 exists. We must be at least DX2
//    pDDraw2->Release();
//    dwDXVersion = 0x200;
//
//
//	//-------------------------------------------------------------------------
//    // DirectX 3.0 Checks
//	//-------------------------------------------------------------------------
//
//    // DirectInput was added for DX3
//    hDInputDLL = LoadLibrary( "DINPUT.DLL" );
//    if( hDInputDLL == NULL )
//    {
//        // No DInput... must not be DX3
//        OutputDebugString( "Couldn't LoadLibrary DInput\r\n" );
//        pDDraw->Release();
//        return dwDXVersion;
//    }
//
//    DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( hDInputDLL,
//                                                        "DirectInputCreateA" );
//    if( DirectInputCreate == NULL )
//    {
//        // No DInput... must be DX2
//        FreeLibrary( hDInputDLL );
//        FreeLibrary( hDDrawDLL );
//        pDDraw->Release();
//        OutputDebugString( "Couldn't GetProcAddress DInputCreate\r\n" );
//        return dwDXVersion;
//    }
//
//    // DirectInputCreate exists. We are at least DX3
//    dwDXVersion = 0x300;
//    FreeLibrary( hDInputDLL );
//
//    // Can do checks for 3a vs 3b here
//
//
//	//-------------------------------------------------------------------------
//    // DirectX 5.0 Checks
//	//-------------------------------------------------------------------------
//
//    // We can tell if DX5 is present by checking for the existence of
//    // IDirectDrawSurface3. First, we need a surface to QI off of.
//    DDSURFACEDESC ddsd;
//    ZeroMemory( &ddsd, sizeof(ddsd) );
//    ddsd.dwSize         = sizeof(ddsd);
//    ddsd.dwFlags        = DDSD_CAPS;
//    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
//
//    hr = pDDraw->SetCooperativeLevel( NULL, DDSCL_NORMAL );
//    if( FAILED(hr) )
//    {
//        // Failure. This means DDraw isn't properly installed.
//        pDDraw->Release();
//        FreeLibrary( hDDrawDLL );
//        dwDXVersion = 0;
//        OutputDebugString( "Couldn't Set coop level\r\n" );
//        return dwDXVersion;
//    }
//
//    hr = pDDraw->CreateSurface( &ddsd, &pSurf, NULL );
//    if( FAILED(hr) )
//    {
//        // Failure. This means DDraw isn't properly installed.
//        pDDraw->Release();
//        FreeLibrary( hDDrawDLL );
//        dwDXVersion = 0;
//        OutputDebugString( "Couldn't CreateSurface\r\n" );
//        return dwDXVersion;
//    }
//
//    // Query for the IDirectDrawSurface3 interface
//    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface3,
//                                       (VOID**)&pSurf3 ) ) )
//    {
//        pDDraw->Release();
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // QI for IDirectDrawSurface3 succeeded. We must be at least DX5
//    dwDXVersion = 0x500;
//
//
//	//-------------------------------------------------------------------------
//    // DirectX 6.0 Checks
//	//-------------------------------------------------------------------------
//
//    // The IDirectDrawSurface4 interface was introduced with DX 6.0
//    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface4,
//                                       (VOID**)&pSurf4 ) ) )
//    {
//        pDDraw->Release();
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // IDirectDrawSurface4 was create successfully. We must be at least DX6
//    dwDXVersion = 0x600;
//    pSurf->Release();
//    pDDraw->Release();
//
//
//	//-------------------------------------------------------------------------
//    // DirectX 6.1 Checks
//	//-------------------------------------------------------------------------
//
//    // Check for DMusic, which was introduced with DX6.1
//    LPDIRECTMUSIC pDMusic = NULL;
//    CoInitialize( NULL );
//    hr = CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
//                           IID_IDirectMusic, (VOID**)&pDMusic );
//    if( FAILED(hr) )
//    {
//        OutputDebugString( "Couldn't create CLSID_DirectMusic\r\n" );
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // DirectMusic was created successfully. We must be at least DX6.1
//    dwDXVersion = 0x601;
//    pDMusic->Release();
//    CoUninitialize();
//    
//
//	//-------------------------------------------------------------------------
//    // DirectX 7.0 Checks
//	//-------------------------------------------------------------------------
//
//    // Check for DirectX 7 by creating a DDraw7 object
//    LPDIRECTDRAW7 pDD7;
//    DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
//                                                       "DirectDrawCreateEx" );
//    if( NULL == DirectDrawCreateEx )
//    {
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
//                                    NULL ) ) )
//    {
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // DDraw7 was created successfully. We must be at least DX7.0
//    dwDXVersion = 0x700;
//    pDD7->Release();
//
//
//	//-------------------------------------------------------------------------
//    // DirectX 8.0 Checks
//	//-------------------------------------------------------------------------
//
//    // Simply see if D3D8.dll exists.
//    hD3D8DLL = LoadLibrary( "D3D8.DLL" );
//    if( hD3D8DLL == NULL )
//    {
//	    FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // D3D8.dll exists. We must be at least DX8.0
//    dwDXVersion = 0x800;
//
//
//	//-------------------------------------------------------------------------
//    // End of checking for versions of DirectX 
//	//-------------------------------------------------------------------------
//
//    // Close open libraries and return
//    FreeLibrary( hDDrawDLL );
//    FreeLibrary( hD3D8DLL );
//    
//    return dwDXVersion;
//}
