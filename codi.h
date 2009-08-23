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

#ifndef _CODI_H_
#define _CODI_H_

//#define WIN32_LEAN_AND_MEAN


#include <windows.h>        // includes basic windows functionality
#include <commctrl.h>       // includes the common controls
#include <string.h>         // includes the string functions
#include <Shlobj.h>         // incluye la funcionalidad del Shell
#include <prsht.h>          // includes the property sheet functionality
#include "resource.h"       // includes the definitions for the resources
#include "CODIfunc.h"
#include "COREfunc.h"


// Barra de estado
#define ID_BARRA_ESTADO 100
#define ID_REBAR        101
#define ID_TOOLBAR      102
#define ID_TREEVIEW     103


// Valores del programa
#define NUM_BOTONES     7
#define MAX_IDS         16  // Maxima longitud de las cadenas para el toolbar IDS_...
#define MAX_IDSM        80  // y para la barra de estado IDSM_...
#define BITMAP_WIDTH    16  // Imagenes del tree    
#define BITMAP_HEIGHT   16  // "    "
#define NUM_BITMAPS_TV  6   // "    "
#define TB_ESPACIO      12  // Espacio adicional al toolbar
#define TB_ESPACIO_I_T  3   // Espacio adicional entre imagen y texto del toolbar
#define TB_IMAGEY       32  // Altura de las imagenes del Toolbar
#define TB_IMAGEX       32  // Ancho "  "
#define TB_BOTONX       76  // Ancho de los botones del Toolbar
#define ESPACIO_R_T     13  // Espacio entre el Rebar y el Tree
#define TV_MARGEN_IZQ   10  // Espacio a la derecha del Treeview
#define NUM_WIZ_PARTIR  3   // Numero de paginas del Wizard para partir
#define NUM_WIZ_REUNIR  2   // Numero de paginas del Wizard para reunir
#define NUM_WIZ_CRIPTO  2   // Numero de paginas del Wizard para codificar o Decodificar
#define MIN_BMP_MENU    18  // Altura minima del bmp del menu (16 + 2 del borde)
#define MAX_RESPUESTA   9   // tamaño de la respuesta wizz password

#define MAX_LONG_MENU   40  // (WARNING) Longitud maxima de cadena del menu owner-drawn
#define NUM_MENUS_OD    7   // (WARNING) menus owner drawn

#define DRAG_NADA           0
#define DRAG_ENCRIPTAR      1
#define DRAG_DESENCRIPTAR   2
#define DRAG_REUNIR         3
#define DRAG_PARTIR         4

#define L_COD           5               // Longitud codigo codificacion
#define L_ORD           4               // Longitud codigo ordenacion
#define LNG_CODIGO      L_COD + L_ORD   // Longitud del codigo de encriptacion
#define CODI_VERS       "Codi1.1"       // Version
#define NUM_BYTS        4096            // Numero de bytes leidos
#define LP              32              // Longitud de la palabra y del vector ordenacion
#define LC              256             // Longitud vector codificacion
#if LC > LP
    #define L   LC
#else
    #define L   LP
#endif
#define MAX_NUM         15              // Longitud maxima de cadenas-numeros
                                        // (segun mis calculos no deberia pasar de 10, para 999MB y 256 byts por bloque)

// Informacion del Wizard para partir archivos
typedef struct tagWIZPARTIRINFO
{
    int     iNumArch;
    BOOL    bFin;
    BOOL    bArchivoFuenteElegido;
    BOOL    bArchivoDestinoElegido;
    BOOL    bNumeroElegido;
    DWORD   dwTamanyoFuenteLow;
    DWORD   dwTamanyoMaxPartido;
    char    szNombreArchivoFuente [MAX_PATH + 1];
    char    szNombreArchivoGenerico [MAX_PATH + 1];
    char    szNombreSoloFuente [MAX_PATH + 1];
    char    szCarpetaDestino [MAX_PATH + 1];
} WIZPARTIRINFO, *LPWIZPARTIRINFO;


// Esta funcion tiene como parametro un tipo definido en este archivo
BOOL    FinalizarPartirTreeView (HWND, int *, LPWIZPARTIRINFO);
BOOL    Fragmentar              (HWND, HINSTANCE, HWND, LPWIZPARTIRINFO);

