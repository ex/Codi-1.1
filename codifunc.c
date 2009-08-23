/* ========================================================================== */
/*                                 CODI (Win32)                               */
/* -------------------------------------------------------------------------- */
/* An exercise on Win32 programming disguised as a file splitter/merger,      */
/* additionally it has the ability to encrypt the files (in a very weak way). */
/*                                                                            */
/* Copyright (c) 2001-2009 Laurens Rodriguez Oscanoa.                         */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation; either version 2 of the License, or          */
/* (at your option) any later version.                                        */
/*                                                                            */
/* This program is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of             */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               */
/* GNU General Public License for more details.                               */
/*                                                                            */
/* You should have received a copy of the GNU General Public License          */
/* along with this program; if not, write to the Free Software                */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA    */
/* -------------------------------------------------------------------------- */

//***************************************************************
//  Especificacion de funciones usadas para la interface de <CODI>
#include "CODIfunc.h"

//  Esta funcion crea los menus con dibujos
//
VOID PonerDibujoMenu (HWND      hWnd, 
                      HINSTANCE hInstance)
{

HBITMAP hbmpDibujoMenu1;
HBITMAP hbmpDibujoMenu2;
HMENU   hmenu = GetMenu (hWnd);     // handle to main menu


    // (dibujaremos un signo de interrogacion al costado del IDM_ACERCADE)
    hbmpDibujoMenu1 = DameMiDibujoMenu ((UINT)IDB_MENUACERCADE, hInstance);
    SetMenuItemBitmaps (hmenu, IDM_ACERCADE, MF_BYCOMMAND, hbmpDibujoMenu1, hbmpDibujoMenu1);
    
    // (dibujaremos una x al costado del IDM_SALIR)
    hbmpDibujoMenu2 = DameMiDibujoMenu ((UINT)IDB_MENUSALIR, hInstance);
    SetMenuItemBitmaps (hmenu, IDM_SALIR, MF_BYCOMMAND, hbmpDibujoMenu2, hbmpDibujoMenu2);
}


//  Esta funcion la usamos para devolvernos una mapa de bits para el menu,
//  Sirve cuando quieres poner un dibujo monocromo (un signo) al costado de tu menu. 
//  El recurso mapa de bits debe de ser pequeño ~ (13*13 pxls) pues sino se vera muy distorcionado
//
HBITMAP DameMiDibujoMenu (UINT      opcionID,
                          HINSTANCE hinst) 
{ 

HDC         hdcSource;          // source device context             
HDC         hdcTarget;          // target device context             
HBITMAP     hbmpDibujo;         // manejador del mapa de bits del dibujo (recurso)
BITMAP      bmDibujo;           // structure for bitmap data         
HBITMAP     hbmpSourceOld;      // handle to original source bitmap  
HBITMAP     hbmpTargetOld;      // handle to original target bitmap  
HBITMAP     hbmpDibujoMenu;     // manejador del dibujo del menu
WORD        wBitmapX;           // width of check-mark bitmap        
WORD        wBitmapY;           // height of check-mark bitmap       

HBRUSH      hbrTargetOld;
HBRUSH      hbrBackground;

    //-----------------------------------------------------------------------------
    // Version: 1.1
    //-----------------------------------------------------------------------------
    // Ya no se agrandara el tamaño de estos menus de acuerdo al tamaño del menu
    // que tenga el sistema (se ve feo), pero si es mas grande se centrará.
    //-----------------------------------------------------------------------------
    int x0 = 0, y0 = 0;

    // Create memory device contexts for the source and destination bitmaps. 
    hdcSource = CreateCompatibleDC ((HDC) NULL);
    hdcTarget = CreateCompatibleDC (hdcSource);

    // Get the size of the the system default check-mark bitmap and
    // create a compatible bitmap of the same size.
    wBitmapX = GetSystemMetrics (SM_CXMENUCHECK);
    wBitmapY = GetSystemMetrics (SM_CYMENUCHECK);

    //-----------------------------------------------------------------------------
    // Version: 1.1
    //-----------------------------------------------------------------------------
    hbrBackground = CreateSolidBrush (RGB(255,255,255));

    hbmpDibujoMenu = CreateCompatibleBitmap (hdcTarget, wBitmapX, wBitmapY);

    // Select the background brush and bitmap into the target DC.
    hbrTargetOld = SelectObject (hdcTarget, hbrBackground);
    hbmpTargetOld = SelectObject (hdcTarget, hbmpDibujoMenu);
    
    // Use the selected brush to initialize the background color
    PatBlt (hdcTarget, 0, 0, wBitmapX, wBitmapY, PATCOPY);
    
    // Carga el mapa de bits cuyo ID le pasaste a la funcion y lo selecciona en
    // el contexto de dispositivo fuente
    hbmpDibujo = LoadBitmap ((HINSTANCE) hinst, MAKEINTRESOURCE(opcionID));
    hbmpSourceOld = SelectObject (hdcSource, hbmpDibujo);
    
    // Llena una structura BITMAP con informacion acerca de tu mapa de bits de recurso
    GetObject (hbmpDibujo, sizeof(BITMAP), &bmDibujo);

    //-----------------------------------------------------------------------------
    // Version: 1.1
    //-----------------------------------------------------------------------------
    x0 = GetSystemMetrics (SM_CXMENUCHECK) - 13;
    if (x0 > 0)
    {
        x0 = (int) (x0 / 2);
    }

    y0 = GetSystemMetrics (SM_CYMENUCHECK) - 13;
    if (y0 > 0)
    {
        y0 = (int) (y0 / 2);
    }
    //-----------------------------------------------------------------------------

    // Copy the appropriate bitmap into the target DC. If StretchBlt must convert a monochrome bitmap to a color
    // bitmap, it sets white bits (1) to the background color and black bits (0) to the foreground color. 
    if (y0 < 0)
    {
        // para aquellos sadicos que tiene sus menus bien chiquitos...
        StretchBlt (hdcTarget, 0, 0, 13 + x0, 13 + y0, hdcSource, 0, 0, bmDibujo.bmWidth, bmDibujo.bmHeight, SRCCOPY);
    }
    else
    {
        StretchBlt (hdcTarget, x0, y0, bmDibujo.bmWidth, bmDibujo.bmHeight, hdcSource, 0, 0, bmDibujo.bmWidth, bmDibujo.bmHeight, SRCCOPY);
    }

    SelectObject (hdcSource, hbmpSourceOld);
    hbmpDibujoMenu = SelectObject (hdcTarget, hbmpTargetOld); 
    SelectObject (hdcTarget, hbrTargetOld); 
    DeleteObject (hdcSource);
    DeleteObject (hdcTarget);
    DeleteObject (hbmpDibujo);

    // Return the handle to the new check-mark bitmap.  
    return hbmpDibujoMenu; 
}

