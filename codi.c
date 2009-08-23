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
//***** ESAU ABRIL 2001 ARCHIVO FUENTE PRINCIPAL DE <CODI> ******
//***************************************************************
//  Este programa tendra por objetivo partir archivos en un
//  tamaño fijado por el usuario ademas de permitir su
//  encriptacion.   Debe tratar de ser pequeño (No MFC) y confiable.

#include "CODI.h"       // includes the application-specific information

//  ******************
//  VARIABLES GLOBALES
//  ******************

HWND hWndMain;          // Manejador de la ventana Principal Global
HINSTANCE hInstance;    // Manejador de la instancia principal


HWND hWndStatus;        // Manejador de la barra de estado
HWND hWndRebar;         // Manejador del CoolBar
HWND hWndToolbar;       // Manejador del ToolBar incrustado en el CoolBar
HWND hWndTree;          // Manejador del TreeView


int gibmpTV[NUM_BITMAPS_TV];    // indices de imagenes para el TreeeView

char    *gs_CODICurrentCrypSystem = "CriptoCodi 1.0";

WIZPARTIRINFO    wpi;   // Informacion del wizard Partir
WIZREUNIRINFO    wri;   // Informacion del wizard Reunir
WIZCRIPTOINFO    wci;   // Informacion del wizard Cripto
WIZPASSWORDINFO  wpwi;  // Informacion del wizard Password

LPMALLOC    g_pMalloc;  // Global pointer to the shell's IMalloc interface. 

HGLOBAL     hGout;      // Para pegar al clipboard

char        szPath[MAX_PATH + 1];   // Para guardar el directorio inicial de ejecucion

MIMENUITEM      menus [NUM_MENUS_OD];

INFODRAG    g_InfDrag;  // Valores de arrastre
BOOL        g_bDrag;    // Indicador para la opcion de arratre 

//  ***************************
//  FUNCIONES DE INICIALIZACION
//  ***************************

//   FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
//   PURPOSE: Main entry point for the application. 
//   This function calls the initialization functions and processes
//   the main message loop.
//
int APIENTRY WinMain (HINSTANCE hInst,
                      HINSTANCE hPrevInstance,
                      LPSTR     lpCmdLine,
                      int       nCmdShow)
{
    MSG msg;
    HACCEL hAccel;
        
    InitCommonControls();   //Para inicializar controles comunes

    hInstance = hInst;      //Necesario porque se vuelve a llamar a la instancia fuera de esta funcion  

    // if the initialization fails, return.
    if (!InitApplication(hInstance)) return (FALSE);     

    // Perform initializations that apply to a specific instance 
    if (!InitInstance(hInstance, nCmdShow)) return (FALSE);

    hAccel= LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR));

    // Acquire and dispatch messages until a WM_QUIT message is received. 
    while (GetMessage(&msg,NULL,0,0))                 
    {
        if (!TranslateAccelerator(hWndMain,hAccel,&msg))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg); 
        }
    }
    return (msg.wParam);  
}


//   FUNCTION: InitApplication(HANDLE) 
//   PURPOSE: Initializes window data and registers window class 
//   This function registers the window class for the main window.  
// 
BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wcCODI;

    wcCODI.style = 0;                     
    wcCODI.lpfnWndProc = (WNDPROC)MainWndProc;  // Puntero a la funcion que maneja
                                                // los procedimientos de la ventana
    wcCODI.cbClsExtra = 0;              
    wcCODI.cbWndExtra = 0;              
    wcCODI.hInstance = hInstance;       
    wcCODI.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CODI_ICON));
    wcCODI.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcCODI.hbrBackground =  (HBRUSH) (COLOR_WINDOW + 1);    // Color de la ventana del usuario.
    wcCODI.lpszMenuName =  MAKEINTRESOURCE(IDR_CODI_MENU);  // El nombre del menu de la ventana
    wcCODI.lpszClassName = "CODIWClass"; 

    return (RegisterClass(&wcCODI));

}

//   FUNCTION: InitInstance(HANDLE, int)
//   PURPOSE: Creates the main window.
// 
BOOL InitInstance (HANDLE hInstance,
                   int    nCmdShow) 
{
    
    hWndMain = CreateWindow("CODIWClass",           //El nombre de la clase
                            "CODI",                 //Nombre de la ventana
                            WS_OVERLAPPEDWINDOW,    //estilo
                            100, 75,                //x,y
                            TB_BOTONX * NUM_BOTONES + 10,       //ancho inicial
                            400,                    //alto
                            NULL,               
                            NULL,               
                            hInstance,              //La instancia que Windows(AGH!) nos paso
                            NULL);

    // If window could not be created, return "failure" 
    if (!hWndMain) return (FALSE);

    // Make the window visible; update its client area; and return "success" 
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain); 
    return (TRUE);      

}


//  ******************************
//  FUNCION PRINCIPAL DEL PROGRAMA
//  ******************************

