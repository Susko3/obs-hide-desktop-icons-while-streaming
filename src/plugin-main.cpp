/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <obs-frontend-api.h>

#include "plugin-macros.generated.h"

// FROM https://stackoverflow.com/a/53347282
#include <ShlObj.h>     // Shell API
#include <atlcomcli.h>  // CComPtr & Co.
#include <string>
#include <iostream>
#include <system_error>

// Throw a std::system_error if the HRESULT indicates failure.
template< typename T >
void ThrowIfFailed( HRESULT hr, T&& msg )
{
    if( FAILED( hr ) )
        throw std::system_error{ hr, std::system_category(), std::forward<T>( msg ) };
}

// RAII wrapper to initialize/uninitialize COM
struct CComInit
{
    CComInit() { ThrowIfFailed( ::CoInitialize( nullptr ), "CoInitialize failed" ); }
    ~CComInit() { ::CoUninitialize(); }
    CComInit( CComInit const& ) = delete;
    CComInit& operator=( CComInit const& ) = delete;
};

// Query an interface from the desktop shell view.
void FindDesktopFolderView( REFIID riid, void **ppv, std::string const& interfaceName )
{
    CComPtr<IShellWindows> spShellWindows;
    ThrowIfFailed(
        spShellWindows.CoCreateInstance( CLSID_ShellWindows ),
        "Failed to create IShellWindows instance" );

    CComVariant vtLoc( CSIDL_DESKTOP );
    CComVariant vtEmpty;
    long lhwnd;
    CComPtr<IDispatch> spdisp;
    ThrowIfFailed(
        spShellWindows->FindWindowSW(
            &vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &spdisp ),
        "Failed to find desktop window" );

    CComQIPtr<IServiceProvider> spProv( spdisp );
    if( ! spProv )
        ThrowIfFailed( E_NOINTERFACE, "Failed to get IServiceProvider interface for desktop" );

    CComPtr<IShellBrowser> spBrowser;
    ThrowIfFailed(
        spProv->QueryService( SID_STopLevelBrowser, IID_PPV_ARGS( &spBrowser ) ),
        "Failed to get IShellBrowser for desktop" );

    CComPtr<IShellView> spView;
    ThrowIfFailed(
        spBrowser->QueryActiveShellView( &spView ),
        "Failed to query IShellView for desktop" );

    ThrowIfFailed(
        spView->QueryInterface( riid, ppv ),
        "Could not query desktop IShellView for interface " + interfaceName );
}

void SetDesktopIconsVisible(bool show)
{
    CComPtr<IFolderView2> spView;
    FindDesktopFolderView( IID_PPV_ARGS(&spView), "IFolderView2" );

    DWORD flags = 0;
    ThrowIfFailed(
        spView->GetCurrentFolderFlags( &flags ),
        "GetCurrentFolderFlags failed" );

    if(show)
        flags = flags & ~FWF_NOICONS;
    else
        flags = flags | FWF_NOICONS;

    ThrowIfFailed(
        spView->SetCurrentFolderFlags( FWF_NOICONS, flags ),
        "SetCurrentFolderFlags failed" );
}

void callback(enum obs_frontend_event event, void* data)
{
    (void)data;

    try
    {
        switch(event)
        {
            case OBS_FRONTEND_EVENT_STREAMING_STARTED:
                SetDesktopIconsVisible(false);
                break;

            case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
                SetDesktopIconsVisible(true);
                break;
        }
    }
    catch (const std::system_error& e)
    {
        blog(LOG_ERROR, "Error: %s", e.what());
    }
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	blog(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

    obs_frontend_add_event_callback(callback, nullptr);
	return true;
}

void obs_module_unload()
{
    obs_frontend_remove_event_callback(callback, nullptr);

	blog(LOG_INFO, "plugin unloaded");
}