//  Esta funcion responde al mensaje enviado por el sistema WM_DROPFILES
//
VOID OnDropFiles (HWND          hwnd,
                  HINSTANCE     hInstance,
                  WPARAM        wParam,
                  LPINFODRAG    lpInfDrag,
                  BOOL          *pg_bDrag)
{

int                 iFiles;
HDROP               hDropInfo = (HDROP) wParam;
WIN32_FIND_DATA     FileData;
HANDLE              handle;
WORD                wMensaje;
int                 k;

    
    iFiles = DragQueryFile (hDropInfo, (DWORD)(-1), (LPSTR)NULL, 0);
    if (iFiles != 1)
    {
        MessageBox (NULL, "Hey! Uno a la vez, please.", NULL, MB_OK | MB_ICONERROR);
        DragFinish (hDropInfo);
    }
    else
    {
        // Pedimos informacion del archivo y lo guardamos en el buffer
        DragQueryFile (hDropInfo, 0, lpInfDrag->szFileDrag, sizeof (lpInfDrag->szFileDrag));
        DragFinish (hDropInfo);

        handle = FindFirstFile(lpInfDrag->szFileDrag, & FileData);
        if (handle == INVALID_HANDLE_VALUE)
            MessageBox (NULL, "No se pudo acceder al archivo.", NULL, MB_OK | MB_ICONERROR);
        else
        {
            FindClose (handle);
            if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            {
                // Si es un directorio suponemos que el usuario quiere reunir sus archivos
                wMensaje = IDM_REUNIR;
                goto ENVIAR_MENSAJE;
            }
            else
            {
                // Cuando el nombre es chico el campo <FileData.cAlternateFileName> no se llena (!)
                // Buscamos la extension => buscamos el ultimo caracter '.' 
                for( k = strlen (lpInfDrag->szFileDrag) - 1; k > 0; k--)
                {
                    if (lpInfDrag->szFileDrag [k] == '.')
                        break;
                }

                // Si no tiene extension la comparacion sera falsa
                if (!lstrcmpi (& lpInfDrag->szFileDrag [k + 1], "cd$"))
                {
OP_DRAG_REUNIR:
                    // Si es un archivo *.cd$ buscamos el directorio al que pertenece
                    // y suponemos que el usuario quiere reunir sus archivos.
                    // Buscamos el ultimo caracter '\\' 
                    for( k = strlen (lpInfDrag->szFileDrag) - 1; k > 0; k--)
                    {
                        if (lpInfDrag->szFileDrag [k] == '\\')
                        {
                            lpInfDrag->szFileDrag [k] = 0;
                            break;
                        }
                    }
                    // Si es el directorio raiz:
                    if (strlen (lpInfDrag->szFileDrag) < 3)
                    {
                        MessageBox (NULL, "No se acepta el directorio raiz", NULL, MB_OK);
                        return;
                    }
                    wMensaje = IDM_REUNIR;
                    goto ENVIAR_MENSAJE;
                }
                else
                {
                    if (!lstrcmpi (& lpInfDrag->szFileDrag [k + 1], "cx$"))
                        // Si es un archivo *.cx$ suponemos que el usuario quiere desencriptarlo
                        goto OP_DRAG_DESENCRIPTAR;
                    else
                    {
                        // En otro caso pedimos al usuario la accion a realizar
                        DialogBox (hInstance, MAKEINTRESOURCE(IDD_DRAGFILES), hwnd, (DLGPROC)OpcionDragDlg);
                        switch (lpInfDrag->nOpcion)
                        {
                            case DRAG_REUNIR:
                                // Es necesaro buscar el directorio al que pertenece
                                goto OP_DRAG_REUNIR;

                            case DRAG_PARTIR:
                                wMensaje = IDM_PARTIR;
                                goto ENVIAR_MENSAJE;

                            case DRAG_ENCRIPTAR:
                                wMensaje = IDM_ENCRIPTAR;
                                goto ENVIAR_MENSAJE;

                            case DRAG_DESENCRIPTAR:
OP_DRAG_DESENCRIPTAR:
                                wMensaje = IDM_DESENCRIPTAR;
                                goto ENVIAR_MENSAJE;

                            default:
                                return;
                        }

ENVIAR_MENSAJE:
                            * pg_bDrag = TRUE;
                            SendMessage (hwnd, WM_COMMAND, MAKEWPARAM (wMensaje, 0), 0);
                            * pg_bDrag = FALSE;
                    }
                }
            }
        }
    }
}



//  Esta funcion responde al mensaje enviado por el sistema WM_DRAWITEM
//  es la que controla lo que aparecera en el menu.
//  De manera aleatoria: First-chance exception in CODI.exe (GDI32.DLL): 0xC0000005: Access Violation.
//  (aun me aparece este mensaje de forma aleatoria (no al inicio) y no adivino por que)
//------------------------------------------------------------------------------------------
//  Version: 1.1 No me sale la excepcion esa en XP pero cuando depuro se congela el VC++ 6
//------------------------------------------------------------------------------------------

VOID OnDrawItem (HWND               hwnd,
                 LPDRAWITEMSTRUCT   lpdis)
{

LPMIMENUITEM    lpmItem = (LPMIMENUITEM) lpdis->itemData;
HBRUSH          hbrTargetOld;
HBRUSH          hbrBackground;
HBRUSH          hbr3DHiLight;
HBRUSH          hbr3DShadow;
HBITMAP         hbmpSourceOld;
HBITMAP         hbmpTargetOld;
HBITMAP         hBitmapOld;
HBITMAP         hbmpBitmapMenu;
BITMAP          bmBitmap;
HDC             hDcMem;
HDC             hdcSource;
HDC             hdcTarget;
COLORREF        clrPrevText;
COLORREF        crBackground;
COLORREF        cr3DHiLight;
COLORREF        cr3DShadow;
int             x, y;
int             oldSetBkMode;
WORD            wBitmapX;
WORD            wBitmapY;
SIZE            sz;

    //-----------------------------------------------------------------------------
    // Version: 1.1
    //-----------------------------------------------------------------------------
    // Ya no se agrandara el tamaño de estos menus de acuerdo al tamaño del menu
    // que tenga el sistema (se ve feo), pero si es mas grande se centrará.
    //-----------------------------------------------------------------------------
    int x0 = 0, y0 = 0;

    // Dibujar el texto transparente
    oldSetBkMode = SetBkMode(lpdis->hDC, TRANSPARENT);

    // Crear el bitmap para el menu:
    // ----------------------------
    // If this handle is NULL, the function creates a memory device context compatible with the application's current screen.
    hdcSource = CreateCompatibleDC ((HDC) NULL);
    hdcTarget = CreateCompatibleDC (hdcSource);

    // Get the menu background color and create a solid brush with that color. 
    crBackground = GetSysColor (COLOR_MENU);
    hbrBackground = CreateSolidBrush (crBackground);

    // Calculamos el tamaño
    wBitmapY = (UINT) (lpdis->rcItem.bottom - lpdis->rcItem.top);
    wBitmapX = wBitmapY + 1;

    // Crear bitmap para el menu (del tamaño esperado)
    hbmpBitmapMenu = CreateCompatibleBitmap (lpdis->hDC, wBitmapX, wBitmapY);   // No: hdcTarget

    // Select the background brush and bitmap into the target DC.
    hbrTargetOld = SelectObject (hdcTarget, hbrBackground);
    hbmpTargetOld = SelectObject (hdcTarget, hbmpBitmapMenu);
    
    // Use the selected brush to initialize the background color
    // of the bitmap in the target device context.
    PatBlt (hdcTarget, 0, 0, wBitmapX, wBitmapY, PATCOPY);
    
    // Cargamos el bitmap del menu
    hbmpSourceOld = SelectObject (hdcSource, lpmItem->hBitmap);

    // Llena una structura BITMAP con informacion acerca de tu mapa de bits de recurso
    GetObject (lpmItem->hBitmap, sizeof(BITMAP), &bmBitmap);

    //-----------------------------------------------------------------------------
    // Version: 1.1
    //-----------------------------------------------------------------------------
    x0 = wBitmapX - 16;
    if (x0 > 0)
    {
        x0 = (int) (x0 / 2);
    }

    y0 = wBitmapY - 16;
    if (y0 > 0)
    {
        y0 = (int) (y0 / 2);
    }
    //-----------------------------------------------------------------------------

    // Copiamos el bitmap al destino (el rojo es transparente)
    // (Dc destino - up X dest - up Y dest - W dest - H dest - DC orig - up X orig - up Y orig - W orig - H orig - color transparente)
    TransparentBlt (hdcTarget, x0 + 1, y0, bmBitmap.bmWidth, bmBitmap.bmHeight, hdcSource, 0, 0, bmBitmap.bmWidth, bmBitmap.bmHeight, RGB(255,0,0));

    // Hacer el efecto de sobresalte
    if (lpdis->itemState & ODS_SELECTED)
    {
        // Get the COLOR_3DLIGHT color and create a solid brush with that color. 
        cr3DHiLight = GetSysColor (COLOR_3DHILIGHT);
        hbr3DHiLight = CreateSolidBrush (cr3DHiLight);

        SelectObject (hdcTarget, hbr3DHiLight);
        PatBlt (hdcTarget, 0, 0, wBitmapX, 1, PATCOPY);
        PatBlt (hdcTarget, 0, 0, 1, wBitmapY, PATCOPY);
        DeleteObject (hbr3DHiLight);

        // Get the COLOR_3DSHADOW color and create a solid brush with that color. 
        cr3DShadow = GetSysColor (COLOR_3DSHADOW);
        hbr3DShadow = CreateSolidBrush (cr3DShadow);

        SelectObject (hdcTarget, hbr3DShadow);
        PatBlt (hdcTarget, wBitmapX - 1, 0, 1, wBitmapY, PATCOPY);
        PatBlt (hdcTarget, 0, wBitmapY - 1, wBitmapX, 1, PATCOPY);
        DeleteObject (hbr3DShadow);
    }

    // Select the old source and destination bitmaps into the
    // source and destination DCs, and then delete the DCs and 
    // the background brush.
    SelectObject (hdcSource, hbmpSourceOld);
    SelectObject (hdcTarget, hbrTargetOld); 
    hbmpBitmapMenu = SelectObject (hdcTarget, hbmpTargetOld);
    DeleteObject (hbrBackground);
    DeleteObject (hdcSource);
    DeleteObject (hdcTarget);

    // Simulamos el efecto de seleccionar el menu
    lpdis->rcItem.left += wBitmapX;
    if (lpdis->itemState & ODS_SELECTED)
    {
        FillRect(lpdis->hDC, &lpdis->rcItem, CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)));
        clrPrevText = SetTextColor(lpdis->hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
    }
    else
    {
        FillRect(lpdis->hDC, &lpdis->rcItem, CreateSolidBrush(GetSysColor(COLOR_MENU)));
        clrPrevText = SetTextColor(lpdis->hDC, GetSysColor (COLOR_MENUTEXT));
    }
    lpdis->rcItem.left -= wBitmapX;

    // Pinta el bitmap
    hDcMem = CreateCompatibleDC (lpdis->hDC);
    hBitmapOld = SelectObject (hDcMem, hbmpBitmapMenu);
    BitBlt (lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, wBitmapX, wBitmapY, hDcMem, 0, 0, SRCCOPY);
    SelectObject (hDcMem, hBitmapOld);
    DeleteDC (hDcMem);

    // Poner el texto:
    // --------------
    // Determinar la ubicacion para el texto
    x = wBitmapX + 2;
    GetTextExtentPoint32 (lpdis->hDC, lpmItem->szText, lpmItem->nLongText, &sz);
    y = lpdis->rcItem.top + (wBitmapY - sz.cy)/2;

    // Imprimir el texto del menú
    TextOut (lpdis->hDC, x, y, (LPSTR) lpmItem->szText, lpmItem->nLongText);

    SetBkMode(lpdis->hDC, oldSetBkMode);
    SetTextColor(lpdis->hDC, clrPrevText);

}