// Informacion del Wizard para reunir archivos
typedef struct tagWIZREUNIRINFO
{
    int     iNumArch;
    BOOL    bFin;
    BOOL    bCarpetaFuenteElegida;
    BOOL    bArchivoDestinoElegido;
    BOOL    bAutomatico;
    char    szCarpetaFuente [MAX_PATH + 1];
    char    szNombreArchivoDestino [MAX_PATH + 1];
    char    *Archivos[99];      // <Archivos> es un array de 99 punteros a caracter
} WIZREUNIRINFO, *LPWIZREUNIRINFO;

BOOL    FinalizarReunirTreeView (HWND, int *, LPWIZREUNIRINFO);
BOOL    Defragmentar            (HWND, HINSTANCE, HWND, LPWIZREUNIRINFO);

// Informacion del Wizard Cripto
typedef struct tagWIZCRIPTOINFO
{
    BOOL    bFin;
    BOOL    bEstamosEncriptando;
    BOOL    bArchivoFuenteElegido;
    BOOL    bArchivoDestinoElegido;
    BOOL    bNombreAutomaticoCreado;
    BOOL    bAutomatico;
    BOOL    bMascara;
    char    szNombreArchivoFuente [MAX_PATH + 1];
    char    szNombreArchivoDestino [MAX_PATH + 1];
    char    szNombreSoloFuente [MAX_PATH + 1];
    char    szCarpetaFuente [MAX_PATH +1];
    char    codigo[LNG_CODIGO];
} WIZCRIPTOINFO, *LPWIZCRIPTOINFO;

BOOL    CriptoTreeView  (HWND, int *, LPWIZCRIPTOINFO);
BOOL    Encriptar       (HWND, HINSTANCE, HWND, LPWIZCRIPTOINFO);
BOOL    Desencriptar    (HWND, HINSTANCE, HWND, LPWIZCRIPTOINFO);

// Informacion para el Wizard password
typedef struct tagWIZPASSWORDINFO
{
    char        opcion;
    char        szRespuesta [MAX_RESPUESTA + 1];
    int         nDia;
    int         nMes;
    int         nAnyo;
} WIZPASSWORDINFO, *LPWIZPASSWORDINFO;

// Cabecera fragmentar
typedef struct tagINFOFILEFRAG
{
    char    szCodiVersion [8];
    char    szOrigen [MAX_PATH + 1];
    int     iOrden;
    DWORD   tamanyo;
} INFOFILEFRAG;

// Cabecera cripto
typedef struct tagINFOFILECRIP
{
    char    szCodiVersion [8];
    int     iLongCadOrigen;
    DWORD   tamanyo;
} INFOFILECRIP;

// Tipo para el manejo del menu owner-drawn
typedef struct tagMIMENUITEM
{
    HBITMAP             hBitmap;
    int                 nLongText; 
    char                szText [MAX_LONG_MENU];
} MIMENUITEM, *LPMIMENUITEM;

VOID CrearMenuOwnerDrawn (HWND, HINSTANCE, LPMIMENUITEM);

// Tipo para la comparacion de archivos
typedef struct tagINFOCOMPARAR
{
    BOOL        bIgual;
    char        szFile1 [MAX_PATH + 1];
    char        szFile2 [MAX_PATH + 1];
} INFOCOMPARAR, *LPINFOCOMPARAR;

BOOL CompararArchivos (HWND, HINSTANCE, HWND, LPINFOCOMPARAR);
VOID CompararTreeView (HWND, int*, LPINFOCOMPARAR);

// Tipo para el arrastre de archivos
typedef struct tagINFODRAG
{
    int         nOpcion;
    char        szFileDrag [MAX_PATH + 1];
} INFODRAG, *LPINFODRAG;

VOID OnDropFiles (HWND, HINSTANCE, WPARAM, LPINFODRAG, BOOL*);

// Funciones llamadas por el sistema
LRESULT CALLBACK MainWndProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK About (HWND, UINT, UINT, LONG);
LRESULT CALLBACK OpcionDragDlg (HWND, UINT, UINT, LONG);

LRESULT CALLBACK BuscarArchivoPartirDialogProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK NumeroArchivosPartirDialogProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK FinalizarPartirDialogProc (HWND, UINT, UINT, LONG);

LRESULT CALLBACK BuscarCarpetaReunirDialogProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK FinalizarReunirDialogProc (HWND, UINT, UINT, LONG);

LRESULT CALLBACK CriptoDialogProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK FinalizarCriptoDialogProc (HWND, UINT, UINT, LONG);

LRESULT CALLBACK PasswordDialogProc (HWND, UINT, UINT, LONG);
LRESULT CALLBACK FinalizarPasswordDialogProc (HWND, UINT, UINT, LONG);

// Funciones de inicializacion
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);


#endif