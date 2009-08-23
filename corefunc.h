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
// Declaracion de las funciones de trabajo de <CODI>
#ifndef _COREFUNC_H_
#define _COREFUNC_H_


VOID ordig (int n,
            char v [],
            int d);

BOOL cody (HWND         hwnd,
           HINSTANCE    hinst,
           HWND         hwndStatus,
           HANDLE       hArchivoFuente,
           HANDLE       hArchivoDestino,
           char         cod[],
           char         ord[],
           DWORD        dwLongitudArchivo, 
           BOOL         bEncriptar);

BOOL new_vect (char v []);

BOOL szSumar (char  a [],
              char  b [],
              char  c [],
              int   tam_c);

BOOL VerificarEspacioDisco (char    szRuta [],
                            DWORD   dwEspacio);


#endif