//  Esta funcion responde al mensaje enviado por el sistema WM_MEASUREITEM
//  El tamaño del menu puede variar, hay que tener cuidado
//  (al momento de depurar el debuguer no elimina el breakpoint y se baja el sistema)
//
VOID OnMeasureItem (HWND                hwnd,
                    LPMEASUREITEMSTRUCT lpmis)
{

LPMIMENUITEM    lpmItem = (LPMIMENUITEM) lpmis->itemData;
HMENU           hMenu;
RECT            rc;
LONG            lng;

    hMenu = GetMenu (hwnd);
    // Obtenemos el tamaño del primer submenu el cual usamos para estimar el tamaño del menu
    GetMenuItemRect (hwnd, hMenu, 0, &rc);
    lpmis->itemWidth = (UINT) (rc.right - rc.left);
    lng = rc.bottom - rc.top;
    if (lng < MIN_BMP_MENU)
        lpmis->itemHeight = MIN_BMP_MENU;
    else
        lpmis->itemHeight = (UINT) lng;

}



//  Esta funcion convierte los menus del recurso en owner-draw items.
//
VOID CrearMenuOwnerDrawn (HWND          hwnd,
                          HINSTANCE     hinst,
                          MIMENUITEM    menus[])
{

HMENU           hmenuBar = GetMenu (hwnd);
HMENU           hmenuPopup;
MENUITEMINFO    mii;
LPMIMENUITEM    lpmItem;
int             k;
                        // Indice menu raiz - ID Menu - ID Bitmap - ID Cadena
UINT            MD [] = {0, IDM_COMPARAR,  IDB_MENUCOMPARAR, IDS_MENUCOMPARAR,
                         0, IDM_BORRAR,  IDB_MENUBORRAR, IDS_MENUBORRAR,
                         1, IDM_ENCRIPTAR, IDB_MENUCRIPTO, IDS_MENUCRIPTO,
                         1, IDM_DESENCRIPTAR, IDB_MENUDESENCRIPTAR, IDS_MENUDESENCRIPTAR,
                         2, IDM_EXPLORAR, IDB_MENUEXPLORAR, IDS_MENUEXPLORAR,
                         3, IDM_COMPLEMENTOS, IDB_MENUCOMPLEMENTOS, IDS_MENUCOMPLEMENTOS,
                         4, IDM_CONTENIDO, IDB_MENUHELP, IDS_MENUCONTENIDO};


    for( k = 0; k < (sizeof(MD)/sizeof(UINT)) ; k += 4)
    {
        lpmItem = &menus [k/4];
        
        // Obtener handle del menu raiz del cual se origina el menu OD
        hmenuPopup = GetSubMenu (hmenuBar, MD[k]);

        // Llenar la estructura con el texto y longitud del texto
        LoadString (hinst, MD [k + 3], lpmItem->szText, MAX_LONG_MENU);
        lpmItem->nLongText = lstrlen (lpmItem->szText);

        // Asociar el mapa de bits
        lpmItem->hBitmap = (HBITMAP) LoadBitmap (hinst, MAKEINTRESOURCE(MD [k + 2]));

        // Modificar item
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_TYPE | MIIM_DATA;
        mii.fType = MFT_OWNERDRAW;
        mii.dwItemData = (DWORD) lpmItem;
        SetMenuItemInfo (hmenuPopup, MD [k + 1], FALSE, &mii);
    }
}



//  Esta funcion crea una barra de estado, lo inicializa y devuelve un manejador a ella.
//  InitCommonControls() debe haber sido llamada antes.
//
HWND CrearBarraEstado (HWND         hWnd, 
                       HINSTANCE    hInstance)
{

HWND hWndStatus;

    hWndStatus = CreateWindowEx (0,                         // extended style (nothing)
                                 STATUSCLASSNAME,           // create status bar
                                 (LPCTSTR) NULL,            // window title (nothing)
                                 WS_CHILD |WS_VISIBLE,      // window styles
                                 0, 0, 0, 0,                // x, y, width, height
                                 hWnd,                      // parent window
                                 (HMENU) ID_BARRA_ESTADO,   // Identificador de la ventana hija
                                 hInstance,                 // instance
                                 NULL);                     // window data

    if (hWndStatus == NULL)
        MessageBox (NULL, "No se pudo crear barra de estado", NULL, MB_OK);
    else
    {
        // Poner el texto normal de la barra de estado
        SendMessage (hWndStatus, SB_SIMPLE, (WPARAM) (BOOL) FALSE,0);
        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM)"Listo");
    }

    return (hWndStatus); 
}