//  FUNCTION: MainWndProc(HWND, UINT, UINT, LONG)
//  PURPOSE:  Processes messages for the main window procedure 
//
//  MESSAGES:
//  
//  WM_CREATE - creates the main window and his child windows.
//  WM_COMMAND - processes the menu commands for the application.
//  WM_SIZING - message is sent to a window that the user is resizing.
//  WM_SIZE - The WM_SIZE message is sent to a window after its size has changed.
//  WM_DESTROY - posts a quit message and returns.
//  WM_MENUSELECT - message is sent to a menu's owner window when the user selects a menu item.
//  WM_MEASUREITEM - is sent to the owner window of an owner-drawn menu item when is created.
//  WM_DRAWITEM - is sent to the owner window of an owner-drawn menu when a visual aspect has changed.
//  WM_EXITMENULOOP - informs an application's main window procedure that a menu modal loop has been exited.
//  WM_DROPFILES - sent when the user drops a file on the window of an application that has registered itself as a recipient of dropped files.
//
LRESULT CALLBACK  MainWndProc (HWND     hWnd,       // window handle                   
                               UINT     message,    // type of message                 
                               WPARAM   wParam,     // additional information          
                               LPARAM   lParam)     // additional information          
{

char            szCad[MAX_IDSM]; 
static  int     iTopTree;


// Variables del tooltip:
UINT            idButton;
LPTOOLTIPTEXT   lpttt; 


RECT        rc;                 // Para guardar las coordenadas de la ventana
RECT        rcSB;
LPRECT      lprc;

int         k = 0;
UINT        unFlag;
UINT        unItem;

INFOCOMPARAR    InfoComparar;

HINSTANCE       hins;

    switch (message) 
    {
     
        case WM_CREATE:

            // Creacion de los controles de la ventana principal:
            // *************************************************

            // Centramos la ventana
            GetWindowRect (hWnd, & rc);
            MoveWindow (hWnd,
                        (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                        (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                        (rc.right - rc.left),
                        (rc.bottom - rc.top ),
                        TRUE);

            // Creacion de menus con dibujo asociado:
            PonerDibujoMenu (hWnd, hInstance);

            // Creacion de los menus owner-draw
            CrearMenuOwnerDrawn (hWnd, hInstance, menus);

            // Creacion  e inicializacion de la barra de estado:
            if ( (hWndStatus = CrearBarraEstado (hWnd, hInstance)) == NULL )
                break;

            // Creacion e inicializacion del Rebar
            if ( (hWndRebar = CrearRebar (hWnd, hInstance)) == NULL )
                break;

            // Creacion del Toolbar que se insertara en la banda del Rebar
            if ( (hWndToolbar = CrearToolbar (hWnd, hInstance)) == NULL )
                break;
            
            // Insertar banda al Rebar
            iTopTree = InsertarToolbarAlRebar (hWnd, hInstance, hWndRebar, hWndToolbar);
            
            // Creacion del TreeView:
            if ( (hWndTree = CrearTreeview (hWnd, hInstance, hWndStatus, iTopTree, gibmpTV)) == NULL )
                break;

            // El directorio inicial de ayuda
            GetCurrentDirectory (MAX_PATH, szPath);
            strcat (szPath, "\\CODIhelp\\index.htm");

            // Soporte drag and drop
            DragAcceptFiles (hWnd, TRUE); 
            g_bDrag = FALSE;
    
            return FALSE;



        case WM_DROPFILES:
            
            //  Mensaje enviado cuando un archivo esta siendo soltado en la ventana
            OnDropFiles (hWnd, hInstance, wParam, & g_InfDrag, & g_bDrag);
            return FALSE;


            
        case WM_MEASUREITEM:

            // Mensaje enviado la primera vez que haya que mostrar el menu owner-drawn
            OnMeasureItem (hWnd, (LPMEASUREITEMSTRUCT) lParam);
            return TRUE;



        case WM_DRAWITEM:

            // Mensaje enviado cada vez que haya que dibujar un menu owner-drawn
            OnDrawItem (hWnd, (LPDRAWITEMSTRUCT) lParam);
            return TRUE;            



        case WM_EXITMENULOOP:
            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Listo");
            return FALSE;           



        case WM_MENUSELECT:

            // Manejo de las notificaciones para la barra de estado
            unItem = (UINT) LOWORD(wParam);
            unFlag = (UINT) HIWORD(wParam);

            switch(unItem)
            {
                case IDM_PARTIR:
                    LoadString (hInstance, IDSM_PARTIR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_REUNIR:
                    LoadString (hInstance, IDSM_REUNIR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_BORRAR:
                    LoadString (hInstance, IDSM_BORRAR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_EXTRAER:
                    LoadString (hInstance, IDSM_EXTRAER, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_COMPARAR:
                    LoadString (hInstance, IDSM_COMPARAR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_SALIR:
                    LoadString (hInstance, IDSM_SALIR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_ENCRIPTAR:
                    LoadString (hInstance, IDSM_ENCRIPTAR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_DESENCRIPTAR:
                    LoadString (hInstance, IDSM_DESENCRIPTAR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_PASSWORD:
                    LoadString (hInstance, IDSM_PASSWORD, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_EXPLORAR:
                    LoadString (hInstance, IDSM_EXPLORAR, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_OPCIONES_EXPLORACION:
                    LoadString (hInstance, IDSM_OPCIONES_EXPLORACION, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_COMPLEMENTOS:
                    LoadString (hInstance, IDSM_COMPLEMENTOS, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_CRIPTOCODI_1_0:
                    LoadString (hInstance, IDSM_CRIPTOCODI_1_0, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_CONTENIDO:
                    LoadString (hInstance, IDSM_CONTENIDO, (LPSTR) &szCad, MAX_IDSM);
                    goto PONER_TEXTO_STATUS;

                case IDM_ACERCADE:
                    LoadString (hInstance, IDSM_ACERCADE, (LPSTR) &szCad, MAX_IDSM);
PONER_TEXTO_STATUS:
                    SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) (LPSTR) szCad );
                    break;

                case 0:
                    if (unFlag & MF_POPUP)
                        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "mejores son dos que uno...");
                    else
                        goto LIMPIAR_STATUS;
                    break;
                
                case 1:
                    if (unFlag & MF_POPUP)
                        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "tienen ojos y no ven...");
                    else
                        goto LIMPIAR_STATUS;
                    break;

                case 2:
                    if (unFlag & MF_POPUP)
                        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "nada hay oculto que no haya de revelarse...");
                    else
                        goto LIMPIAR_STATUS;
                    break;

                case 3:
                    if (unFlag & MF_POPUP)
                        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "si tuvieras fé...");
                    else
                        goto LIMPIAR_STATUS;
                    break;

                case 4:
                    if (unFlag & MF_POPUP)
                        SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "vanidad de vanidades...");
                    else
                        goto LIMPIAR_STATUS;
                    break;

                default:
LIMPIAR_STATUS:
                    SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "");

            }
            return FALSE;



        case WM_COMMAND:

            // Manejo de los commandos del menu y toolbar
            // ******************************************

            switch( LOWORD( wParam ))
            {
                case IDM_PARTIR:
                    CrearWizardPartir (hWnd, hInstance);
                    if (wpi.bFin)
                    {
                        UpdateWindow (hWnd);
                        if (Fragmentar (hWnd, hInstance, hWndStatus, &wpi))
                        {
                            FinalizarPartirTreeView (hWndTree, gibmpTV, &wpi);
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Se ha partido el archivo");
                        }
                        else
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Error fragmentando");
                    }
                    break;

                case IDM_REUNIR:
                    CrearWizardReunir (hWnd, hInstance);
                    if (wri.bFin)
                    {
                        UpdateWindow (hWnd);
                        if (Defragmentar (hWnd, hInstance, hWndStatus, &wri)) 
                        {
                            FinalizarReunirTreeView (hWndTree, gibmpTV, &wri);
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Se ha reunido el archivo");
                        }
                        else
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Error defragmentando");
                    }
                    // Liberar la memoria ocupada por los nombres de archivos encontrados
                    // Si no se encontro archivos (wri.inumArch == 0)
                    for (k = 0; k < wri.iNumArch; k++)
                        free (wri.Archivos[k]);
                    break;

                case IDM_BORRAR:
                    MessageBox (NULL, "Borrar", "", MB_OK);
                    break;

                case IDM_EXTRAER:
                    MessageBox (NULL, "Extraer", "", MB_OK);
                    break;

                case IDM_COMPARAR:
                    if (CompararArchivos (hWnd, hInstance, hWndStatus, &InfoComparar))
                    {
                        CompararTreeView (hWndTree, gibmpTV, &InfoComparar);
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Listo");
                    }
                    break;

                case IDM_SALIR:
                    PostQuitMessage(0);
                    break;

                case IDM_ENCRIPTAR:
                    wci.bEstamosEncriptando = TRUE;
                    CrearWizardCripto (hWnd, hInstance);
                    if (wci.bFin)
                    {
                        UpdateWindow (hWnd);
                        if (Encriptar (hWnd, hInstance, hWndStatus, &wci))
                        {
                            CriptoTreeView (hWndTree, gibmpTV, &wci);
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Se ha encriptado el archivo");
                        }
                        else
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Error encriptando");
                    }
                    break;

                case IDM_DESENCRIPTAR:
                    wci.bEstamosEncriptando = FALSE;
                    CrearWizardCripto (hWnd, hInstance);
                    if (wci.bFin)
                    {
                        UpdateWindow (hWnd);
                        if (Desencriptar (hWnd, hInstance, hWndStatus, &wci)) 
                        {
                            CriptoTreeView (hWndTree, gibmpTV, &wci);
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Se ha desencriptado el archivo");
                        }
                        else
                            SendMessage (hWndStatus, SB_SETTEXT, 0 ,(LPARAM) "Error desencriptando");
                    }
                    break;
    
                case IDM_PASSWORD:
                    CrearWizardPassword (hWnd, hInstance);
                    break;

                case IDM_EXPLORAR:
                    MessageBox (NULL, "Explorar", "", MB_OK);
                    break;

                case IDM_OPCIONES_EXPLORACION:
                    MessageBox (NULL, "Opciones exploracion", "", MB_OK);
                    break;

                case IDM_COMPLEMENTOS:
                    MessageBox (NULL, "Complementos", "", MB_OK);
                    break;

                case IDM_CONTENIDO:
                    hins = ShellExecute (hWnd, "open", szPath, NULL, NULL, 0);
                    if (hins < (HINSTANCE) 33)
                        hins = ShellExecute (hWnd, "open", "http://www.geocities.com/codihelp", NULL, NULL, 0);
                    break;

                case IDM_ACERCADE:
                    DialogBox (hInstance, MAKEINTRESOURCE(IDD_DLG_ABOUT), hWnd, (DLGPROC)About);
                    break;

                default:
                    // (Segun GENERIC.C)  Que otros comandos enviara el sistema? 
                    return (DefWindowProc (hWnd, message, wParam, lParam));

            }
            return FALSE;




        // Soporte para los Tooltip del toolbar:
        case WM_NOTIFY:
            switch (((LPNMHDR) lParam)->code)
            { 
                case TTN_NEEDTEXT: 
                    lpttt = (LPTOOLTIPTEXT) lParam; 
                    lpttt->hinst = hInstance; 

                    // Specify the resource identifier of the descriptive text for the given button. 
                    idButton = lpttt->hdr.idFrom; 
                    switch (idButton)
                    { 
                        case IDM_ENCRIPTAR: 
                            lpttt->lpszText = gs_CODICurrentCrypSystem; 
                            break; 
                        case IDM_DESENCRIPTAR: 
                            lpttt->lpszText = gs_CODICurrentCrypSystem; 
                            break; 
                        case IDM_ACERCADE: 
                            lpttt->lpszText = CODI_VERS;
                            break; 
                    } 
                    break;
            }
            return FALSE;

 
        case WM_SIZING:

            // Este codigo permite que la ventana no sea completamente reducida
            rc = *((LPRECT)lParam);
            if (rc.right - rc.left < 200)
                rc.right = rc.left + 200;
            if (rc.bottom - rc.top < 200)
                rc.bottom = rc.top + 200;
            lprc = (LPRECT)lParam;
            lprc->right = rc.right;
            lprc->bottom = rc.bottom;
            // Debe retornar TRUE y no FALSE
            return TRUE;



        case WM_SIZE:

            // Resize the status bar to fit along the bottom of the client area.
            // No es necesario pasar las dimensiones ni las nuevas cordenadas pues la barra de estado
            // se acomoda sola, sin embargo es necesario indicarle que la ventana se ha movido
            MoveWindow (hWndStatus, 0, 0, 0, 0, TRUE);
            
            // Mueve y ajusta el tamaño del Rebar
            MoveWindow (hWndRebar, 0, 0, 0, 0,TRUE);

            // Mueve y ajusta el tamaño del Tree
            GetClientRect (hWnd, &rc);
            GetClientRect (hWndStatus, &rcSB);
            MoveWindow (hWndTree,                           // Manejador del tree
                        TV_MARGEN_IZQ,                      // x (esquina superior izquierda)
                        iTopTree,                           // y (esquina superior izquierda)
                        rc.right - TV_MARGEN_IZQ,           // ancho
                        rc.bottom - iTopTree - rcSB.bottom, // alto
                        TRUE);                              // volver a dibujar ventana
            return FALSE; 

            

        case WM_DESTROY:
            
            if (hWndStatus) 
                DestroyWindow (hWndStatus);

            if (hWndRebar) 
                DestroyWindow (hWndRebar);

            if (hWndToolbar)
                DestroyWindow (hWndToolbar);

            if (hWndTree)
                DestroyWindow (hWndTree);

            PostQuitMessage (0);
            return FALSE;



        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));

    }
    return FALSE;   // Algo no salio bien
}


//  *******************************
//  FUNCIONES DE MANEJO DE DIALOGOS
//  *******************************

//  Esta funcion procesa los mensajes del dialogo Opciones de arrastre
//  
LRESULT CALLBACK OpcionDragDlg (HWND hDlg,
                                UINT message,
                                UINT wParam,
                                LONG lParam)
{

static int nOpcion;

    switch (message)
    {
        case WM_INITDIALOG:
            CheckDlgButton (hDlg, IDRB_PARTIR, BST_CHECKED);
            nOpcion = DRAG_PARTIR;
            SendDlgItemMessage (hDlg, IDTX_DRAGFILE, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) g_InfDrag.szFileDrag);
            return TRUE;


        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            { 
                switch (LOWORD(wParam)) 
                { 
                    case IDRB_PARTIR:
                        nOpcion = DRAG_PARTIR;
                        break;

                    case IDRB_REUNIR:
                        nOpcion = DRAG_REUNIR;
                        break;

                    case IDRB_ENCRIPTAR:
                        nOpcion = DRAG_ENCRIPTAR;
                        break;

                    case IDRB_DESENCRIPTAR:
                        nOpcion = DRAG_DESENCRIPTAR;
                        break;

                    case IDCANCEL:
                        g_InfDrag.nOpcion = DRAG_NADA;
                        EndDialog (hDlg, TRUE);
                        break;

                    case IDOK:
                        g_InfDrag.nOpcion = nOpcion;
                        EndDialog (hDlg, TRUE);
                        return TRUE;
            
                    default:
                        return FALSE;
                }
                return TRUE;
            }

        default:
            return FALSE;
    }
}



//  Esta funcion procesa los mensajes del cuadro de dialogo Acerca De
//  
LRESULT CALLBACK About (HWND hDlg,
                        UINT message,
                        UINT wParam,
                        LONG lParam)
{

static  HBITMAP         hbmps [4];
static  BLENDFUNCTION   blend;
static  HDC             hDCImagen;
static  HWND            hwndImagen;
static  int             iFusion;
static  BOOL            bIni;
HDC     mDC;
HBITMAP hAntBmp;


    switch (message)
    {
        case WM_INITDIALOG:
            iFusion = 1;
            bIni = TRUE;
            hwndImagen = GetDlgItem (hDlg, IDC_IMAGEN);
            hDCImagen = GetDC (hwndImagen);
            hbmps [0] = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_WIZ_NUMERO));
            hbmps [1] = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_CRIPTO));
            hbmps [2] = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_WIZ_BUSCAR_ARCHIVO));
            hbmps [3] = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_WIZ_FINALIZAR));
            blend.BlendOp = AC_SRC_OVER;
            blend.BlendFlags = 0;
            blend.SourceConstantAlpha = 0;
            blend.AlphaFormat = 0;
            return TRUE;

        case WM_PAINT:
            mDC = CreateCompatibleDC (hDCImagen);
            hAntBmp = (HBITMAP) SelectObject (mDC, hbmps [iFusion]);
            AlphaBlend (hDCImagen, 0, 0, 289, 238, mDC, 0, 0, 289, 238, blend);
            if (!bIni)
                Sleep (200);
            else
                bIni = FALSE;

            SelectObject (mDC, hAntBmp);
            DeleteObject (mDC);
            if (blend.SourceConstantAlpha < 220)
                SendMessage (hDlg, WM_COMMAND, 0, 0);
            else
            {
                iFusion++;
                iFusion = (iFusion % 4); //Porque no sirve: iFusion = (iFusion++) % 4?
                blend.SourceConstantAlpha = 0;
                SendMessage (hDlg, WM_COMMAND, 0, 0);
            }
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDCANCEL:

                case IDBT_SALIR:
