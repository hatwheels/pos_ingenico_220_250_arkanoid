/*
 * utility.c
 *
 *  Created on: Nov 15, 2019
 *      Author: hatwheels
 *      Based on TRAINING module of Ingenico
 */

#include "SDK30.H"
#include "LinkLayer.h"
#include "schVar_def.h"
#include "SchutilTlv_def.h"
#include "VGE_FMG.H"

#include "inc/utility.h"

//****************************************************************************
//     int show_menu (cont char *pcTitle, int iMenuStyle, int iDefaultItem,
//                   int iMaxItems, const char *tpcItems[])
//  This function shows the navigation menu.
//  This function has parameters.
//     pcTitle (I-) : Menu title
//     iMenuStyle (I-) : 0-with shortcut 1-without shortcut 2-with predefined value
//     iDefaultItem (I-) : Pre-select item number
//     iMaxItems (I-) : Maximum items 
//     tpcItems (I-) : Items table
//  This function has return value.
//    >=0 : Selected item
//    <0  : Cancel/timeout/backspace
//****************************************************************************
int show_menu (const char *pcTitle, int iMenuStyle, int iDefaultItem, int iMaxItems, const char *tpcItems[])
{
    // Local variables 
    // ***************
    StructList xMenu;                                       // List definition
    int iMaxX, iMaxY;
    ENTRY_BUFFER xEntry;                                    // Result entry
    int i, iInput;
    int iRet;

    // Menu management
    // ***************
    GetScreenSize(&iMaxY, &iMaxX);                          // Get screen size
    if ((iDefaultItem < 0) || (iDefaultItem >= iMaxItems))  // Initialise pre-select item
        iDefaultItem=0;
    if ((iMenuStyle < 0) || (iMenuStyle > 2))               // Initialise menu style
        iMenuStyle=0;

    memset(&xMenu, 0, sizeof(xMenu));
    xMenu.MyWindow.left         = 0;                        // Windows
    xMenu.MyWindow.top          = 0;                        // position
    xMenu.MyWindow.rigth        = iMaxX-1;                  // in
    xMenu.MyWindow.bottom       = iMaxY-1;                  // pixels

    if (iMaxY == 128)
        xMenu.MyWindow.nblines  = 10;                       // Large screen
    else
        xMenu.MyWindow.nblines  = 5;                        // Normal screen

    xMenu.MyWindow.fontsize	    = _MEDIUM_;
    xMenu.MyWindow.type	        = _PROPORTIONNEL_;          // Font type
    xMenu.MyWindow.font	        = 0;                        // Should be zero
    xMenu.MyWindow.correct      = _ON_;                     // Process correction key 
    xMenu.MyWindow.offset       = 0;                        // Table index for selection

    if (iMenuStyle == 0)
        xMenu.MyWindow.shortcommand	= _ON_;                 // Shortcut key
    else
        xMenu.MyWindow.shortcommand	= _OFF_;

    if (iMenuStyle == 2)
        xMenu.MyWindow.selected = _ON_;                     // Display a predefined item
    else
        xMenu.MyWindow.selected = _OFF_;

    xMenu.MyWindow.thickness    = 2;                        // Border thickness of the window entry
    xMenu.MyWindow.border       = _ON_;                     // Display window entry border 
    xMenu.MyWindow.popup        = _NOPOPUP_;                // Save an restore screen
    xMenu.MyWindow.first        = iDefaultItem;             // Pre select item if selected is _OFF_
    xMenu.MyWindow.current      = iDefaultItem;             // Pre select item if selected is _ON_
    xMenu.MyWindow.time_out     = 60;                       // Time-out in second
    xMenu.MyWindow.title        = (byte*)pcTitle;           // Window title, no title is NULL

    for (i = 0; i < iMaxItems; i++)                         // Initialize the item list
        xMenu.tab[i] = (byte*)tpcItems[i];

    G_List_Entry((void*)&xMenu);                            // Start navigating menu
    ttestall(ENTRY, 0);                                     // Wait for confirmation key
    iInput = Get_Entry((void*)&xEntry);                     // Get the result of the selection

    switch(iInput)
    {
        case CR_ENTRY_OK: // Retrieve the item selected
            iRet = xEntry.d_entry[0];
            break;

        default: // Timeout/Cancel/BackSpace
            iRet=-1;
            break;
    }

    return iRet;
}

//****************************************************************************
//     void display_small (byte ucLine, byte ucColumn, char *pcText)
//     void display_medium (byte ucLine, byte ucColumn, char *pcText)
//     void display_large (byte ucLine, byte ucColumn, char *pcText)
//  The following standard functions:
//  - fopen(), fclose() (enable, disable display)
//  - putc(), puts(), printf() (write character, string and formatted string)
//  MUST NOT be used anymore.
//  Use Graphic Library instead to keep compatibility between B&W and color
//  terminal on colorized mode.
//  What is colorized mode?
//  - Emulation mode: monochrome application runs on a color terminal
//  - header controlled by the manager
//  - Text & background colors setting through Manager menu
//    (Initialization/Hardware/Display/Color Setup)
//  - Expansion factor performed by manager
//  This function has parameters.
//     ucLine (I-) : Line number
//     ucColumn (I-) : Column number
//     pcText (I-) : String to display
//  This function has no return value.
//****************************************************************************

void display_small (byte ucLine, byte ucColumn, char *pcText)
{
    DrawExtendedString(ucColumn*5, ucLine*8, pcText, _OFF_, _POLICE4x7_, _NORMALE_);
}

void display_medium (byte ucLine, byte ucColumn, char *pcText)
{
    DrawExtendedString(ucColumn*8, ucLine*17, pcText, _OFF_, _POLICE8x12_, _NORMALE_);
}

void display_large (byte ucLine, byte ucColumn, char *pcText)
{
    DrawExtendedString(ucColumn*16, ucLine*16, pcText, _OFF_, _XLARGE_, _NORMALE_);
}