//  Esta funcion crea el Rebar y lo inicializa, devuelve un manejador a ella.
//
HWND CrearRebar (HWND       hWnd, 
                 HINSTANCE  hInstance)
{

INITCOMMONCONTROLSEX    icex;
REBARINFO               rbi;
HWND                    hWndRebar;

    // Preparandonos para crear el Coolbar:
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES;
    InitCommonControlsEx(&icex);

    // Create the COOLBAR control.
    hWndRebar =  CreateWindowEx (WS_EX_TOOLWINDOW,
                                 REBARCLASSNAME,
                                 (LPCTSTR) NULL,
                                 WS_VISIBLE | WS_BORDER |
                                 WS_CHILD | WS_CLIPCHILDREN |
                                 WS_CLIPSIBLINGS | CCS_NODIVIDER |
                                 RBS_VARHEIGHT | RBS_BANDBORDERS,
                                 0, 0, 0, 0,
                                 hWnd,
                                 (HMENU)ID_REBAR,
                                 hInstance,
                                 NULL );

    if (hWndRebar == NULL)
        MessageBox (NULL, "No se pudo crear Rebar, necesitas la version 4.70 o superior de Comctl32.dll.", NULL, MB_OK);
    else
    {
        // Initialize and send the REBARINFO structure.
        rbi.cbSize = sizeof(REBARINFO);     // Required when using this struct.
        rbi.fMask  = 0;                     // No se pondra una imagen en el Coolbar
        rbi.himl   = (HIMAGELIST) NULL;
        if (!SendMessage (hWndRebar, RB_SETBARINFO, 0, (LPARAM)&rbi))
            return NULL;
    }

    return (hWndRebar);
}



//  Esta funcion crea un Toolbar, la inicializa y devuelve un manejador a ella.
//  InitCommonControls() debe haber sido llamada antes.
//
HWND CrearToolbar(HWND      hWnd,
                  HINSTANCE hInstance)
{

HIMAGELIST              himl, himlHot;
HWND                    hWndToolbar;
char                    szBuf[MAX_IDS];
int                     j[NUM_BOTONES];
TBBUTTON                tbb[NUM_BOTONES];


    // Create the toolbar control to be added.
    hWndToolbar = CreateWindowEx (WS_EX_TOOLWINDOW,
                                  TOOLBARCLASSNAME,
                                  (LPSTR) NULL, 
                                  WS_CHILD | TBSTYLE_FLAT |
                                  WS_CLIPCHILDREN | TBSTYLE_TOOLTIPS |
                                  WS_CLIPSIBLINGS | CCS_NORESIZE ,  // Si quieres un marco alrededor del toolbar: | WS_DLGFRAME ,
                                  0, 0, 0, 0,
                                  hWnd,
                                  NULL,     // se usara imagelist
                                  hInstance,
                                  NULL);

    if (hWndToolbar == NULL )
      MessageBox (NULL, "No se pudo crear barra de herramientas", NULL, MB_OK );
    else
    {
        // Send the TB_BUTTONSTRUCTSIZE message, which is required for backward compatibility. 
        SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

        // Agregamos las imagenes de los botones en estado NORMAL
        // Create, fill, and assign the image list for normal buttons.
        himl = ImageList_Create (TB_IMAGEX, TB_IMAGEY, ILC_COLOR8 | ILC_MASK, NUM_BOTONES, 0);      

        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL1)), RGB(255,255,0));
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL2)), RGB(255,255,0));   
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL3)), RGB(255,255,0));
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL4)), RGB(255,255,0));
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL6)), RGB(255,255,0));
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL5)), RGB(255,255,0));
        ImageList_AddMasked (himl, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOL7)), RGB(255,255,0));
        
        SendMessage(hWndToolbar, TB_SETIMAGELIST, 0, (LPARAM)himl);

        // Agregamos las imagenes de los botones en estado HOT
        // Create, fill, and assign the image list for hot buttons.
        himlHot = ImageList_Create (TB_IMAGEX, TB_IMAGEY, ILC_COLOR8 | ILC_MASK, NUM_BOTONES, 0); 

        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT1)), RGB(192,192,192));
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT2)), RGB(192,192,192));   
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT3)), RGB(192,192,192));
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT4)), RGB(192,192,192));
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT6)), RGB(192,192,192));
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT5)), RGB(192,192,192));
        ImageList_AddMasked (himlHot, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HOT7)), RGB(192,192,192));
        
        SendMessage(hWndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot);
      
        // Add the button strings to the toolbar. 
        LoadString (hInstance, IDS_PARTIR, (LPSTR) &szBuf, MAX_IDS);
        j[0] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf);
        
        LoadString (hInstance, IDS_REUNIR, (LPSTR) &szBuf, MAX_IDS); 
        j[1] = SendMessage (hWndToolbar, TB_ADDSTRING,0, (LPARAM) (LPSTR) szBuf); 

        LoadString (hInstance, IDS_ENCRIPTAR, (LPSTR) &szBuf, MAX_IDS); 
        j[2] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf); 

        LoadString (hInstance, IDS_DESENCRIPTAR, (LPSTR) &szBuf, MAX_IDS); 
        j[3] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf);

        LoadString (hInstance, IDS_CONTENIDO, (LPSTR) &szBuf, MAX_IDS); 
        j[4] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf);

        LoadString (hInstance, IDS_ACERCADE, (LPSTR) &szBuf, MAX_IDS);
        j[5] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf);

        LoadString (hInstance, IDS_SALIR, (LPSTR) &szBuf, MAX_IDS);
        j[6] = SendMessage (hWndToolbar, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuf);

        
        // Fill the TBBUTTON array with button information, and add the buttons to the toolbar. 
        tbb[0].iBitmap = 0; 
        tbb[0].idCommand = IDM_PARTIR; 
        tbb[0].fsState = TBSTATE_ENABLED; 
        tbb[0].fsStyle = TBSTYLE_BUTTON; 
        tbb[0].dwData = 0; 
        tbb[0].iString = j[0]; 

        tbb[1].iBitmap = 1; 
        tbb[1].idCommand = IDM_REUNIR; 
        tbb[1].fsState = TBSTATE_ENABLED; 
        tbb[1].fsStyle = TBSTYLE_BUTTON; 
        tbb[1].dwData = 0; 
        tbb[1].iString = j[1]; 

        tbb[2].iBitmap = 2; 
        tbb[2].idCommand = IDM_ENCRIPTAR; 
        tbb[2].fsState = TBSTATE_ENABLED; 
        tbb[2].fsStyle = TBSTYLE_BUTTON; 
        tbb[2].dwData = 0; 
        tbb[2].iString = j[2]; 

        tbb[3].iBitmap = 3; 
        tbb[3].idCommand = IDM_DESENCRIPTAR; 
        tbb[3].fsState = TBSTATE_ENABLED; 
        tbb[3].fsStyle = TBSTYLE_BUTTON; 
        tbb[3].dwData = 0; 
        tbb[3].iString = j[3]; 

        tbb[4].iBitmap = 4; 
        tbb[4].idCommand = IDM_CONTENIDO; 
        tbb[4].fsState = TBSTATE_ENABLED; 
        tbb[4].fsStyle = TBSTYLE_BUTTON; 
        tbb[4].dwData = 0; 
        tbb[4].iString = j[4]; 

        tbb[5].iBitmap = 5; 
        tbb[5].idCommand = IDM_ACERCADE; 
        tbb[5].fsState = TBSTATE_ENABLED; 
        tbb[5].fsStyle = TBSTYLE_BUTTON; 
        tbb[5].dwData = 0; 
        tbb[5].iString = j[5]; 

        tbb[6].iBitmap = 6; 
        tbb[6].idCommand = IDM_SALIR; 
        tbb[6].fsState = TBSTATE_ENABLED; 
        tbb[6].fsStyle = TBSTYLE_BUTTON; 
        tbb[6].dwData = 0; 
        tbb[6].iString = j[6];

        //  Inserta los botones en la barra de herramientas
        SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM) NUM_BOTONES, (LPARAM) (LPTBBUTTON) &tbb);
    }

    return (hWndToolbar);
}