SALIR_DLG:
                    // Salir del dialogo al presionar ESC ENT o ESP
                    DeleteObject (hbmps [0]);
                    DeleteObject (hbmps [1]);
                    DeleteObject (hbmps [2]);
                    DeleteObject (hbmps [3]);
                    EndDialog (hDlg, TRUE);
                    return TRUE;
            
                default:
                    InvalidateRect (hwndImagen, NULL, FALSE);
                    if (blend.SourceConstantAlpha < 25)
                        blend.SourceConstantAlpha += 1;
                    else
                        if (blend.SourceConstantAlpha < 100)
                            blend.SourceConstantAlpha += 10;
                        else
                            blend.SourceConstantAlpha += 30;
                    return TRUE;
            }
        
        
        case WM_ACTIVATEAPP:

        case WM_MOVING:
        
        case WM_LBUTTONDOWN:

        case WM_RBUTTONDOWN:
            // Salir del dialogo al hacer click con el mouse o al tratar de moverlo
            // o al cambiar de aplicacion.
            goto SALIR_DLG;

        default:
            return FALSE;
    }
}


//  Esta funcion procesa los mensajes de la primera hoja del Wizard Partir
//
LRESULT CALLBACK BuscarArchivoPartirDialogProc (HWND hDlg,
                                                UINT message,
                                                UINT wParam,
                                                LONG lParam)
{

HWND            hWndBtn;
OPENFILENAME    ofn;
BOOL            bTemp;
char            szFile[MAX_PATH + 1];
HANDLE          hArchivoFuente; 
DWORD           dwTamanyoFuenteHigh;
DWORD           dw;
HWND            hWndParent;
BROWSEINFO      bi;
LPITEMIDLIST    pidlBrowse;                 // PIDL selected by user 
RECT            rc;
int             k;

    switch (message)
    {
        case WM_INITDIALOG:
            // Centramos el wizard
            hWndParent = GetParent (hDlg);
            GetWindowRect (hWndParent, & rc);
            MoveWindow (hWndParent,
                        (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                        (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                        (rc.right - rc.left),
                        (rc.bottom - rc.top ),
                        TRUE);
            // Inicializamos valores
            wpi.bFin = FALSE;
            if (g_bDrag)
            {
                hWndBtn = GetDlgItem (hDlg, IDLB_PARTIR1);
                EnableWindow (hWndBtn, FALSE);
                hWndBtn = GetDlgItem (hDlg, IDBT_ELEGIR_ARCHIVO);
                EnableWindow (hWndBtn, FALSE);
                strcpy (wpi.szNombreArchivoFuente, g_InfDrag.szFileDrag);
                hArchivoFuente = CreateFile (wpi.szNombreArchivoFuente,
                                             GENERIC_READ,
                                             0,             //No se permite compartir
                                             NULL,          //No seguridad
                                             OPEN_EXISTING, //Falla si no existe
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);                           
                if (hArchivoFuente == INVALID_HANDLE_VALUE)
                {   
                    MessageBox (NULL,"El archivo fuente no pudo ser abierto. Verifica que existe y NO esta siendo usado", NULL, MB_OK | MB_ICONERROR);
                    return TRUE;
                }
                wpi.dwTamanyoFuenteLow = GetFileSize (hArchivoFuente, & dwTamanyoFuenteHigh);
                if (wpi.dwTamanyoFuenteLow != 0xFFFFFFFF)
                {
                    if (dwTamanyoFuenteHigh != 0)
                    {
                        MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Error",MB_OK | MB_ICONERROR);
                        CloseHandle (hArchivoFuente);
                        return TRUE;
                    }
                }
                else
                {
                    if ( (dw = GetLastError()) != NO_ERROR)
                    {
                        MessageBox (NULL,"No se pudo calcular el tamaño de la fuente.", "Error",MB_OK | MB_ICONERROR);
                        CloseHandle (hArchivoFuente);
                        return TRUE;
                    }
                }
                CloseHandle (hArchivoFuente);

                // Copiamos solo el nombre sin la ruta
                // Buscamos el ultimo caracter '\\' 
                for( k = strlen (g_InfDrag.szFileDrag) - 1; k > 0; k--)
                {
                    if (g_InfDrag.szFileDrag [k] == '\\')
                        break;
                }
                strcpy (wpi.szNombreSoloFuente, & g_InfDrag.szFileDrag [k + 1]);
                
                // Creamos nombre generico
                strcpy (wpi.szNombreArchivoGenerico, wpi.szNombreSoloFuente);
                // Buscamos el punto de la extension
                for( k = strlen (wpi.szNombreArchivoGenerico); k > 0; k--)
                {
                    if (wpi.szNombreArchivoGenerico [k] == '.')
                        break;
                }
                if (k)
                {
                    // Si tiene extension: [k + 1] porque la funcion toma el siguiente caracter al punto
                    CrearNombreGenerico (wpi.szNombreArchivoGenerico, (WORD) (k + 1) , 0);
                }
                else
                    CrearNombreGenerico (wpi.szNombreArchivoGenerico, 0, 0);

                wpi.bArchivoFuenteElegido = TRUE;
            }
            else
            {
                hWndBtn = GetDlgItem (hDlg, IDLB_PARTIR2);
                EnableWindow (hWndBtn, FALSE);
                hWndBtn = GetDlgItem (hDlg, IDBT_ELEGIR_CARPETA);
                EnableWindow (hWndBtn, FALSE);
                wpi.bArchivoFuenteElegido = FALSE;
                wpi.dwTamanyoFuenteLow = 0;
            }
            dwTamanyoFuenteHigh = 0;
            wpi.bArchivoDestinoElegido = FALSE;
            return TRUE;

    
        case WM_COMMAND:
            switch( LOWORD(wParam))
            {
                case IDBT_ELEGIR_ARCHIVO:
                    // Ningun archivo inicial que mostrar en el Dialogo Abrir
                    ZeroMemory (szFile, sizeof(szFile));
                    // Necesitamos este Handle si queremos un cuadro de dialogo MODAL
                    hWndParent = GetParent (hDlg);
                    bTemp = MostrarDlgComunAbrir (hWndParent,
                                                  & ofn,
                                                  szFile,
                                                  sizeof(szFile),
                                                  "Selecciona el archivo a partir:",
                                                  "Todos los archivos\0*.*\0",
                                                  NULL,
                                                  OFN_FILEMUSTEXIST |
                                                  OFN_PATHMUSTEXIST |
                                                  OFN_HIDEREADONLY);
                    if (bTemp)
                    {
                        strcpy (wpi.szNombreArchivoFuente, ofn.lpstrFile);
                        hArchivoFuente = CreateFile (ofn.lpstrFile,
                                                     GENERIC_READ,
                                                     0,             //No se permite compartir
                                                     NULL,          //No seguridad
                                                     OPEN_EXISTING, //Falla si no existe
                                                     FILE_ATTRIBUTE_NORMAL,
                                                     NULL);                           
                        if (hArchivoFuente == INVALID_HANDLE_VALUE)
                        {   
                            MessageBox (NULL,"El archivo fuente no pudo ser abierto. Verifica que existe y NO esta siendo usado", NULL, MB_OK | MB_ICONERROR);
                            return TRUE;
                        }
                        wpi.dwTamanyoFuenteLow = GetFileSize (hArchivoFuente, & dwTamanyoFuenteHigh);
                        if (wpi.dwTamanyoFuenteLow != 0xFFFFFFFF)
                        {
                            if (dwTamanyoFuenteHigh != 0)
                            {
                                MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Error",MB_OK | MB_ICONERROR);
                                CloseHandle (hArchivoFuente);
                                return TRUE;
                            }
                        }
                        else
                        {
                            if ( (dw = GetLastError()) != NO_ERROR)
                            {
                                MessageBox (NULL,"No se pudo calcular el tamaño de la fuente.", "Error",MB_OK | MB_ICONERROR);
                                CloseHandle (hArchivoFuente);
                                return TRUE;
                            }
                        }
                        CloseHandle (hArchivoFuente);
                        // <!> Copiamos solo el nombre (suponemos el buffer alojado ininterrumpidamente en memoria)
                        strcpy (wpi.szNombreSoloFuente, ofn.lpstrFile + ofn.nFileOffset);
                        strcpy (wpi.szNombreArchivoGenerico, wpi.szNombreSoloFuente);
                        CrearNombreGenerico (wpi.szNombreArchivoGenerico, ofn.nFileExtension, ofn.nFileOffset);
                        wpi.bArchivoFuenteElegido = TRUE;
                        if (wpi.bArchivoDestinoElegido != TRUE )
                        {
                            hWndBtn = GetDlgItem (hDlg, IDBT_ELEGIR_CARPETA);
                            EnableWindow (hWndBtn, TRUE);
                            hWndBtn = GetDlgItem (hDlg, IDLB_PARTIR2);
                            EnableWindow (hWndBtn, TRUE);
                        }
                    }
                    return TRUE;

                case IDBT_ELEGIR_CARPETA:
                    ZeroMemory (szFile, sizeof(szFile));
                    hWndParent = GetParent (hDlg);
                    // Get the shell's allocator. 
                    if (!SUCCEEDED(SHGetMalloc(&g_pMalloc)))
                    {
                        MessageBox (NULL, "Problemas con el Shell de Windows", NULL, MB_OK |MB_ICONERROR);
                        return TRUE;
                    }
                    // Llenamos la estructura para pedir informacion:
                    bi.hwndOwner = hWndParent;
                    bi.pidlRoot = NULL; 
                    bi.pszDisplayName = szFile; 
                    bi.lpszTitle = "Selecciona la carpeta donde se guardaran los archivos fragmentados"; 
                    bi.ulFlags = BIF_RETURNONLYFSDIRS;      // Solo folderes fisicos del sistema (no "MiPC" por ejemplo)
                    bi.lpfn = NULL; 
                    bi.lParam = 0; 
                    bi.iImage = 0;
                    pidlBrowse = SHBrowseForFolder(&bi);
                    if (pidlBrowse)         // Devuelve NULL cuando se elige CANCELAR
                    {
                        if (SHGetPathFromIDList (pidlBrowse, szFile))
                        {
                            dw = lstrlen (wpi.szNombreArchivoGenerico) + lstrlen (szFile); 
                            if (dw > MAX_PATH - 5)
                                MessageBox (NULL, "La ruta es muy larga. elige otra mas corta", NULL, MB_OK | MB_ICONEXCLAMATION);
                            else
                            {
                                strcpy (wpi.szCarpetaDestino, szFile);
                                wpi.bArchivoDestinoElegido = TRUE;
                            }

                        }
                        else
                            MessageBox (NULL, "No se pudo reconocer dicha carpeta", NULL, MB_OK | MB_ICONERROR);
                        // Free the PIDL returned by SHBrowseForFolder.
                        g_pMalloc->lpVtbl->Free(g_pMalloc, pidlBrowse);
                    }
                    return TRUE;

                default:
                    return FALSE;
            }


        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT);
                    break;

                case PSN_WIZNEXT:
                    if (!wpi.bArchivoFuenteElegido)
                    {
                        MessageBox(NULL,"Debes elegir un archivo primero",NULL,MB_OK | MB_ICONEXCLAMATION);
                        SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                    }
                    else
                        if (!wpi.bArchivoDestinoElegido)
                        {
                            MessageBox(NULL,"Debes elegir una carpeta",NULL,MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                        }
                        else
                            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}


//  Esta funcion procesa los mensajes de la segunda hoja del Wizard Partir
//
LRESULT CALLBACK NumeroArchivosPartirDialogProc (HWND hDlg,
                                                 UINT message,
                                                 UINT wParam,
                                                 LONG lParam)
{

char    szTamanyoPartido[11];
char    str[120];
char    *s;
HWND    hWndC;
DWORD   dw;


    switch (message)
    {
        case WM_INITDIALOG:
            CheckDlgButton (hDlg, IDRB_POR_NUMERO, BST_CHECKED);
            SetDlgItemText (hDlg, IDTXT_NUMERO, "2");
            SetDlgItemText (hDlg, IDTXT_TAMANYO, "1300000");
            wpi.iNumArch = 2;
            wpi.bNumeroElegido = TRUE;
            SendDlgItemMessage (hDlg, IDTXT_NUMERO, EM_SETLIMITTEXT, (WPARAM) 2, 0);
            SendDlgItemMessage (hDlg, IDTXT_TAMANYO, EM_SETLIMITTEXT, (WPARAM) 9, 0);
            ZeroMemory (szTamanyoPartido, sizeof(szTamanyoPartido));
            return TRUE;


        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED) 
            { 
                switch (LOWORD(wParam)) 
                { 
                    case IDRB_POR_NUMERO:
                        wpi.bNumeroElegido = TRUE;
                        hWndC = GetDlgItem (hDlg, IDFRA_NUMERO);
                        EnableWindow (hWndC, TRUE);
                        hWndC = GetDlgItem (hDlg, IDLB_NUMERO);
                        EnableWindow (hWndC, TRUE);
                        hWndC = GetDlgItem (hDlg, IDTXT_NUMERO);
                        EnableWindow (hWndC, TRUE);
                        hWndC = GetDlgItem (hDlg, IDFRA_TAMANYO);
                        EnableWindow (hWndC, FALSE);
                        hWndC = GetDlgItem (hDlg, IDLB_TAMANYO);
                        EnableWindow (hWndC, FALSE);
                        hWndC = GetDlgItem (hDlg, IDTXT_TAMANYO);
                        EnableWindow (hWndC, FALSE);
                        break; 

                    case IDRB_POR_TAMANYO:
                        wpi.bNumeroElegido = FALSE;
                        hWndC = GetDlgItem (hDlg, IDFRA_NUMERO);
                        EnableWindow (hWndC, FALSE);
                        hWndC = GetDlgItem (hDlg, IDLB_NUMERO);
                        EnableWindow (hWndC, FALSE);
                        hWndC = GetDlgItem (hDlg, IDTXT_NUMERO);
                        EnableWindow (hWndC, FALSE);
                        hWndC = GetDlgItem (hDlg, IDFRA_TAMANYO);
                        EnableWindow (hWndC, TRUE);
                        hWndC = GetDlgItem (hDlg, IDLB_TAMANYO);
                        EnableWindow (hWndC, TRUE);
                        hWndC = GetDlgItem (hDlg, IDTXT_TAMANYO);
                        EnableWindow (hWndC, TRUE);
                        break; 

                    default:
                        return FALSE;
                }
                return TRUE;
            }
            return FALSE;

                            
        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT | PSWIZB_BACK);
                    break;

                case PSN_WIZNEXT:
                    if (wpi.bNumeroElegido)
                    {
                        wpi.iNumArch = (int) GetDlgItemInt (hDlg,   IDTXT_NUMERO, NULL, FALSE);
                        if ( (wpi.iNumArch < 2) || (wpi.iNumArch >99) )
                        {
                            MessageBox(NULL,"Debes elegir un numero entre 2 y 99",NULL,MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                        }
                        else
                        {
                            if (wpi.dwTamanyoFuenteLow % wpi.iNumArch)
                                wpi.dwTamanyoMaxPartido = (wpi.dwTamanyoFuenteLow / wpi.iNumArch) + 1;
                            else
                                wpi.dwTamanyoMaxPartido = wpi.dwTamanyoFuenteLow / wpi.iNumArch;
                            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                        }
                    }
                    else
                    {
                        GetDlgItemText (hDlg, IDTXT_TAMANYO, szTamanyoPartido , 10);
                        wpi.dwTamanyoMaxPartido = (DWORD) strtoul( szTamanyoPartido, &s, 10);
                        dw = sizeof(INFOFILEFRAG);
                        if (wpi.dwTamanyoMaxPartido <= dw)
                        {
                            wsprintf (str, "Ingresa un valor para el tamaño mayor a %lu (Bytes)", dw);
                            MessageBox(NULL, str, NULL, MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                        }
                        else
                        {
                            // Solo tenemos en cuenta el numero de datos (sin cabecera)
                            wpi.dwTamanyoMaxPartido -= dw;
                            if (wpi.dwTamanyoMaxPartido >= wpi.dwTamanyoFuenteLow)
                            {
                                wsprintf (str, "El tamaño ingresado debe ser menor al del archivo fuente que actualmente tiene: %lu (Bytes)" , wpi.dwTamanyoFuenteLow);
                                MessageBox(NULL, str, NULL, MB_OK | MB_ICONEXCLAMATION);
                                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                            }
                            else
                            {   
                                if (wpi.dwTamanyoFuenteLow % wpi.dwTamanyoMaxPartido)
                                    wpi.iNumArch = (wpi.dwTamanyoFuenteLow / wpi.dwTamanyoMaxPartido) + 1;
                                else
                                    wpi.iNumArch = (wpi.dwTamanyoFuenteLow / wpi.dwTamanyoMaxPartido);
                                if (wpi.iNumArch > 99)
                                {
                                    MessageBox (NULL, "El tamaño especificado es muy pequeño. \nCodi fragmenta archivos hasta en 99 partes. Ingresa un valor mas grande", NULL, MB_OK | MB_ICONEXCLAMATION);
                                    SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                                }
                                else
                                    SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                            }
                        }
                    }
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}


//  Esta funcion procesa los mensajes de la tercera hoja del Wizard Partir
//
LRESULT CALLBACK FinalizarPartirDialogProc (HWND hDlg,
                                            UINT message,
                                            UINT wParam,
                                            LONG lParam)
{

char    linea[MAX_PATH + 5];    // +4 para los espacios
HWND    hLista;
int     k;
DWORD   t;

    switch (message)
    {
        case WM_INITDIALOG:     
            // Especificamos el ancho visible en pxls de la Lista (por HScroll) 
            SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_SETHORIZONTALEXTENT, (WPARAM) 1600, 0);
            return TRUE;

        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code)
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg),  PSWIZB_BACK | PSWIZB_FINISH);
                    // limpiar lista:
                    SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_RESETCONTENT, 0, 0);

                    // Mostrar la informacion
                    hLista = GetDlgItem(hDlg, IDL_RESUMEN);
                    strcpy (linea, "[ ARCHIVO FUENTE ]\0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    \0");
                    strcat (linea, wpi.szNombreArchivoFuente);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    wsprintf (linea, "    DE TAMAÑO: %lu (Bytes)", wpi.dwTamanyoFuenteLow);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " \0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    wsprintf (linea, "[ SE PARTIRA EN %lu ARCHIVOS ]", wpi.iNumArch);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    t = wpi.dwTamanyoMaxPartido + sizeof(INFOFILEFRAG);
                    wsprintf (linea, "    TAMAÑO MAXIMO: %lu (Bytes)", t);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " \0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ CARPETA DE DESTINO ]\0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    \0");
                    strcat (linea, wpi.szCarpetaDestino);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " \0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ ARCHIVOS GENERADOS ]\0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    t = strlen (wpi.szNombreArchivoGenerico);
                    for( k = 0; k < wpi.iNumArch; k++)
                    {
                        strcpy (linea, "    \0");
                        wpi.szNombreArchivoGenerico[t-6] = (char) (48 + (k / 10));
                        wpi.szNombreArchivoGenerico[t-5] = (char) (48 + (k % 10));
                        strcat (linea, wpi.szNombreArchivoGenerico);
                        SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    }
                    break;

                case PSN_WIZFINISH:
                    wpi.bFin = TRUE;
                    break;

                default:
                    return FALSE;

            }
            return TRUE;
    }
    return FALSE;
}


