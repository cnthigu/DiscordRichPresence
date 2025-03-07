#include <Windows.h>
#include <vector>
#include <string>
#include <ctime>
#include "discord.h"

using f_CrashFunction = void( * )( );

static const std::vector< std::string > blacklisted_names =
{
    "Lua script: Cheat Table",
    "Cheat Engine settings",
    "Pause the game while scanning",
    "Enable Speedhack"
};

discord::Core* core{ };

static BOOL CALLBACK enum_windows_routine( HWND hwnd, LPARAM lparam )
{
    char buffer[ 256 ]{ };
    if ( !GetWindowTextA( hwnd, buffer, sizeof( buffer ) - 1 ) )
        return TRUE;

    std::string window_text = buffer;

    for ( const auto& blacklisted_name : blacklisted_names )
    {
        if ( window_text.find( blacklisted_name ) != std::string::npos )
            reinterpret_cast< f_CrashFunction >( lparam )( );
    }

    return TRUE;
}

static DWORD WINAPI main_thread( void* )
{

    discord::Core::Create( 1344514931135086613, DiscordCreateFlags_Default, &core );


    auto& activityManager = core->ActivityManager( );


    discord::Activity activity{ };

    activity.SetDetails( "Classic Conquer" );
    activity.SetState( "http://origensco.com/" );
    activity.GetAssets( ).SetLargeImage( "origensco_resized" );
    activity.GetAssets( ).SetLargeText( "Testando23092390320923" );
    activity.GetTimestamps( ).SetStart( std::time( nullptr ) );


    activityManager.UpdateActivity(activity, [](discord::Result result) 
    {
        Sleep( 1 );
    });

    bool changed_name = false, changed_title = false, changed_url = false;

    while ( true )
    {
        if ( !changed_name || !changed_title || !changed_url )
        {
            const auto game_module    = reinterpret_cast< char* >( GetModuleHandleA( "GameModule.dll" ) );
            const auto conquer_module = reinterpret_cast< char* >( GetModuleHandleA( "Conquer.exe" ) );

            if ( !game_module || !conquer_module )
                continue;

            const auto executable_name = game_module + 0x7434;
            const auto url_name        = conquer_module + 0x1C93C8;

            if ( !changed_name && !strcmp( executable_name, "Resurrection" ) )
            {
                strcpy( executable_name, "OrigensCO" );
                changed_name = true;
            }

            if ( !changed_url && !strcmp( url_name, "http://co.91.com/signout/" ) )
            {
                strcpy( url_name, "http://origensco.com" );
                changed_url = true;
            }

            if ( !changed_title )
            {
                const auto game_hwnd = FindWindowW( nullptr, L"LastCO" );

                if ( game_hwnd && SetWindowTextW( game_hwnd, L"OrigensCO" ) )
                    changed_title = true;
            }

            Sleep( 1 );
        }

        else
        {

            core->RunCallbacks( );

            const auto crash_function = reinterpret_cast< f_CrashFunction >( GetModuleHandleA( "GameModule.dll" ) );
            EnumWindows( &enum_windows_routine, reinterpret_cast< LPARAM >( crash_function ) );

            if ( GetModuleHandleA( "speedhack-i386.dll" ) )
                crash_function( );

            Sleep( 100 );
        }
    }

    return NULL;
}

EXTERN_C __declspec( dllexport ) int dmy = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch ( ul_reason_for_call )
    {
        case DLL_PROCESS_ATTACH:
        {
            const auto hthread = CreateThread( nullptr, 0, &main_thread, nullptr, 0, nullptr );

            if ( hthread )
                CloseHandle( hthread );

            break;
        }
    }

    return TRUE;
}