//  Esta funcion inserta una banda al Rebar con el Toolbar contenido.
//  Se encarga del problema del tamaño inicial de los botones con texto.
//  Devuelve el tamaño inicial del Rebar
//
int InsertarToolbarAlRebar (HWND        hWnd,
                            HINSTANCE   hInstance,
                            HWND        hWndRebar, 
                            HWND        hWndToolbar)
{

LONG            lFontY;
LOGFONT         Font;
HDC             hdc;
TEXTMETRIC      tm;
HFONT           hFont;
HFONT           hFontOld;
REBARBANDINFO   rbBand;
int             iTopTree;


    //  NOTA: El tamaño del texto del Rebar si cambió cuando se cambió las fuentes del sistema 
    //  (Tamaño de la fuente asociado al icono) entonces tenemos que calcular el tamaño de los 
    //  botones en tiempo de ejecucion. Obtenemos el tamaño de la fuente de los iconos del sistema
    SystemParametersInfo (SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), & Font,  0);
    if ((Font.lfHeight) < 0) 
    {
        hdc = GetDC(hWnd);
        hFont = CreateFont (-(Font.lfHeight), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, FF_DONTCARE,  Font.lfFaceName);
        hFontOld = SelectObject (hdc, hFont);
        GetTextMetrics (hdc, &tm);
        lFontY = tm.tmHeight ;
        SelectObject (hdc,hFontOld);
        DeleteObject (hdc);
        DeleteObject (hFont);
    }
    else
        lFontY = Font.lfHeight;

    //  Especifica el tamaño de los botones MAKELONG(x,y) 
    SendMessage(hWndToolbar, TB_SETBUTTONSIZE, 0, (LPARAM) MAKELONG (TB_BOTONX,TB_IMAGEY + TB_ESPACIO + lFontY ));

    //  Especifica el tamaño de las imagenes de boton MAKELONG(x,y)
    SendMessage(hWndToolbar, TB_SETBITMAPSIZE, 0, (LPARAM) MAKELONG (TB_IMAGEX,TB_IMAGEY + TB_ESPACIO_I_T)); 

    
    //  Hace visible el toolbar
    ShowWindow(hWndToolbar,SW_SHOW);

    // Initialize REBARBANDINFO
    rbBand.cbSize = sizeof(REBARBANDINFO);
    rbBand.fMask =  RBBIM_CHILD |       // hwndChild is valid
                    RBBIM_CHILDSIZE |   // cxMinChild and cyMinChild are valid
                    RBBIM_STYLE |       // fStyle is valid
                    RBBIM_BACKGROUND;   // hbmBack is valid
    rbBand.fStyle = RBBS_FIXEDSIZE |    // The band can't be sized. With this style, the sizing grip is not displayed on the band
                    RBBS_FIXEDBMP;      // The background bitmap does not move when the band is resized
    rbBand.hbmBack = LoadBitmap (hInstance, MAKEINTRESOURCE(IDB_BACKGRND));
    rbBand.hwndChild = hWndToolbar;
    rbBand.cxMinChild = 0;
    
    // Aqui especificamos la altura del Rebar, el tm.tmInternalLeading me permite
    // ver el borde inferior del boton cuando paso el mouse de lo contrario se corta. 
    rbBand.cyMinChild = TB_IMAGEY + TB_ESPACIO + lFontY + tm.tmInternalLeading + 1; // Minimum width of the child window, in pixels. The band can't be sized smaller than this value 
    
    // Hallamos el top del Tree
    iTopTree = rbBand.cyMinChild + ESPACIO_R_T;

    // Insert band into coolbar.
    SendMessage(hWndRebar, RB_INSERTBAND, (WPARAM) (UINT) -1, (LPARAM)(LPREBARBANDINFO)&rbBand);

    return (iTopTree);
}



//  Esta funcion crea un control Treeview, lo inicializa y devuelve un manejador a el.
//  Para esto crea un control ImageList y lo inicializa.
//  InitCommonControls() debe haber sido llamada antes.
//
HWND CrearTreeview (HWND        hWnd,
                    HINSTANCE   hInstance,
                    HWND        hWndStatus,
                    int         iTopTree,
                    int         gibmpTV[])
{

HIMAGELIST  hIml;               // handle to image list
HBITMAP     hBmp;               // handle to bitmap 
HWND        hWndTree;
RECT        rc;
RECT        rcSB;

    // Get the size and position of the parent window.
    GetClientRect (hWnd, &rc);
    GetClientRect (hWndStatus, &rcSB);

    // Create the tree view window.
    hWndTree = CreateWindowEx ( 0,
                                WC_TREEVIEW,                // tree view class
                                (LPCTSTR) NULL,             // no default text
                                WS_VISIBLE | WS_CHILD |
                                TVS_HASLINES | TVS_HASBUTTONS |
                                TVS_LINESATROOT |TVS_DISABLEDRAGDROP ,
                                TV_MARGEN_IZQ,
                                iTopTree,
                                rc.right - TV_MARGEN_IZQ,
                                rc.bottom - iTopTree - rcSB.bottom,
                                hWnd,
                                (HMENU)ID_TREEVIEW,
                                hInstance,
                                NULL);

    if (hWndTree == NULL)
        MessageBox (NULL, "No se pudo crear TreeView", NULL, MB_OK);
    else
    {
        // Initialize the tree view window.
        // First create the image list you will need.
        hIml = ImageList_Create (BITMAP_WIDTH,          // width
                                 BITMAP_HEIGHT,         // height
                                 ILC_MASK,              // IMPORTANTE si quieres imagenes transparentes
                                 NUM_BITMAPS_TV,        // number of images
                                 0);                    // amount this list can grow
        if (hIml == NULL)
        {
            MessageBox (NULL, "No se pudo crear ImageList", NULL, MB_OK);
            return NULL;
        }

        // Load the bitmaps and add them to the image list.
        // Las imagenes tienen un fondo azul que se vera transparente.
        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_TAREA));
        gibmpTV[0] = ImageList_AddMasked (hIml, hBmp, RGB(0, 0, 255));
        DeleteObject(hBmp); 

        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_ARCHIVO_FUENTE));
        gibmpTV[1] = ImageList_AddMasked (hIml, hBmp, RGB(0, 0, 255));
        DeleteObject(hBmp); 

        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_CARPETA_CERRADA));
        gibmpTV[2] = ImageList_AddMasked (hIml, hBmp, RGB(0, 0, 255));
        DeleteObject(hBmp); 

        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_ARCHIVO_CORTADO));
        gibmpTV[3] = ImageList_AddMasked (hIml, hBmp, RGB(0, 0, 255));
        DeleteObject(hBmp);

        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_CODI));
        gibmpTV[4] = ImageList_AddMasked (hIml, hBmp, RGB(0, 0, 255));
        DeleteObject(hBmp);

        hBmp = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_MENUCOMPARAR));
        gibmpTV[5] = ImageList_AddMasked (hIml, hBmp, RGB(255, 0, 0));
        DeleteObject(hBmp);

        // Be sure that all the bitmaps were added.
        if (ImageList_GetImageCount (hIml) < NUM_BITMAPS_TV) 
            return NULL;
        
        // Associate the image list with the tree view control.
        TreeView_SetImageList (hWndTree, hIml, TVSIL_NORMAL);
            
        // Si queremos especificar una mayor separacion para los items del Treeview:
        //      TreeView_SetItemHeight(hWndTree, TV_ESPACIOY + lFontY);
        // Donde lFontY es la altura de la fuente para los iconos del sistema.
        // y TV_ESPACIOY un valor adicional (aqui fue 7)
    }

    return hWndTree;
}



//  Esta funcion crea una barra de progreso y devuelve un manejador a ella.
//  InitCommonControls() debe haber sido llamada antes.
//
HWND CrearProgressBar (HWND         hwndParent,
                       HINSTANCE    hinst,
                       HWND         hwndStatusBar) 
{
    
RECT rcClient;  // client area of parent window
RECT rcSB;
HWND hwndPB;    // handle of progress bar 
 
    GetClientRect (hwndParent, &rcClient);
    GetClientRect (hwndStatusBar, &rcSB);
    hwndPB = CreateWindowEx (0,
                             PROGRESS_CLASS,
                             (LPSTR) NULL, 
                             WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                             0,
                             rcClient.bottom - rcSB.bottom,
                             rcClient.right,
                             rcSB.bottom, 
                             hwndParent,
                             (HMENU) 0,
                             hinst,
                             NULL); 

    return (hwndPB);
}

 