//  Esta funcion procesa los mensajes de la primera hoja del Wizard Reunir
//
LRESULT CALLBACK BuscarCarpetaReunirDialogProc (HWND hDlg,
                                                UINT message,
                                                UINT wParam,
                                                LONG lParam)
{

char            szFile[MAX_PATH + 1];
char            ch;
HWND            hWndParent;
HWND            hWndBtn;
BROWSEINFO      bi;
LPITEMIDLIST    pidlBrowse;                 // PIDL selected by user 
BOOL            bTemp;
OPENFILENAME    ofn;
UINT            ui;
RECT            rc;

    switch (message)
    {
        case WM_INITDIALOG:
            // Centramos el wizard
            hWndParent = GetParent (hDlg);
            GetWindowRect (hWndParent, & rc);
            MoveWindow (hWndParent,
                        (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                        (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                        (rc.right - rc.left),
                        (rc.bottom - rc.top ),
                        TRUE);
            // Inicializamos valores
            wri.bFin = FALSE;
            wri.iNumArch = 0;
            if (g_bDrag)
            {
                hWndBtn = GetDlgItem (hDlg, IDLB_1);
                EnableWindow (hWndBtn, FALSE);
                hWndBtn = GetDlgItem (hDlg, IDLB_2);
                EnableWindow (hWndBtn, FALSE);
                hWndBtn = GetDlgItem (hDlg, IDBT_ELEGIR_CARPETA);
                EnableWindow (hWndBtn, FALSE);
                SendDlgItemMessage (hDlg, IDLB_REUNIR, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) "La carpeta con los archivos a reunir ya fue elegida!");
                strcpy (wri.szCarpetaFuente, g_InfDrag.szFileDrag);
                wri.bCarpetaFuenteElegida = TRUE;
            }
            else
                wri.bCarpetaFuenteElegida = FALSE;
            wri.bArchivoDestinoElegido = FALSE;
            wri.bAutomatico = TRUE;
            hWndBtn = GetDlgItem (hDlg, IDBT_REUNIR_DESTINO);
            EnableWindow (hWndBtn, FALSE);
            strcpy (wri.szNombreArchivoDestino, "<Automatico>");
            CheckDlgButton (hDlg, IDCHK_AUTOMATICO, BST_CHECKED);
            return TRUE;

    
        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {    
                switch( LOWORD(wParam))
                {
                    case IDBT_ELEGIR_CARPETA:
                        ZeroMemory (szFile, sizeof(szFile));
                        hWndParent = GetParent (hDlg);
                        // Get the shell's allocator. 
                        if (!SUCCEEDED(SHGetMalloc(&g_pMalloc)))
                        {
                            MessageBox (NULL, "Problemas con el Shell de Windows", NULL, MB_OK |MB_ICONERROR);
                            return TRUE;
                        }
                        // Llenamos la estructura para pedir informacion:
                        bi.hwndOwner = hWndParent;
                        bi.pidlRoot = NULL;
                        bi.pszDisplayName = szFile;
                        bi.lpszTitle = "Selecciona la carpeta en donde copiaste los archivos fragmentados"; 
                        bi.ulFlags = BIF_RETURNONLYFSDIRS;      // Solo folderes fisicos del sistema (no "MiPC" por ejemplo)
                        bi.lpfn = NULL;
                        bi.lParam = 0; 
                        bi.iImage = 0;
                        pidlBrowse = SHBrowseForFolder(&bi);
                        if (pidlBrowse)         // Devuelve NULL cuando se elige CANCELAR
                        {
                            if (SHGetPathFromIDList (pidlBrowse, szFile))
                            {
                                ch = szFile[strlen(szFile) - 1];
                                if (ch == '\\')
                                    MessageBox (NULL, "No se acepta el directorio raiz", NULL, MB_OK | MB_ICONERROR);
                                else
                                {
                                    strcpy (wri.szCarpetaFuente, szFile);
                                    wri.bCarpetaFuenteElegida = TRUE;
                                }
                            }
                            else
                                MessageBox (NULL, "El sistema no pudo reconocer dicha carpeta", NULL, MB_OK | MB_ICONERROR);
                            // Free the PIDL returned by SHBrowseForFolder.
                            g_pMalloc->lpVtbl->Free(g_pMalloc, pidlBrowse);
                        }
                        return TRUE;

                    case IDBT_REUNIR_DESTINO:
                        ZeroMemory (szFile, sizeof(szFile));
                        hWndParent = GetParent (hDlg);
                        bTemp = MostrarDlgComunGuardar (hWndParent,
                                                        & ofn,
                                                        szFile,
                                                        sizeof(szFile),
                                                        "Ingresa el nombre del archivo a formar:",
                                                        "Todos los Archivos\0*.*\0",
                                                        NULL,
                                                        NULL,
                                                        OFN_HIDEREADONLY |
                                                        OFN_OVERWRITEPROMPT);
                        if (bTemp)
                        {
                            strcpy (wri.szNombreArchivoDestino, ofn.lpstrFile);
                            wri.bArchivoDestinoElegido = TRUE;
                        }
                        return TRUE;
                    
                    case IDCHK_AUTOMATICO:
                        ui = IsDlgButtonChecked (hDlg, IDCHK_AUTOMATICO);
                        if (ui == BST_CHECKED)
                        {
                            hWndBtn = GetDlgItem (hDlg, IDBT_REUNIR_DESTINO);
                            EnableWindow (hWndBtn, FALSE);
                            wri.bAutomatico = TRUE;
                        }
                        else
                        {
                            hWndBtn = GetDlgItem (hDlg, IDBT_REUNIR_DESTINO);
                            EnableWindow (hWndBtn, TRUE);
                            wri.bAutomatico = FALSE;
                        }
                        return TRUE;

                    default:
                        return FALSE;
                }
            }
            return FALSE;


        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT);
                    break;

                case PSN_WIZNEXT:
                    if (!wri.bCarpetaFuenteElegida)
                    {
                        MessageBox(NULL,"Debes especificar el folder que creaste",NULL,MB_OK | MB_ICONEXCLAMATION);
                        SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                    }
                    else
                    {
                        if (!wri.bAutomatico && !wri.bArchivoDestinoElegido)
                        {
                            MessageBox(NULL,"Selecciona automatico o elige un destino",NULL,MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                        }
                        else
                            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                    }
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}



//  Esta funcion procesa los mensajes de la segunda hoja del Wizard Reunir
//
LRESULT CALLBACK FinalizarReunirDialogProc (HWND hDlg,
                                            UINT message,
                                            UINT wParam,
                                            LONG lParam)
{

WIN32_FIND_DATA     FileData;
HANDLE              hSearch;
char                linea[MAX_PATH + 5];    // +4 para los espacios
char                *pCad;
HWND                hLista;


    switch (message)
    {
        case WM_INITDIALOG:     
            // Especificamos el ancho visible en pxls de la Lista (por HScroll) 
            SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_SETHORIZONTALEXTENT, (WPARAM) 1600, 0);
            return TRUE;

        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code)
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg),  PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
                    // limpiar lista:
                    SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_RESETCONTENT, 0, 0);

                    // Mostrar la informacion
                    hLista = GetDlgItem(hDlg, IDL_RESUMEN);
                    
                    strcpy (linea, "[ CARPETA FUENTE ELEGIDA ]\0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    \0");
                    strcat (linea, wri.szCarpetaFuente);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " \0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ DESTINO ELEGIDO ]\0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    \0");
                    if (wri.bAutomatico)
                        strcat (linea, "<Automatico>");
                    else
                        strcat (linea, wri.szNombreArchivoDestino);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " \0");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    
                    wsprintf (linea, "[ ARCHIVOS ENCONTRADOS ]", wpi.iNumArch);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    // Listamos los archivos encontrados en la carpeta
                    // y almacenamos los nombres de los archivos encontrados
                    strcpy (linea, wri.szCarpetaFuente);
                    strcat (linea, "\\*.*");
                    hSearch = FindFirstFile(linea, &FileData);
                    if (hSearch == INVALID_HANDLE_VALUE)
                    { 
                        MessageBox (NULL, "Imposible acceder al directorio",NULL,MB_OK | MB_ICONERROR);
                        break;      // out case
                    }
                    else
                    {
                        wri.iNumArch = 0;
                        while (TRUE) 
                        { 
                            if (wri.iNumArch >= 99)
                            {
                                MessageBox (NULL, "Error: La carpeta especificada tiene mas de 99 archivos. \
                                            Elimina aquellos archivos que no formen parte del original.",NULL,MB_OK | MB_ICONERROR);
                                break;      // out while
                            }
                            if (FileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
                            {
                                pCad = (char*) malloc(MAX_PATH + 1);
                                if (pCad)
                                {
                                    wri.Archivos[wri.iNumArch] = pCad;
                                    strcpy (pCad, FileData.cFileName);
                                    ++ wri.iNumArch;
                                    wsprintf (linea, "    [%i] %s", wri.iNumArch, pCad);
                                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                                }
                                else
                                {
                                    MessageBox (NULL, "Sin Memoria",NULL,MB_OK | MB_ICONERROR);
                                    break;
                                }
                            }
                            if (!FindNextFile(hSearch, &FileData)) 
                            {
                                if (GetLastError() == ERROR_NO_MORE_FILES)
                                {
                                    if (wri.iNumArch == 0)
                                        MessageBox (NULL, "No se encontraron archivos.",NULL,MB_OK | MB_ICONERROR);
                                    else
                                        PropSheet_SetWizButtons(GetParent(hDlg),  PSWIZB_BACK | PSWIZB_FINISH);
                                    break;
                                }
                                else 
                                { 
                                    MessageBox (NULL, "No se puedo acceder a un archivo de la carpeta especificada.",NULL,MB_OK | MB_ICONERROR);
                                    break;
                                }
                            }
                        }
                        FindClose (hSearch);
                    }
                    break;

                case PSN_WIZFINISH:
                    wri.bFin = TRUE;
                    break;

                default:
                    return FALSE;

            }
            return TRUE;
    }
    return FALSE;
}



