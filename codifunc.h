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
//  Declaraciones de funciones usadas para la interface de <CODI>
#ifndef _CODIFUNC_H_
#define _CODIFUNC_H_

#include "CODI.h"

// Declaracion de funciones
HWND        CreateProgressBar       (HWND       hWnd,
                                    HINSTANCE   hInstance);

VOID        PonerDibujoMenu         (HWND       hWnd,
                                    HINSTANCE   hInstance);

VOID        OnMeasureItem           (HWND               hwnd,
                                    LPMEASUREITEMSTRUCT lpmis);

VOID        OnDrawItem              (HWND               hwnd,
                                    LPDRAWITEMSTRUCT    lpdis);

HWND        CrearBarraEstado        (HWND       hWnd, 
                                    HINSTANCE   hInstance);

HWND        CrearRebar              (HWND       hWnd, 
                                    HINSTANCE   hInstance);

HWND        CrearToolbar            (HWND       hWnd,
                                    HINSTANCE   hInstance);

int         InsertarToolbarAlRebar  (HWND       hWnd,
                                    HINSTANCE   hInstance,
                                    HWND        hWndRebar, 
                                    HWND        hWndToolbar);

HWND        CrearTreeview           (HWND       hWnd,
                                    HINSTANCE   hInstance,
                                    HWND        hWndStatus,
                                    int         iTopTree,
                                    int         gibmpTV[]);

HBITMAP     DameMiDibujoMenu        (UINT       opcion,
                                    HINSTANCE   hinst);

VOID        CrearWizardPartir       (HWND       hwndOwner,
                                    HINSTANCE   hInstance);

VOID        CrearWizardReunir       (HWND       hwndOwner,
                                    HINSTANCE   hInstance);

VOID        CrearWizardCripto       (HWND       hwndOwner,
                                    HINSTANCE   hInstance);

VOID        CrearWizardPassword     (HWND       hwndOwner,
                                    HINSTANCE   hInstance);

BOOL        MostrarDlgComunGuardar  (HWND           hwndOwner,
                                    OPENFILENAME    *ofn,
                                    LPTSTR          lpstrFile,
                                    DWORD           nMaxFile,
                                    LPCTSTR         lpstrTitulo,
                                    LPCTSTR         lpstrFiltro,
                                    LPCTSTR         lpstrDirectorioInicial,
                                    LPCTSTR         lpstrDefExt,
                                    DWORD           Flags);

BOOL        MostrarDlgComunAbrir    (HWND           hwndOwner,
                                    OPENFILENAME    *ofn,
                                    LPTSTR          lpstrFile,
                                    DWORD           nMaxFile,
                                    LPCTSTR         lpstrTitulo,
                                    LPCTSTR         lpstrFiltro,
                                    LPCTSTR         lpstrDirectorioInicial,
                                    DWORD           Flags);

VOID        CrearNombreGenerico     (char   nombre[],
                                    WORD    ExtOffset,
                                    WORD    FileOffset);

HTREEITEM   AddOneItem              (HTREEITEM  hParent,
                                    LPSTR       szText,
                                    HTREEITEM   hInsAfter, 
                                    int         iImage,
                                    int         iImageSelected, 
                                    HWND        hwndTree);
#endif