BOOL FinalizarPartirTreeView (HWND              hwndTree,
                              int               *gibmpTV,
                              LPWIZPARTIRINFO   lpwpi)
{

HTREEITEM   hTRoot;
HTREEITEM   hTFuente;
HTREEITEM   hTFolder;
HTREEITEM   hT;
int         t, k;
SYSTEMTIME  st;
char        sz [40];

    
    GetLocalTime (& st);
    wsprintf (sz, "TAREA : Fragmentar   [%d:%d:%d]", st.wHour, st.wMinute, st.wSecond); 
    hTRoot = AddOneItem ((HTREEITEM)NULL, sz, (HTREEITEM)TVI_ROOT, gibmpTV[0], gibmpTV[0], hwndTree);
    // Nos pasaron la ruta completa: buscamos el ultimo caracter '\\'
    for( k = strlen (lpwpi->szNombreArchivoFuente) - 1; k > 0; k--)
    {
        if (lpwpi->szNombreArchivoFuente [k] == '\\')
        {
            lpwpi->szNombreArchivoFuente [k] = 0;
            break;
        }
    }
    hTFolder = AddOneItem (hTRoot, lpwpi->szNombreArchivoFuente, (HTREEITEM)TVI_FIRST, gibmpTV[2], gibmpTV[2], hwndTree);
    hTFuente = AddOneItem (hTFolder, & lpwpi->szNombreArchivoFuente [k + 1], (HTREEITEM)TVI_FIRST, gibmpTV[1], gibmpTV[1], hwndTree);
    SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hTFuente);
    hTFolder = AddOneItem (hTRoot, lpwpi->szCarpetaDestino , (HTREEITEM)TVI_LAST, gibmpTV[2], gibmpTV[2], hwndTree);
    // Agregar los nombres de los archivos generados:
    t = lstrlen (lpwpi->szNombreArchivoGenerico);
    for( k = 0; k < lpwpi->iNumArch; k++)
    {
        lpwpi->szNombreArchivoGenerico[t-6] = (char) (48 + (k / 10));
        lpwpi->szNombreArchivoGenerico[t-5] = (char) (48 + (k % 10));
        hT = AddOneItem (hTFolder, lpwpi->szNombreArchivoGenerico, (HTREEITEM)TVI_LAST, gibmpTV[3], gibmpTV[3], hwndTree);
    }
    SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hT);
    return TRUE;
}



BOOL FinalizarReunirTreeView (HWND              hwndTree,
                              int               *gibmpTV,
                              LPWIZREUNIRINFO   lpwri)
{

HTREEITEM   hTRoot;
HTREEITEM   hTDestino;
HTREEITEM   hTFolder;
HTREEITEM   hT;
int         k;
SYSTEMTIME  st;
char        sz [40];

    
    GetLocalTime (& st);
    wsprintf (sz, "TAREA : Defragmentar   [%d:%d:%d]", st.wHour, st.wMinute, st.wSecond);
    hTRoot = AddOneItem ((HTREEITEM)NULL, sz, (HTREEITEM)TVI_ROOT, gibmpTV[0], gibmpTV[0], hwndTree);
    hTFolder = AddOneItem (hTRoot, lpwri->szCarpetaFuente, (HTREEITEM)TVI_FIRST, gibmpTV[2], gibmpTV[2], hwndTree);
    // Agregar los nombres de los archivos encontrados:
    for( k = 0; k < lpwri->iNumArch; k++)
    {
        hT = AddOneItem (hTFolder, lpwri->Archivos[k], (HTREEITEM)TVI_LAST, gibmpTV[3], gibmpTV[3], hwndTree);
    }
    SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hT);
    if (lpwri->bAutomatico)
    {
        // Si es automatico se mostrara el archivo creado dentro de la carpeta:
        hTDestino = AddOneItem (hTFolder, lpwri->szNombreArchivoDestino, (HTREEITEM)TVI_LAST, gibmpTV[1], gibmpTV[1], hwndTree);
        SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hTDestino);
    }
    else
    {
        // Si no, se creará una nueva carpeta y alli se anexara el nombre
        // Nos pasaron la ruta completa: buscamos el ultimo caracter '\\' 
        for( k = strlen (lpwri->szNombreArchivoDestino) - 1; k > 0; k--)
        {
            if (lpwri->szNombreArchivoDestino [k] == '\\')
            {
                lpwri->szNombreArchivoDestino [k] = 0;
                break;
            }
        }
        hTFolder = AddOneItem (hTRoot, lpwri->szNombreArchivoDestino, (HTREEITEM)TVI_LAST, gibmpTV[2], gibmpTV[2], hwndTree);
        hTDestino = AddOneItem (hTFolder, & lpwri->szNombreArchivoDestino [k + 1], (HTREEITEM)TVI_LAST, gibmpTV[1], gibmpTV[1], hwndTree);
        SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hTDestino);
    }
    return TRUE;
}



BOOL CriptoTreeView (HWND               hwndTree,
                     int                *gibmpTV,
                     LPWIZCRIPTOINFO    lpwci)
{

HTREEITEM   hTRoot;
HTREEITEM   hTDestino;
HTREEITEM   hTFuente;
SYSTEMTIME  st;
char        sz [MAX_PATH + 20];

    
    GetLocalTime (& st);
    if (lpwci->bEstamosEncriptando)
    {
        wsprintf (sz, "TAREA : Encriptar   [%d:%d:%d]", st.wHour, st.wMinute, st.wSecond);
        hTRoot = AddOneItem ((HTREEITEM)NULL, sz, (HTREEITEM)TVI_ROOT, gibmpTV[0], gibmpTV[0], hwndTree);
        wsprintf (sz, "[original]  %s", lpwci->szNombreArchivoFuente);
        hTFuente = AddOneItem (hTRoot, sz, (HTREEITEM)TVI_FIRST, gibmpTV[1], gibmpTV[1], hwndTree);
        wsprintf (sz, "[encriptado]  %s", lpwci->szNombreArchivoDestino);
        hTDestino= AddOneItem (hTRoot, sz, (HTREEITEM)TVI_LAST, gibmpTV[4], gibmpTV[4], hwndTree);
    }
    else
    {
        wsprintf (sz, "TAREA : Desencriptar   [%d:%d:%d]", st.wHour, st.wMinute, st.wSecond);
        hTRoot = AddOneItem ((HTREEITEM)NULL, sz, (HTREEITEM)TVI_ROOT, gibmpTV[0], gibmpTV[0], hwndTree);
        wsprintf (sz, "[encriptado]  %s", lpwci->szNombreArchivoFuente);
        hTFuente = AddOneItem (hTRoot, sz, (HTREEITEM)TVI_FIRST, gibmpTV[4], gibmpTV[4], hwndTree);
        wsprintf (sz, "[desencriptado]  %s", lpwci->szNombreArchivoDestino);
        hTDestino= AddOneItem (hTRoot, sz, (HTREEITEM)TVI_LAST, gibmpTV[1], gibmpTV[1], hwndTree);
    }
    SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hTDestino);
    return TRUE;
}



//  Esta funcion agrega un Item al Treeview.
//
HTREEITEM AddOneItem (HTREEITEM hParent,
                      LPSTR     szText,
                      HTREEITEM hInsAfter,
                      int       iImage,
                      int       iImageSelected,
                      HWND      hwndTree)
{
HTREEITEM hItem;
TV_ITEM tvI;
TV_INSERTSTRUCT tvIns;

    // The pszText, iImage, and iSelectedImage members are filled out.
    tvI.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    tvI.pszText = szText;
    tvI.cchTextMax = lstrlen (szText);
    tvI.iImage = iImage;
    tvI.iSelectedImage = iImageSelected;

    tvIns.item = tvI;
    tvIns.hInsertAfter = hInsAfter;
    tvIns.hParent = hParent;

    // Insert the item into the tree.
    hItem = (HTREEITEM) SendMessage (hwndTree, TVM_INSERTITEM, 0,(LPARAM)(LPTV_INSERTSTRUCT)&tvIns);

    return (hItem);
} 