//  Esta funcion procesa los mensajes de la primera hoja del Wizard Cripto
//
LRESULT CALLBACK CriptoDialogProc (HWND hDlg,
                                   UINT message,
                                   UINT wParam,
                                   LONG lParam)
{

OPENFILENAME    ofn;
BOOL            bTemp;
char            szFile[MAX_PATH + 1];
char            cad[MAX_PATH];
char            szCodigo[LNG_CODIGO + 1];
char            szConfirmacion[LNG_CODIGO + 1];
HWND            hWndParent;
HWND            hWndBtn;
UINT            ui;
RECT            rc;
int             k;

    switch (message)
    {
        case WM_INITDIALOG:
            // Centramos el wizard
            hWndParent = GetParent (hDlg);
            GetWindowRect (hWndParent, & rc);
            MoveWindow (hWndParent,
                        (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                        (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                        (rc.right - rc.left),
                        (rc.bottom - rc.top ),
                        TRUE);
            // Inicializamos valores
            wci.bFin = FALSE;
            if (g_bDrag)
            {
                strcpy (wci.szNombreArchivoFuente, g_InfDrag.szFileDrag);
                // Sacamos la ruta (con el ultimo '\\'):
                strcpy (wci.szCarpetaFuente, g_InfDrag.szFileDrag);
                // Buscamos el ultimo caracter '\\'
                for( k = strlen (wci.szCarpetaFuente) - 1; k > 0; k--)
                {
                    if (wci.szCarpetaFuente [k] == '\\')
                    {
                        // Guardamos el nombre del archivo solo:
                        strcpy (wci.szNombreSoloFuente, & wci.szCarpetaFuente [k + 1]);
                        // Truncamos la ruta
                        wci.szCarpetaFuente [k + 1] = 0;
                        break;
                    }
                }
                if (wci.bEstamosEncriptando)
                {
                    // Crear nombre de destino automatico
                    strcpy (wci.szNombreArchivoDestino, g_InfDrag.szFileDrag);
                    if (strlen (wci.szNombreArchivoDestino) > MAX_PATH - 10)
                        wci.bNombreAutomaticoCreado = FALSE;
                    else
                    {
                        // Buscamos el punto de la extension
                        for( k = strlen (wci.szNombreArchivoDestino); k > 0; k--)
                        {
                            if (wci.szNombreArchivoDestino [k] == '.')
                                break;
                        }
                        if (k)
                            wci.szNombreArchivoDestino [k] = '_';
                        strcat (wci.szNombreArchivoDestino, ".cx$");
                        wci.bNombreAutomaticoCreado = TRUE;
                    }
                }
                wci.bArchivoFuenteElegido = TRUE;
                hWndBtn = GetDlgItem (hDlg, IDBT_FUENTE);
                EnableWindow (hWndBtn, FALSE);
            }
            else
                wci.bArchivoFuenteElegido = FALSE;

            wci.bArchivoDestinoElegido = FALSE;
            wci.bAutomatico = TRUE;
            wci.bMascara = TRUE;
            hWndBtn = GetDlgItem (hDlg, IDBT_DESTINO);
            EnableWindow (hWndBtn, FALSE);
            CheckDlgButton (hDlg, IDCHK_AUTOMATICO, BST_CHECKED);
            CheckDlgButton (hDlg, IDCHK_MASCARA, BST_CHECKED);
            SendDlgItemMessage (hDlg, IDTXT_CODIGO, EM_SETPASSWORDCHAR, (WPARAM) (UINT) '*', 0);
            SendDlgItemMessage (hDlg, IDTXT_CODIGO, EM_SETLIMITTEXT, (WPARAM) 9, 0);
            SendDlgItemMessage (hDlg, IDTXT_CONFIRMACION, EM_SETLIMITTEXT, (WPARAM) 9, 0);
            if (wci.bEstamosEncriptando)
            {
                SendDlgItemMessage (hDlg, IDFRA_CODIGO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Ingresa el código con que quieres Encriptarlo :");
                SendDlgItemMessage (hDlg, IDBT_FUENTE, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "&Elegir Archivo a Encriptar");
                if (g_bDrag)
                    SendDlgItemMessage (hDlg, IDFRA_CRIPTO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "El archivo a Encriptar ya fue elegido!");
                else
                    SendDlgItemMessage (hDlg, IDFRA_CRIPTO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "Elige el archivo a Encriptar:");
            }
            else
            {
                SendDlgItemMessage (hDlg, IDBT_FUENTE, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) "&Elegir Archivo a Desencriptar");
                SendDlgItemMessage (hDlg, IDFRA_CODIGO, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) "Ingresa el código con el que lo Encriptaste :");
                if (g_bDrag)
                    SendDlgItemMessage (hDlg, IDFRA_CRIPTO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "El archivo a Desencriptar ya fue elegido!");
                else
                    SendDlgItemMessage (hDlg, IDFRA_CRIPTO, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) "Elige el archivo a Desencriptar:");
            }
            return TRUE;

    
        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {    
                switch( LOWORD(wParam))
                {
                    case IDBT_FUENTE:
                        // Ningun archivo inicial que mostrar en el Dialogo Abrir
                        ZeroMemory (szFile, sizeof(szFile));
                        // Necesitamos este Handle si queremos un cuadro de dialogo MODAL
                        hWndParent = GetParent (hDlg);
                        if (wci.bEstamosEncriptando)
                            strcpy (cad, "Selecciona el archivo a encriptar:");
                        else
                            strcpy (cad, "Selecciona el archivo a desencriptar:");
                        bTemp = MostrarDlgComunAbrir (hWndParent,
                                                      & ofn,
                                                      szFile,
                                                      sizeof(szFile),
                                                      cad,
                                                      "Todos los archivos\0*.*\0",
                                                      NULL,
                                                      OFN_FILEMUSTEXIST |
                                                      OFN_PATHMUSTEXIST |
                                                      OFN_HIDEREADONLY);
                        if (bTemp)
                        {
                            strcpy (wci.szNombreArchivoFuente, ofn.lpstrFile);
                            strcpy (wci.szNombreSoloFuente, ofn.lpstrFile + ofn.nFileOffset);
                            strcpy (wci.szCarpetaFuente, ofn.lpstrFile);
                            // Solo conservamos la ruta (con el ultimo '\\')
                            wci.szCarpetaFuente [ofn.nFileOffset] = '\0';
                            if (wci.bEstamosEncriptando)
                            {
                                // Crear nombre de destino automatico
                                strcpy (wci.szNombreArchivoDestino, ofn.lpstrFile);
                                if (lstrlen (wci.szNombreArchivoDestino) > MAX_PATH - 10)
                                    wci.bNombreAutomaticoCreado = FALSE;
                                else
                                {
                                    wci.szNombreArchivoDestino [ofn.nFileExtension - 1] = '_';
                                    strcat (wci.szNombreArchivoDestino, ".cx$");
                                    wci.bNombreAutomaticoCreado = TRUE;
                                }
                            }
                            wci.bArchivoFuenteElegido = TRUE;
                        }
                        return TRUE;

                    case IDBT_DESTINO:
                        ZeroMemory (szFile, sizeof(szFile));
                        hWndParent = GetParent (hDlg);
                        bTemp = MostrarDlgComunGuardar (hWndParent,
                                                        & ofn,
                                                        szFile,
                                                        sizeof(szFile),
                                                        "Ingresa el nombre del archivo destino:",
                                                        "Todos los Archivos\0*.*\0",
                                                        NULL,
                                                        NULL,
                                                        OFN_HIDEREADONLY |
                                                        OFN_OVERWRITEPROMPT);
                        if (bTemp)
                        {
                            strcpy (wci.szNombreArchivoDestino, ofn.lpstrFile);
                            wci.bArchivoDestinoElegido = TRUE;
                        }
                        return TRUE;

                    case IDCHK_AUTOMATICO:
                        ui = IsDlgButtonChecked (hDlg, IDCHK_AUTOMATICO);
                        if (ui == BST_CHECKED)
                        {
                            hWndBtn = GetDlgItem (hDlg, IDBT_DESTINO);
                            EnableWindow (hWndBtn, FALSE);
                            wci.bAutomatico = TRUE;
                        }
                        else
                        {
                            hWndBtn = GetDlgItem (hDlg, IDBT_DESTINO);
                            EnableWindow (hWndBtn, TRUE);
                            wci.bAutomatico = FALSE;
                        }
                        return TRUE;

                    case IDCHK_MASCARA:
                        ui = IsDlgButtonChecked (hDlg, IDCHK_MASCARA);
                        if (ui == BST_CHECKED)
                        {
                            hWndBtn = GetDlgItem (hDlg, IDTXT_CONFIRMACION);
                            EnableWindow (hWndBtn, TRUE);
                            SendDlgItemMessage (hDlg, IDTXT_CODIGO, EM_SETPASSWORDCHAR, (WPARAM) (UINT) '*', 0);
                            GetDlgItemText (hDlg, IDTXT_CODIGO, szCodigo, LNG_CODIGO + 1);
                            SendDlgItemMessage (hDlg, IDTXT_CODIGO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) szCodigo);
                            wci.bMascara = TRUE;
                        }
                        else
                        {
                            hWndBtn = GetDlgItem (hDlg, IDTXT_CONFIRMACION);
                            EnableWindow (hWndBtn, FALSE);
                            GetDlgItemText (hDlg, IDTXT_CODIGO, szCodigo, LNG_CODIGO + 1);
                            SendDlgItemMessage (hDlg, IDTXT_CODIGO, EM_SETPASSWORDCHAR, (WPARAM) (UINT) '\0', 0);
                            SendDlgItemMessage (hDlg, IDTXT_CODIGO, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) szCodigo);
                            wci.bMascara = FALSE;
                        }
                        return TRUE;

                    default:
                        return FALSE;
                }
            }
            return FALSE;


        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT);
                    break;

                case PSN_WIZNEXT:
                    if (!wci.bArchivoFuenteElegido)
                    {
                        MessageBox(NULL,"Debes elegir un archivo primero",NULL,MB_OK | MB_ICONEXCLAMATION);
                        SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                    }
                    else
                    {
                        if (wci.bAutomatico)
                        {   // No quites esta llave
                            if (!wci.bNombreAutomaticoCreado && wci.bEstamosEncriptando)
                            {
                                MessageBox(NULL,"El nombre automático es muy largo. Selecciona un destino",NULL,MB_OK | MB_ICONEXCLAMATION);
                                hWndBtn = GetDlgItem (hDlg, IDBT_DESTINO);
                                EnableWindow (hWndBtn, TRUE);
                                CheckDlgButton (hDlg, IDCHK_AUTOMATICO, BST_UNCHECKED);
                                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                                break;
                            }
                        }
                        else
                        {
                            if (!wci.bArchivoDestinoElegido)
                            {
                                MessageBox(NULL,"Selecciona automatico o elige un destino",NULL,MB_OK | MB_ICONEXCLAMATION);
                                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                                break;
                            }
                        }
                        GetDlgItemText (hDlg, IDTXT_CODIGO, szCodigo, LNG_CODIGO + 1);
                        if (strlen (szCodigo) != LNG_CODIGO)
                        {
                            MessageBox(NULL,"El codigo no tiene 9 digitos",NULL,MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                        }
                        else
                        {
                            if (wci.bMascara)
                            {
                                GetDlgItemText (hDlg, IDTXT_CONFIRMACION, szConfirmacion, LNG_CODIGO + 1);
                                if (strcmp (szCodigo, szConfirmacion))
                                {
                                    MessageBox(NULL,"La confirmacion no concuerda",NULL,MB_OK | MB_ICONEXCLAMATION);
                                    SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                                    break;
                                }
                            }
                            strcpy (wci.codigo, szCodigo);
                            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                        }
                    }
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}