//  Esta funcion crea el Wizard para el comando Partir
//  
VOID CrearWizardPartir (HWND        hwndOwner,
                        HINSTANCE   hInstance)
{

PROPSHEETPAGE psp[NUM_WIZ_PARTIR];
PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hInstance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_BUSCAR_ARCHIVO);
    psp[0].pfnDlgProc = BuscarArchivoPartirDialogProc;
    psp[0].lParam = 0;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hInstance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_NUMERO_ARCHIVOS);
    psp[1].pfnDlgProc = NumeroArchivosPartirDialogProc;
    psp[1].lParam = 0;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = 0;
    psp[2].hInstance = hInstance;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_FINALIZAR);
    psp[2].pfnDlgProc = FinalizarPartirDialogProc;
    psp[2].lParam = 0;
   
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInstance;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;

    PropertySheet(&psh);

    return;
}



//  Esta funcion crea el Wizard para el comando Reunir
//  
VOID CrearWizardReunir (HWND        hwndOwner,
                        HINSTANCE   hInstance)
{

PROPSHEETPAGE psp[NUM_WIZ_REUNIR];
PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hInstance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_BUSCAR_CARPETA);
    psp[0].pfnDlgProc = BuscarCarpetaReunirDialogProc;
    psp[0].lParam = 0;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hInstance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_FINALIZAR);
    psp[1].pfnDlgProc = FinalizarReunirDialogProc;
    psp[1].lParam = 0;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInstance;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;

    PropertySheet(&psh);

    return;
}



//  Esta funcion crea el Wizard para el comando Cripto
//  
VOID CrearWizardCripto (HWND        hwndOwner,
                        HINSTANCE   hInstance)
{

PROPSHEETPAGE psp[NUM_WIZ_CRIPTO];
PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hInstance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_CRIPTO);
    psp[0].pfnDlgProc = CriptoDialogProc;
    psp[0].lParam = 0;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hInstance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_FINALIZAR);
    psp[1].pfnDlgProc = FinalizarCriptoDialogProc;
    psp[1].lParam = 0;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInstance;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;

    PropertySheet(&psh);

    return;
}



//  Esta funcion crea el Wizard para el comando Password
//  
VOID CrearWizardPassword (HWND      hwndOwner,
                          HINSTANCE hInstance)
{

PROPSHEETPAGE psp[2];
PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hInstance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PASSWORD);
    psp[0].pfnDlgProc = PasswordDialogProc;
    psp[0].lParam = 0;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hInstance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_FINALIZAR);
    psp[1].pfnDlgProc = FinalizarPasswordDialogProc;
    psp[1].lParam = 0;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInstance;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;

    PropertySheet(&psh);

    return;
}



//  Esta funcion muestra el dialogo comun Guardar Como
//  Devuelve un puntero a la estructura con informacion
//
BOOL MostrarDlgComunGuardar (HWND           hwndOwner,
                             OPENFILENAME   *ofn,
                             LPTSTR         lpstrFile,
                             DWORD          nMaxFile,
                             LPCTSTR        lpstrTitulo,
                             LPCTSTR        lpstrFiltro,
                             LPCTSTR        lpstrDirectorioInicial,
                             LPCTSTR        lpstrDefExt,
                             DWORD          Flags)
{

    // Initialize OPENFILENAME
    ZeroMemory (ofn, sizeof(OPENFILENAME));
    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->hwndOwner = hwndOwner;
    ofn->lpstrTitle = lpstrTitulo;
    ofn->lpstrFilter = lpstrFiltro;
    ofn->nFilterIndex = 1;
    ofn->lpstrFile = lpstrFile;
    ofn->nMaxFile = nMaxFile;
    ofn->lpstrInitialDir = lpstrDirectorioInicial;
    ofn->lpstrDefExt = lpstrDefExt;
    ofn->Flags = Flags;

    // Display the Save dialog box. 
    return (GetSaveFileName (ofn));
}



//  Esta funcion muestra el dialogo comun Abrir Archivo
//  Devuelve un puntero a la estructura con informacion
//
BOOL MostrarDlgComunAbrir (HWND         hwndOwner,
                           OPENFILENAME *ofn,
                           LPTSTR       lpstrFile,
                           DWORD        nMaxFile,
                           LPCTSTR      lpstrTitulo,
                           LPCTSTR      lpstrFiltro,
                           LPCTSTR      lpstrDirectorioInicial,
                           DWORD        Flags)
{

    // Initialize OPENFILENAME
    ZeroMemory (ofn, sizeof(OPENFILENAME));
    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->hwndOwner = hwndOwner;
    ofn->lpstrTitle = lpstrTitulo;
    ofn->lpstrFile = lpstrFile;
    ofn->nMaxFile = nMaxFile;
    ofn->lpstrInitialDir = lpstrDirectorioInicial;
    ofn->lpstrFilter = lpstrFiltro;
    ofn->nFilterIndex = 1;
    ofn->Flags = Flags;

    // Display the Open dialog box. 
    return (GetOpenFileName (ofn));
}



VOID CrearNombreGenerico (char nombre[],
                          WORD ExtOffset,
                          WORD FileOffset)
{

int     k, t;
char    ext [5], cad [8] = "_00.cd$";


    if (!ExtOffset)
    {
        t = lstrlen (nombre);
        // Cuando el archivo no tiene extension
        if (t > MAX_PATH - 20)
            strcpy (nombre, "ArchivoFrag_00.cd$");
        else
            strcat (nombre, cad);
    }
    else
    {
        k = (int) (ExtOffset - FileOffset);
        if (k > MAX_PATH - 20)
        {
            // Solo los primeros cuatro caracteres de la extension
            lstrcpyn (ext, &nombre [k], 4);
            strcpy (nombre, "ArchivoFrag_");
            strcat (nombre, ext);
            strcat (nombre, cad);
        }
        else
        {   
            nombre [k - 1] = '_';
            strcat (nombre, cad);
        }
    }
}