//  Esta funcion procesa los mensajes de la segunda hoja del Wizard Cripto
//
LRESULT CALLBACK FinalizarCriptoDialogProc (HWND hDlg,
                                            UINT message,
                                            UINT wParam,
                                            LONG lParam)
{

char    linea[MAX_PATH + 15];   // +4 para los espacios +10 para el tamaño
HWND    hLista;


    switch (message)
    {
        case WM_INITDIALOG:     
            // Especificamos el ancho visible en pxls de la Lista (por HScroll) 
            SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_SETHORIZONTALEXTENT, (WPARAM) 1600, 0);
            return TRUE;

        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code)
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg),  PSWIZB_BACK | PSWIZB_FINISH);
                    // limpiar lista:
                    SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_RESETCONTENT, 0, 0);

                    // Mostrar la informacion
                    hLista = GetDlgItem (hDlg, IDL_RESUMEN);
                    strcpy (linea, "[ ARCHIVO FUENTE ]");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    ");
                    strcat (linea, wci.szNombreArchivoFuente);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " ");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ ARCHIVO DESTINO ]");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    ");
                    if (wci.bAutomatico && !wci.bEstamosEncriptando)
                        strcat (linea, "<Automatico>");
                    else
                        strcat (linea, wci.szNombreArchivoDestino);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " ");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ CODIGO ENCRIPTACION ]");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    ");
                    if (!wci.bMascara)
                        strcat (linea, wci.codigo);
                    else
                        strcat (linea, "<Personal>");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    break;

                case PSN_WIZFINISH:
                    wci.bFin = TRUE;
                    break;

                default:
                    return FALSE;

            }
            return TRUE;
    }
    return FALSE;
}