//  Pedir dos archivos para compararlos y comparar byte por byte
//  Crea una barra de progreso para indicar el avance
//
BOOL CompararArchivos (HWND             hwnd,
                       HINSTANCE        hinst,
                       HWND             hwndStatus,
                       LPINFOCOMPARAR   lpic)
{

OPENFILENAME    ofn;
char            szFile [MAX_PATH + 1];
BOOL            bTemp;
BOOL            bSkip = FALSE;
HANDLE          hArchivo1;
HANDLE          hArchivo2; 
char            *buff1 = NULL;
char            *buff2 = NULL;
DWORD           dwTamanyo1;
DWORD           dwTamanyo2;
DWORD           dwTamanyoHigh;
DWORD           dw, dw1, dw2;
DWORD           dwRange;
int             k, nByts, n = 0;
HWND            hwndProgress = NULL;


    // Pedir primer archivo
    ZeroMemory (szFile, sizeof(szFile));
    bTemp = MostrarDlgComunAbrir (hwnd,
                                  & ofn,
                                  szFile,
                                  sizeof(szFile),
                                  "Selecciona el primer archivo a comparar:",
                                  "Todos los archivos\0*.*\0",
                                  NULL,
                                  OFN_FILEMUSTEXIST |
                                  OFN_PATHMUSTEXIST |
                                  OFN_HIDEREADONLY);
    if (bTemp)
    {
        strcpy (lpic->szFile1, ofn.lpstrFile);

        // Pedir segundo archivo
        ZeroMemory (szFile, sizeof(szFile));
        bTemp = MostrarDlgComunAbrir (hwnd,
                                      & ofn,
                                      szFile,
                                      sizeof(szFile),
                                      "Selecciona el segundo archivo a comparar:",
                                      "Todos los archivos\0*.*\0",
                                      NULL,
                                      OFN_FILEMUSTEXIST |
                                      OFN_PATHMUSTEXIST |
                                      OFN_HIDEREADONLY);
        if (bTemp)
        {
            strcpy (lpic->szFile2, ofn.lpstrFile);

            if (!lstrcmpi (lpic->szFile1, lpic->szFile2))
            {
                MessageBox (NULL, "Archivos identicos", "CODI", MB_OK);
                return FALSE;
            }
            else
            {
                UpdateWindow (hwnd);
                
                // Abrimos los archivos para lectura
                hArchivo1 = CreateFile (lpic->szFile1,
                                        GENERIC_READ,
                                        0,              //No se permite compartir
                                        NULL,           //No seguridad
                                        OPEN_EXISTING,  //Falla si no existe
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);
                if (hArchivo1 == INVALID_HANDLE_VALUE)
                {   
                    MessageBox (NULL,"El primer archivo no pudo ser abierto. Verifica que existe y NO esta siendo usado", NULL, MB_OK | MB_ICONERROR);
                    return FALSE;
                }
                else
                {
                    hArchivo2 = CreateFile (lpic->szFile2,
                                            GENERIC_READ,
                                            0,              //No se permite compartir
                                            NULL,           //No seguridad
                                            OPEN_EXISTING,  //Falla si no existe
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL);
                    if (hArchivo2 == INVALID_HANDLE_VALUE)
                    {   
                        MessageBox (NULL,"El segundo archivo no pudo ser abierto. Verifica que existe y NO esta siendo usado", NULL, MB_OK | MB_ICONERROR);
                        CloseHandle (hArchivo1);
                        return FALSE;
                    }
                    else
                    {
                        // Comparamos tamaños
                        dwTamanyo1 = GetFileSize (hArchivo1, & dwTamanyoHigh);
                        if (dwTamanyo1 != 0xFFFFFFFF)
                        {
                            if (dwTamanyoHigh != 0)
                            {
ERR_TAMANYO:
                                MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Error",MB_OK | MB_ICONERROR);
ERR_SALIR:
                                if (hwndProgress)
                                    DestroyWindow(hwndProgress);
                                CloseHandle (hArchivo1);
                                CloseHandle (hArchivo2);
                                return FALSE;
                            }
                        }
                        else
                        {
                            if ( (dw = GetLastError()) != NO_ERROR)
                            {
ERR_INDEF_TAM:
                                MessageBox (NULL,"No se pudo calcular el tamaño del archivo.", "Error",MB_OK | MB_ICONERROR);
                                goto ERR_SALIR;
                            }
                        }

                        dwTamanyo2 = GetFileSize (hArchivo2, & dwTamanyoHigh);
                        if (dwTamanyo2 != 0xFFFFFFFF)
                        {
                            if (dwTamanyoHigh != 0)
                                goto ERR_TAMANYO;
                        }
                        else
                        {
                            if ( (dw = GetLastError()) != NO_ERROR)
                                goto ERR_INDEF_TAM;
                        }

                        if (dwTamanyo1 != dwTamanyo2)
                        {
                            lpic->bIgual = FALSE;
EXIT_TRUE:
                            if (hwndProgress)
                                DestroyWindow(hwndProgress);
                            if (buff1)
                                free (buff1);
                            if (buff2)
                                free (buff2);
                            CloseHandle (hArchivo1);
                            CloseHandle (hArchivo2);
                            return TRUE;
                        }
                        else
                        {
                            // Asignamos memoria para los buffers
                            buff1 = (char*) malloc (NUM_BYTS);
                            if (!buff1)
                            {
ERR_MEMORIA:
                                MessageBox (NULL, "Insuficiente memoria", NULL, MB_OK | MB_ICONERROR);
                                goto ERR_SALIR;
                            }

                            buff2 = (char*) malloc (NUM_BYTS);
                            if (!buff2)
                            {
                                free (buff1);
                                goto ERR_MEMORIA;
                            }

                            // Creamos barra de progreso
                            hwndProgress = CrearProgressBar (hwnd, hinst, hwndStatus);
                            if (hwndProgress)
                            {
                                // Set the range and increment of the progress bar.
                                dwRange = (dwTamanyo1 / NUM_BYTS);
                                if (dwRange > 65000)
                                {   
                                    // Si el archivo pasa de los 250 MB
                                    dwRange /= 10;
                                    bSkip = TRUE;
                                    dwRange ++;
                                }
                                dwRange ++;
                                SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, LOWORD(dwRange)));
                                SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM) 1, 0);
                                SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
                            }
                            
                            // Leemos los bytes de ambos archivos y comparamos
                            do
                            {
                                if (ReadFile (hArchivo1, buff1, NUM_BYTS, &dw1, NULL))
                                {
                                    if (ReadFile (hArchivo2, buff2, NUM_BYTS, &dw2, NULL))
                                    {
                                        if (dw1 != dw2)
                                        {
                                            MessageBox (NULL, "Error interno", "Comparar", MB_OK | MB_ICONERROR);
                                            goto ERR_SALIR;
                                        }
                                        else
                                        {
                                            if (dw1 < NUM_BYTS)
                                                nByts = dw1;
                                            else
                                                nByts = NUM_BYTS;
                                        }
                                        
                                        for( k = 0; k < nByts; k++)
                                        {
                                            if (buff1 [k] != buff2 [k])
                                            {
                                                lpic->bIgual = FALSE;
                                                goto EXIT_TRUE;
                                            }
                                        }
                                        
                                        // Refrescamos barra de progreso
                                        if (!bSkip)
                                            SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
                                        else
                                        {
                                            n++;
                                            if (n >= 10)
                                            {
                                                SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
                                                n = 0;
                                            }
                                        }
                                    }
                                    else
                                        goto ERR_LECTURA;
                                }
                                else
                                {
ERR_LECTURA:
                                    MessageBox (NULL, "Error de lectura", "Comparar", MB_OK | MB_ICONERROR);
                                    goto ERR_SALIR;
                                }
                            }while (dw1 == NUM_BYTS);

                            lpic->bIgual = TRUE;
                            goto EXIT_TRUE;
                        }
                    }
                }
            }
        }
        else
            return FALSE;
    }
    else
        return FALSE;

}



VOID CompararTreeView (HWND             hwndTree,
                       int              *gibmpTV,
                       LPINFOCOMPARAR   lpic)
{

HTREEITEM   hTRoot;
HTREEITEM   hTArchivo1;
HTREEITEM   hTArchivo2;
HTREEITEM   hTResultado;
SYSTEMTIME  st;
char        sz [MAX_PATH + 15];

    
    GetLocalTime (& st);
    wsprintf (sz, "TAREA : Comparar   [%d:%d:%d]", st.wHour, st.wMinute, st.wSecond);
    hTRoot = AddOneItem ((HTREEITEM)NULL, sz, (HTREEITEM)TVI_ROOT, gibmpTV[0], gibmpTV[0], hwndTree);
    wsprintf (sz, "[archivo 1]  %s", lpic->szFile1);
    hTArchivo1 = AddOneItem (hTRoot, sz, (HTREEITEM)TVI_FIRST, gibmpTV[1], gibmpTV[1], hwndTree);
    wsprintf (sz, "[archivo 2]  %s", lpic->szFile2);
    hTArchivo2 = AddOneItem (hTRoot, sz, (HTREEITEM)TVI_LAST, gibmpTV[1], gibmpTV[1], hwndTree);
    if (lpic->bIgual)
        strcpy(sz, "RESULTADO : IGUALES");
    else
        strcpy(sz, "RESULTADO : DIFERENTES");
    hTResultado = AddOneItem (hTRoot, sz, (HTREEITEM)TVI_LAST, gibmpTV[5], gibmpTV[5], hwndTree);
    SendMessage (hwndTree, TVM_SELECTITEM, (WPARAM) TVGN_FIRSTVISIBLE ,(LPARAM) (HTREEITEM) hTResultado);   
    return;
}