//  Esta funcion procesa los mensajes de la primera hoja del Wizard Password
//
LRESULT CALLBACK PasswordDialogProc (HWND hDlg,
                                     UINT message,
                                     UINT wParam,
                                     LONG lParam)
{

char            cad [MAX_PATH];
char            szRespuesta [MAX_RESPUESTA + 1];
int             n, k;
UINT            un = 0;
HWND            hwndCB;
HWND            hWndParent;
RECT            rc;

    switch (message)
    {
        case WM_INITDIALOG:
            // Centramos el wizard
            hWndParent = GetParent (hDlg);
            GetWindowRect (hWndParent, & rc);
            MoveWindow (hWndParent,
                        (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                        (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                        (rc.right - rc.left),
                        (rc.bottom - rc.top ),
                        TRUE);
            // Inicializamos valores
            // La tecla rapida &Azul (Alt+A) no funciona porque choca con &Atras del wizard
            CheckDlgButton (hDlg, IDPB_AZUL, BST_CHECKED);
            wpwi.opcion = 0;
            // Cargar el combo
            hwndCB = GetDlgItem(hDlg, IDCB_MES);
            SendMessage(hwndCB, CB_RESETCONTENT, 0, 0);
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Enero");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Febrero");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Marzo");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Abril");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Mayo");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Junio");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Julio");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Agosto");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Septiembre");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Octubre");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Noviembre");
            SendMessage(hwndCB, CB_ADDSTRING, 0, (LPARAM) "Diciembre");
            SendDlgItemMessage (hDlg, IDCB_MES, CB_SETCURSEL, (WPARAM) 0, 0);
            // Limitar campo de texto
            SendDlgItemMessage (hDlg, IDTX_RESPUESTA, EM_SETLIMITTEXT, (WPARAM) MAX_RESPUESTA, 0);
            SendDlgItemMessage (hDlg, IDTX_DIA, EM_SETLIMITTEXT, (WPARAM) 2, 0);
            SendDlgItemMessage (hDlg, IDTX_ANYO, EM_SETLIMITTEXT, (WPARAM) 4, 0);
            return TRUE;

    
        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {    
                switch( LOWORD(wParam))
                {
                    case IDPB_AZUL:
                        LoadString (hInstance, IDS_PREGUNTA1, cad, sizeof(cad));
                        wpwi.opcion = 0;
                        goto PONER_PREGUNTA;

                    case IDPB_ROJO:
                        LoadString (hInstance, IDS_PREGUNTA2, cad, sizeof(cad));
                        wpwi.opcion = 1;
                        goto PONER_PREGUNTA;

                    case IDPB_VERDE:
                        LoadString (hInstance, IDS_PREGUNTA3, cad, sizeof(cad));
                        wpwi.opcion = 2;
PONER_PREGUNTA:
                        SendDlgItemMessage (hDlg, IDLB_PREGUNTA, WM_SETTEXT , 0, (LPARAM)(LPCTSTR) cad);
                        return TRUE;

                    default:
                        return FALSE;
                }
            }
            return FALSE;


        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT);
                    break;

                case PSN_WIZNEXT:
                    // verificamos respuesta
                    GetDlgItemText (hDlg, IDTX_RESPUESTA, szRespuesta, MAX_RESPUESTA + 1);
                    n = lstrlen (szRespuesta);
                    if (n < 2)
                    {
                        MessageBox (NULL, "La respuesta minima es de 2 caracteres!", NULL, MB_OK | MB_ICONEXCLAMATION);
                        SetWindowLong (hDlg, DWL_MSGRESULT, TRUE);
                        break;
                    }
                    else
                    {
                        strcpy (wpwi.szRespuesta, szRespuesta);
                        // Rellenamos la palabra con espacios
                        for( k = n; k < 10 ; k++)
                            wpwi.szRespuesta [k] = ' ';
                        wpwi.szRespuesta [k] = 0;

                        // Verificamos dia
                        un = (UINT) GetDlgItemInt (hDlg, IDTX_DIA, NULL, FALSE);
                        if ((un < 1) || (un > 31))
                        {
                            MessageBox (NULL,"El dia debe estar entre 1 y 31",NULL, MB_OK | MB_ICONEXCLAMATION);
                            SetWindowLong (hDlg,    DWL_MSGRESULT, TRUE);
                        }
                        else
                        {
                            wpwi.nDia = (int) un;
                            // Verificamos anyo
                            un = (UINT) GetDlgItemInt (hDlg, IDTX_ANYO, NULL, FALSE);
                            if ((un < 1900) || (un > 2010))
                            {
                                MessageBox(NULL,"El año debe estar entre 1900 y 2010",NULL,MB_OK | MB_ICONEXCLAMATION);
                                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                            }
                            else
                            {
                                wpwi.nAnyo = (int) un;
                                wpwi.nMes = (int) (SendDlgItemMessage (hDlg, IDCB_MES, CB_GETCURSEL, 0, 0) + 1);
                                SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
                            }
                        }
                    }
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}


//  Esta funcion procesa los mensajes de la segunda hoja del Wizard Password
//
LRESULT CALLBACK FinalizarPasswordDialogProc (HWND hDlg,
                                              UINT message,
                                              UINT wParam,
                                              LONG lParam)
{

        char    linea[MAX_PATH + 15];   // +4 para los espacios +10 para el tamaño
        HWND    hLista;
        int     n, m, k;
static  char    sz [MAX_RESPUESTA + 1];
        char    st [8];
        char    *p;


    switch (message)
    {
        case WM_INITDIALOG:     
            return TRUE;

        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code)
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg),  PSWIZB_BACK | PSWIZB_FINISH);
                    // limpiar lista:
                    SendDlgItemMessage (hDlg, IDL_RESUMEN, LB_RESETCONTENT, 0, 0);

                    // Calculamos el numero de password
                    {   
                        n = wpwi.nAnyo % 10;
                        m = wpwi.nAnyo / 10;
                        sz [0] = (char) (m % 19);
                        sz [1] = (char) (m % 17);
                        sz [2] = (char) (m % 13);
                        sz [3] = (char) (n * 11);
                        sz [4] = (char) wpwi.nMes;
                        sz [5] = (char) (wpwi.nMes * 7);
                        sz [6] = (char) wpwi.nDia;
                        sz [7] = (char) (wpwi.nDia * 3);
                        sz [8] = (char) (wpwi.nDia + wpwi.nMes) + wpwi.opcion;

                        for( k = 0; k < MAX_RESPUESTA; k++)
                            sz [k] = ((sz [k] + wpwi.szRespuesta [k]) % 10) + 48;
                        
                        sz [9] = 0;
                    }

                    // Mostrar la informacion
                    hLista = GetDlgItem (hDlg, IDL_RESUMEN);
                    if (wpwi.opcion == 0)
                        strcpy (st, "AZUL");
                    else
                        if (wpwi.opcion == 1)
                            strcpy (st, "ROJA");
                        else
                            strcpy (st, "VERDE");
                    wsprintf (linea, "[ RESPUESTA A LA PREGUNTA %s ]", st);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, "    ");
                    strcat (linea, wpwi.szRespuesta);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " ");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ FECHA QUE RECUERDAS SIEMPRE ]");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    wsprintf (linea, "    (d/m/a):  %d/%d/%d", wpwi.nDia, wpwi.nMes, wpwi.nAnyo);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " ");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, "[ CODIGO GENERADO ]");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    wsprintf (linea, "    %s", sz);
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " ");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);

                    strcpy (linea, " Elegir <FINALIZAR> copiará este código al");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    strcpy (linea, " clipboard. Pégala <Ctrl + V> donde quieras..");
                    SendMessage (hLista, LB_ADDSTRING, 0, (LPARAM) linea);
                    break;

                case PSN_WIZFINISH:
                    if ( OpenClipboard(hWndMain))
                    {
                        EmptyClipboard ();
                        hGout = GlobalAlloc (GHND |         // Combines the GMEM_MOVEABLE and GMEM_ZEROINIT flags
                                             GMEM_DDESHARE, 
                                             MAX_RESPUESTA + 1);
                        if (hGout == NULL) 
                        { 
                            CloseClipboard(); 
                            break; 
                        } 
                        p = GlobalLock (hGout);
                        strcpy (p, sz);
                        GlobalUnlock (hGout);
                        SetClipboardData (CF_TEXT, hGout);
                        CloseClipboard ();
                    }
                    break;

                default:
                    return FALSE;

            }
            return TRUE;
    }
    return FALSE;
}
