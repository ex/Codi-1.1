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
// Especificacion de las funciones de trabajo de <CODI>
#include "CODI.h"
#include "COREfunc.h"

extern HWND CrearProgressBar (HWND hwndParent, HINSTANCE hinst, HWND hwndStatusBar);


BOOL Fragmentar (HWND               hwnd,
                 HINSTANCE          hinst,
                 HWND               hwndStatus,
                 LPWIZPARTIRINFO    lpwpi)
{

char            szNombreDestino [MAX_PATH + 1];
int             k;
int             t;
INFOFILEFRAG    inf;
char            buff [NUM_BYTS];
DWORD           dwBytesWritten = 0;
DWORD           dw1 , dw2, dw3;
HANDLE          hArchivoFuente;
HANDLE          hArchivoDestino;
BOOL            bContinuar;
HWND            hwndProgress = NULL;

    
    // Verificamos si hay espacio libre en el disco (en caso contrario cancelamos)
    if (!VerificarEspacioDisco (lpwpi->szCarpetaDestino, lpwpi->dwTamanyoFuenteLow + lpwpi->iNumArch * sizeof(inf) + 100))
        return FALSE;

    // Abrir el archivo a partir
    hArchivoFuente = CreateFile (lpwpi->szNombreArchivoFuente,
                                 GENERIC_READ,  //Para lectura
                                 0,             //No se permite compartir
                                 NULL,          //No seguridad
                                 OPEN_EXISTING, //Falla si no existe
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);                           
    if (hArchivoFuente == INVALID_HANDLE_VALUE)
    {   
        MessageBox (NULL,"El archivo fuente no pudo ser abierto. Verifica que existe y NO esta siendo usado", "Partir", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Inicializar Nombre Destino
    strcpy (szNombreDestino, lpwpi->szCarpetaDestino);
    // Si es el directorio raiz el wizard enviara: C:\"
    if (szNombreDestino [strlen (szNombreDestino) - 1] != '\\')
        strcat (szNombreDestino, "\\");
    strcat (szNombreDestino, lpwpi->szNombreArchivoGenerico);
    t = lstrlen (szNombreDestino);
    
    // Nombre del archivo fuente
    strcpy (inf.szOrigen, lpwpi->szNombreSoloFuente);

    // Creamos barra de progreso
    hwndProgress = CrearProgressBar (hwnd, hinst, hwndStatus);
    if (hwndProgress)
    {
        // Set the range and increment of the progress bar.
        SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD)lpwpi->iNumArch));
        SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM) 1, 0);
    }

    // Bucle mientras hayan archivos de destino
    for( k = 0; k < lpwpi->iNumArch; k++)
    {
        // Refrescamos la barra de progreso
        SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
        
        szNombreDestino [t-6] = (char) (48 + (k / 10));
        szNombreDestino [t-5] = (char) (48 + (k % 10));
        // Crear el archivo destino
        hArchivoDestino = CreateFile (szNombreDestino,
                                      GENERIC_WRITE,        //Para escritura
                                      0,                    //No se permite compartir
                                      NULL,                 //No seguridad
                                      CREATE_ALWAYS,        //NO Falla si ya existe
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
        if (hArchivoDestino == INVALID_HANDLE_VALUE)
        {
            MessageBox (NULL, "El archivo destino no pudo ser creado.", "Partir", MB_OK | MB_ICONERROR);
            CloseHandle (hArchivoFuente);
            if (hwndProgress)
                DestroyWindow(hwndProgress);
            return FALSE;
        }
        
        // Crear cabecera
        strcpy (inf.szCodiVersion, CODI_VERS);
        dw1 = lpwpi->dwTamanyoFuenteLow - dwBytesWritten; 
        if (dw1 < lpwpi->dwTamanyoMaxPartido)
            inf.tamanyo = dw1;
        else
            inf.tamanyo = lpwpi->dwTamanyoMaxPartido;
        inf.iOrden = k;

        // Insertar cabecera al destino
        WriteFile (hArchivoDestino, &inf, sizeof(inf), &dw1, NULL);

        // Bucles de llenado de datos
        if (inf.tamanyo <= NUM_BYTS)
        {
            if (ReadFile(hArchivoFuente, buff, inf.tamanyo, &dw1, NULL))
            {
                WriteFile (hArchivoDestino, buff, inf.tamanyo, &dw1, NULL);
                dwBytesWritten += dw1;
                CloseHandle (hArchivoDestino);
            }
            else
            {
ERROR_LECTURA:
                MessageBox (NULL, "No se pudo leer del archivo fuente.", "Partir", MB_OK | MB_ICONERROR);
                CloseHandle (hArchivoDestino);
                CloseHandle (hArchivoFuente);
                if (hwndProgress)
                    DestroyWindow(hwndProgress);
                return FALSE;
            }
        }
        else
        {
            dw2 = 0;
            bContinuar = TRUE;
            do
            {
                if (ReadFile(hArchivoFuente, buff, NUM_BYTS, &dw1, NULL))
                {
                    WriteFile (hArchivoDestino, buff, NUM_BYTS, &dw1, NULL);
                    dwBytesWritten += dw1;
                    dw2 += dw1;
                }
                else
                    goto ERROR_LECTURA;

                if (dw2 + NUM_BYTS >= inf.tamanyo)
                {
                    dw1 = inf.tamanyo - dw2;
                    if (ReadFile(hArchivoFuente, buff, dw1, &dw3, NULL))
                    {
                        WriteFile (hArchivoDestino, buff, dw1, &dw3, NULL);
                        dwBytesWritten += dw3;
                        CloseHandle (hArchivoDestino);
                        // Salir Do y continuar For
                        bContinuar = FALSE;
                    }
                    else
                        goto ERROR_LECTURA;
                }
            }while (bContinuar);
        }
    }
    CloseHandle (hArchivoFuente);
    if (hwndProgress)
        DestroyWindow(hwndProgress);
    return TRUE;
}



BOOL Defragmentar (HWND             hwnd,
                   HINSTANCE        hinst,
                   HWND             hwndStatus,
                   LPWIZREUNIRINFO  lpwri)
{

char            szNombreFuente [MAX_PATH + 1];
char            szNombreDestino [MAX_PATH + 1];
char            szCarpetaDestino [MAX_PATH + 1];
char            cad [8];
int             k;
int             t;
INFOFILEFRAG    inf;
char            buff [NUM_BYTS];
char            str [350];
DWORD           dw1, dw2, dw3;
HANDLE          hArchivoFuente;
HANDLE          hArchivoDestino = NULL;
BOOL            bContinuar;
BOOL            bCompleto;
HANDLE          hArchivos [99];
DWORD           TamanyoMax, TamanyoMin;
HWND            hwndProgress = NULL;

    
    ZeroMemory (hArchivos, sizeof(hArchivos));  

    // Verificamos los archivos encontrados y ordenamos
    for( k = 0; k < lpwri->iNumArch; k++)
    {
        strcpy (szNombreFuente, lpwri->szCarpetaFuente);
        strcat (szNombreFuente, "\\");
        strcat (szNombreFuente, lpwri->Archivos[k]);

        // Abrir un archivo y leer cabecera
        hArchivoFuente = CreateFile (szNombreFuente,
                                     GENERIC_READ,      //Para lectura
                                     0,                 //No se permite compartir
                                     NULL,              //No seguridad
                                     OPEN_EXISTING,     //Falla si no existe
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);
        if (hArchivoFuente == INVALID_HANDLE_VALUE)
        {
            wsprintf (str, "No se pudo abrir el archivo: %s. Verifica que existe y NO esta siendo usado", szNombreFuente);
            MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
            goto SALIR;
        }

        if (ReadFile(hArchivoFuente, &inf, sizeof(inf), &dw1, NULL))
        {
            // Incluir espacio para el caracter nulo
            lstrcpyn (cad, inf.szCodiVersion, 8);
            // Verificar Version
            if (strcmp (cad, CODI_VERS))
            {
ERROR_BAD_FILE:
                wsprintf (str, "El archivo fuente %s fue alterado y/o no concuerda con los otros.", szNombreFuente);
                MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
                goto SALIR_X;
            }
            else
            {
                // Verificar tamaño
                dw1 = GetFileSize (hArchivoFuente, & dw2);
                if (dw1 != 0xFFFFFFFF)
                {
                    if (dw2 != 0)
                    {
                        MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Reunir",MB_OK | MB_ICONERROR);
                        goto SALIR_X;
                    }
                }
                else
                {
                    if ( (dw3 = GetLastError()) != NO_ERROR)
                    {
                        wsprintf (str, "No se pudo calcular el tamaño de %s.", szNombreFuente);
                        MessageBox (NULL, str, "Reunir",MB_OK | MB_ICONERROR);
                        goto SALIR_X;
                    }
                }
                
                if (dw1 != inf.tamanyo + sizeof(inf))
                    goto ERROR_BAD_FILE;
                else
                {
                    // Verificar Orden
                    if ((inf.iOrden < 0) && (inf.iOrden > 98))
                        goto ERROR_BAD_FILE;
                    else
                    {
                        if (hArchivos [inf.iOrden] != 0)
                            // Si ya hubo otro con el mismo orden
                            goto ERROR_BAD_FILE;
                        else
                        {
                            // Guardamos el handle 
                            hArchivos [inf.iOrden] = hArchivoFuente;

                            // Verificamos origen
                            if (k == 0)
                            {
                                lstrcpyn (szNombreDestino, inf.szOrigen, MAX_PATH);
                                // Guardamos el tamaño (Proteccion contra mezcla de archivos partidos)
                                TamanyoMax = inf.tamanyo;
                                TamanyoMin = inf.tamanyo;
                                bCompleto = FALSE;
                            }
                            else
                            {
                                if (lstrcmpi (szNombreDestino, inf.szOrigen))
                                {
                                    hArchivos [inf.iOrden] = 0;
                                    goto ERROR_BAD_FILE;
                                }
                                else
                                {
                                    if (!bCompleto)
                                    {
                                        if (inf.tamanyo > TamanyoMax)
                                        {
                                            TamanyoMax = inf.tamanyo;
                                            bCompleto = TRUE;
                                        }
                                        if (inf.tamanyo < TamanyoMin)
                                        {
                                            TamanyoMin = inf.tamanyo;
                                            bCompleto = TRUE;
                                        }
                                    }
                                    else
                                    {
                                        // Deben haber solo dos tamaños
                                        if (!((inf.tamanyo == TamanyoMax) || (inf.tamanyo == TamanyoMin)))
                                            goto ERROR_BAD_FILE;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            wsprintf (str, "No se pudo leer archivo :%s.", szNombreFuente);
            MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
            goto SALIR_X;
        }
    }


    // Verificamos que los archivos esten completos
    for( k = 0; k < lpwri->iNumArch; k++)
    {
        if (hArchivos [k] == 0)
        {
            wsprintf (str, "El archivo de orden %d no pudo ser encontrado.", &k);
            MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
            goto SALIR;
        }
    }

    // Crear nombre de archivo de destino
    if (lpwri->bAutomatico)
    {
        strcpy (szNombreDestino, lpwri->szCarpetaFuente);
        strcat (szNombreDestino, "\\");
        t = lstrlen (szNombreDestino) + lstrlen (inf.szOrigen);
        if ( t >= MAX_PATH - 5) 
        {
            MessageBox (NULL, "Nombre automatico muy largo, por favor elige un destino.", "Reunir", MB_OK | MB_ICONERROR);
            goto SALIR;
        }
        strcat (szNombreDestino, inf.szOrigen);
        // El Treeview mostrara solo el nombre
        strcpy (lpwri->szNombreArchivoDestino, inf.szOrigen);
    }
    else
        strcpy (szNombreDestino, lpwri->szNombreArchivoDestino);

    // Verificamos que exista espacio en el disco de destino
    if (lpwri->bAutomatico)
        strcpy (szCarpetaDestino, lpwri->szCarpetaFuente);
    else
    {
        strcpy (szCarpetaDestino, lpwri->szNombreArchivoDestino);
        for ( k = strlen (szCarpetaDestino); k >= 0; k--)
        {
            if (szCarpetaDestino [k] == '\\')
            {
                szCarpetaDestino [k] = 0;
                break;
            }
        }
    }
    // Acepta solo ruta de directorios validos
    if (!VerificarEspacioDisco (szCarpetaDestino, TamanyoMax*(lpwri->iNumArch - 1) + TamanyoMin + 100))
        goto SALIR;

    // Crear archivo de destino
    hArchivoDestino = CreateFile (szNombreDestino,
                                  GENERIC_WRITE,    //Para escritura
                                  0,                //No se permite compartir
                                  NULL,             //No seguridad
                                  CREATE_ALWAYS,    //En caso de Falla si no existe
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if (hArchivoDestino == INVALID_HANDLE_VALUE)
    {   
        MessageBox (NULL,"El archivo destino no pudo ser creado.", NULL, MB_OK | MB_ICONERROR);
        goto SALIR;
    }
    
    // Creamos barra de progreso (si es que no esta creada!)
    if (!hwndProgress)
    {
        hwndProgress = CrearProgressBar (hwnd, hinst, hwndStatus);
        if (hwndProgress)
        {
            // Set the range and increment of the progress bar.
            SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD)lpwri->iNumArch));
            SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM) 1, 0);
        }
    }

    // Leer y escribir datos
    for( k = 0; k < lpwri->iNumArch; k++)
    {
        // Refrescamos la barra de progreso
        SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
        
        // Asignamos archivo
        hArchivoFuente = hArchivos[k];
        
        // Asignamos tamaño
        if (k < (lpwri->iNumArch - 1))
            inf.tamanyo = TamanyoMax;
        else
            inf.tamanyo = TamanyoMin;
        
        // Leer datos del archivo a reunir
        if (inf.tamanyo <= NUM_BYTS)
        {
            if (ReadFile (hArchivoFuente, buff, inf.tamanyo, &dw1, NULL))
            {
                WriteFile (hArchivoDestino, buff, inf.tamanyo, &dw1, NULL);
                CloseHandle (hArchivoFuente);
                hArchivos[k] = 0;
            }
            else
            {
ERROR_LECTURA:
                MessageBox (NULL, "Error de lectura", "Reunir", MB_OK | MB_ICONERROR);
                goto SALIR;
            }
        }
        else
        {
            dw2 = 0;
            bContinuar = TRUE;
            do
            {
                if (ReadFile(hArchivoFuente, buff, NUM_BYTS, &dw1, NULL))
                {
                    WriteFile (hArchivoDestino, buff, NUM_BYTS, &dw1, NULL);
                    dw2 += dw1;
                }
                else
                    goto ERROR_LECTURA;

                if (dw2 + NUM_BYTS >= inf.tamanyo)
                {
                    dw1 = inf.tamanyo - dw2;
                    if (ReadFile(hArchivoFuente, buff, dw1, &dw3, NULL))
                    {
                        WriteFile (hArchivoDestino, buff, dw1, &dw3, NULL);
                        CloseHandle (hArchivoFuente);
                        hArchivos [k] = 0;
                        // Continuar bucle For salir de Do
                        bContinuar = FALSE;
                    }
                    else
                        goto ERROR_LECTURA;
                }
            }while (bContinuar);
        }
    }
    CloseHandle (hArchivoDestino);
    if (hwndProgress)
        DestroyWindow(hwndProgress);
    return TRUE;

    // Salidas de error:
SALIR_X:
    CloseHandle (hArchivoFuente);

SALIR:
    if (hwndProgress)
        DestroyWindow(hwndProgress);
    if (hArchivoDestino) 
        CloseHandle (hArchivoDestino);
    for( k = 0; k < lpwri->iNumArch; k++)
    {
        if (hArchivos [k])
            CloseHandle (hArchivos [k]);
    }
    return FALSE;
}



BOOL Encriptar (HWND            hwnd,
                HINSTANCE       hinst,
                HWND            hwndStatus,
                LPWIZCRIPTOINFO lpwci)
{

int             k;
char            cod [L_COD + 1];
char            ord [L_ORD + 1];
char            szCarpetaDestino [MAX_PATH + 1];
char            str [350];
DWORD           dw1, dw2, dw3;
HANDLE          hArchivoFuente;
HANDLE          hArchivoDestino = NULL;
INFOFILECRIP    inf;
BOOL            bOK = FALSE;



    // Abrir archivo fuente
    hArchivoFuente = CreateFile (lpwci->szNombreArchivoFuente,
                                 GENERIC_READ,      //Para lectura
                                 0,                 //No se permite compartir
                                 NULL,              //No seguridad
                                 OPEN_EXISTING,     //Falla si no existe
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
    if (hArchivoFuente == INVALID_HANDLE_VALUE)
    {
        wsprintf (str, "No se pudo abrir el archivo: %s. Verifica que existe y NO esta siendo usado", lpwci->szNombreArchivoFuente);
        MessageBox (NULL, str, "Cripto", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    hArchivoDestino = CreateFile (lpwci->szNombreArchivoDestino,
                                  GENERIC_WRITE,        //Para escritura
                                  0,                    //No se permite 0compartir
                                  NULL,                 //No seguridad
                                  CREATE_ALWAYS,        //NO Falla si ya existe
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if (hArchivoDestino == INVALID_HANDLE_VALUE)
    {
        MessageBox (NULL, "El archivo destino no pudo ser creado.", "Cripto", MB_OK | MB_ICONERROR);
        goto SALIR;
    }

    // Inicializar cabecera
    lstrcpy (inf.szCodiVersion, CODI_VERS);
    inf.iLongCadOrigen = lstrlen (lpwci->szNombreSoloFuente);
    dw1 = GetFileSize (hArchivoFuente, & dw2);
    if (dw1 != 0xFFFFFFFF)
    {
        if (dw2 != 0)
        {
            MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Cripto",MB_OK | MB_ICONERROR);
            goto SALIR;
        }
    }
    else
    {
        if ( (dw3 = GetLastError()) != NO_ERROR)
        {
            wsprintf (str, "No se pudo calcular el tamaño de %s.", lpwci->szNombreArchivoFuente);
            MessageBox (NULL, str, "Cripto",MB_OK | MB_ICONERROR);
            goto SALIR;
        }
    }
    inf.tamanyo = dw1;

    // Verificamos que exista espacio en el disco de destino
    if (lpwci->bAutomatico)
        strcpy (szCarpetaDestino, lpwci->szCarpetaFuente);
    else
    {
        strcpy (szCarpetaDestino, lpwci->szNombreArchivoDestino);
        for ( k = strlen (szCarpetaDestino); k >= 0; k--)
        {
            if (szCarpetaDestino [k] == '\\')
            {
                szCarpetaDestino [k] = 0;
                break;
            }
        }
    }
    // Acepta solo ruta de directorios validos
    if (!VerificarEspacioDisco (szCarpetaDestino, inf.tamanyo + sizeof(inf) + inf.iLongCadOrigen + 100))
        goto SALIR;

    // Insertar cabecera al destino
    WriteFile (hArchivoDestino, &inf, sizeof(inf), &dw1, NULL);

    // Insertar nombre del Origen en destino
    WriteFile (hArchivoDestino, lpwci->szNombreSoloFuente, inf.iLongCadOrigen, &dw1, NULL);

    // Codigos de operacion
    for( k = 0; k < L_COD; k++)
        cod [k] = lpwci->codigo [k];
    cod [L_COD] = 0;
    
    for( k = 0; k < L_ORD; k++)
        ord [k] = lpwci->codigo [L_COD + k];
    ord [L_ORD] = 0;

    // Encriptar
    bOK = cody (hwnd, hinst, hwndStatus, hArchivoFuente, hArchivoDestino, cod, ord, inf.tamanyo, TRUE);
    
SALIR:
    CloseHandle (hArchivoFuente);
    if (hArchivoDestino)
        CloseHandle (hArchivoDestino);
    return (bOK);
}



BOOL Desencriptar (HWND             hwnd,
                   HINSTANCE        hinst,
                   HWND             hwndStatus,
                   LPWIZCRIPTOINFO  lpwci)
{

int             t;
int             k;
char            szNombreOrigen [MAX_PATH + 1];
char            szNombreDestino [MAX_PATH + 1];
char            szCarpetaDestino [MAX_PATH + 1];
char            cad [8];
char            cod [L_COD + 1];
char            ord [L_ORD + 1];
char            str [350];
DWORD           dw1, dw2, dw3;
HANDLE          hArchivoFuente;
HANDLE          hArchivoDestino = NULL;
INFOFILECRIP    inf;
BOOL            bOK = FALSE;

    // Abrir archivo fuente
    hArchivoFuente = CreateFile (lpwci->szNombreArchivoFuente,
                                 GENERIC_READ,      //Para lectura
                                 0,                 //No se permite compartir
                                 NULL,              //No seguridad
                                 OPEN_EXISTING,     //Falla si no existe
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
    if (hArchivoFuente == INVALID_HANDLE_VALUE)
    {
        wsprintf (str, "No se pudo abrir el archivo: %s. Verifica que existe y NO esta siendo usado", lpwci->szNombreArchivoFuente);
        MessageBox (NULL, str, "Cripto", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (ReadFile(hArchivoFuente, &inf, sizeof(inf), &dw1, NULL))
    {
        // Incluir espacio para el caracter nulo
        lstrcpyn (cad, inf.szCodiVersion, 8);
        // Verificar Version
        if (strcmp (cad, CODI_VERS))
        {
ERROR_BAD_FILE:
            wsprintf (str, "El archivo %s no tiene formato.", lpwci->szNombreArchivoFuente);
            MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
            goto SALIR;
        }
        else
        {
            // Verificar tamaño
            dw1 = GetFileSize (hArchivoFuente, & dw2);
            if (dw1 != 0xFFFFFFFF)
            {
                if (dw2 != 0)
                {
                    MessageBox (NULL,"CODI solo maneja archivos de menos de 1 Gigabyte", "Reunir",MB_OK | MB_ICONERROR);
                    goto SALIR;
                }
            }
            else
            {
                if ( (dw3 = GetLastError()) != NO_ERROR)
                {
                    wsprintf (str, "No se pudo calcular el tamaño de %s.", lpwci->szNombreArchivoFuente);
                    MessageBox (NULL, str, "Reunir",MB_OK | MB_ICONERROR);
                    goto SALIR;
                }
            }
            
            if (dw1 != inf.tamanyo + sizeof(inf) + inf.iLongCadOrigen)
                goto ERROR_BAD_FILE;
        }
    }
    else
    {
ERROR_LECTURA:
        wsprintf (str, "No se pudo leer archivo :%s.", lpwci->szNombreArchivoFuente);
        MessageBox (NULL, str, "Reunir", MB_OK | MB_ICONERROR);
        goto SALIR;
    }

    // Verificamos que exista espacio en el disco de destino
    if (lpwci->bAutomatico)
        strcpy (szCarpetaDestino, lpwci->szCarpetaFuente);
    else
    {
        strcpy (szCarpetaDestino, lpwci->szNombreArchivoDestino);
        for ( k = strlen (szCarpetaDestino); k >= 0; k--)
        {
            if (szCarpetaDestino [k] == '\\')
            {
                szCarpetaDestino [k] = 0;
                break;
            }
        }
    }
    // Acepta solo ruta de directorios validos
    if (!VerificarEspacioDisco (szCarpetaDestino, inf.tamanyo + 100))
        goto SALIR;
    
    // Leer cadena de Origen y set nombre destino
    ZeroMemory (szNombreOrigen, sizeof (szNombreOrigen));
    if (ReadFile(hArchivoFuente, szNombreOrigen, inf.iLongCadOrigen, &dw1, NULL))
    {
        if (lpwci->bAutomatico)
        {
            strcpy (szNombreDestino, lpwci->szCarpetaFuente);
            t = lstrlen (szNombreDestino) + lstrlen (szNombreOrigen);
            if ( t >= MAX_PATH - 5) 
            {
                MessageBox (NULL, "Nombre automatico muy largo, por favor elige un destino.", "Cripto", MB_OK | MB_ICONERROR);
                goto SALIR;
            }
            strcat (szNombreDestino, szNombreOrigen);
        }
        else
            strcpy (szNombreDestino, lpwci->szNombreArchivoDestino);
    }
    else
        goto ERROR_LECTURA;
    
    // Crear archivo de destino (si el destino es automatico no sobreescribira ningun archivo)
    if (lpwci->bAutomatico)
        dw1 = CREATE_NEW;
    else
        dw1 = CREATE_ALWAYS;
    hArchivoDestino = CreateFile (szNombreDestino,
                                  GENERIC_WRITE,        //Para escritura
                                  0,                    //No se permite compartir
                                  NULL,                 //No seguridad
                                  dw1,                  //De acuerdo a si es automatico o no
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if (hArchivoDestino == INVALID_HANDLE_VALUE)
    {
        MessageBox (NULL, "El archivo destino no pudo ser creado. (o el nombre del archivo destino automático ya existe -> elige otro destino)", "Cripto", MB_OK | MB_ICONERROR);
        goto SALIR;
    }
    
    lstrcpy (lpwci->szNombreArchivoDestino, szNombreDestino);

    // Codigos de operacion
    for( k = 0; k < L_COD; k++)
        cod [k] = lpwci->codigo [k];
    cod [L_COD] = 0;

    for( k = 0; k < L_ORD; k++)
        ord [k] = lpwci->codigo [L_COD + k];
    ord [L_ORD] = 0;

    // Encriptar
    bOK = cody (hwnd, hinst, hwndStatus, hArchivoFuente, hArchivoDestino, cod, ord, inf.tamanyo, FALSE);
    
SALIR:
    CloseHandle (hArchivoFuente);
    if (hArchivoDestino)
        CloseHandle (hArchivoDestino);
    return (bOK);
}



VOID ordig (int     n,
            char    v [],
            int     d)
{

char    v_t [L];
int     k;

    v_t [0] = v [n-1];

    for( k = 1; k < n; k++)
        v_t [k] = v [((k*(2*d + 1)) % n) - 1];

    for( k = 0; k < n; k++)
        v [k] = v_t [k];
}



BOOL cody (HWND         hwnd,
           HINSTANCE    hinst,
           HWND         hwndStatus,
           HANDLE       hArchivoFuente,
           HANDLE       hArchivoDestino,
           char         cod[],
           char         ord[],
           DWORD        dwLongitudArchivo, 
           BOOL         bEncriptar)
{

char    s_cod [MAX_NUM];    // almacena cadena codificadora
char    s_ord [MAX_NUM];    // almacena cadena ordenadora
char    v_cod [LC];         // Vector codificador
char    v_ord [LP];         // Vector ordenacion
DWORD   k;
int     m, n = 0;
char    cad [LP];
char    c_t [LP] = "";
char    buffRD [NUM_BYTS];
char    buffWR [NUM_BYTS];
DWORD   dwBytesProcesados = 0;
DWORD   dwBytesPorLeer;
DWORD   dwLonPalabra;
DWORD   dwBytesFaltantes;
DWORD   lpBuffRD;
DWORD   lpBuffWR;
DWORD   dw;
DWORD   dwRange;
BOOL    bUltimaPalabra;
BOOL    bSkip = FALSE;
HWND    hwndProgress;


    lstrcpy (s_cod, cod);
    lstrcpy (s_ord, ord);

    // Inicializamos vectores
    for( m = 0; m < LC; m++)
        v_cod [m] = m;
    for( m = 0; m < LP; m++)   
        v_ord [m] = m;

    // Crear el vector de codificacion
    for( m = 0; m < sizeof(cod); m++)
        ordig (LC, v_cod, cod[m] - 48);

    // Crear el vector de ordenacion
    for( m = 0; m < sizeof(ord); m++)
        ordig (LP, v_ord, ord[m] - 48);

    // Creamos barra de progreso
    hwndProgress = CrearProgressBar (hwnd, hinst, hwndStatus);
    if (hwndProgress)
    {
        // Set the range and increment of the progress bar.
        dwRange = (dwLongitudArchivo / NUM_BYTS);
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

    // Leer y procesar datos
    while (dwBytesProcesados < dwLongitudArchivo)
    {
        if ((dwLongitudArchivo - dwBytesProcesados) > NUM_BYTS)
            dwBytesPorLeer = NUM_BYTS;
        else
            dwBytesPorLeer = dwLongitudArchivo - dwBytesProcesados;

        // Leemos un bloque de bytes del archivo
        if (ReadFile (hArchivoFuente, buffRD, dwBytesPorLeer, &dw, NULL))
        {
            // Inicializar bucle
            lpBuffRD = 0;
            lpBuffWR = 0;
            bUltimaPalabra = FALSE;


            while (lpBuffWR < dwBytesPorLeer)
            {
                // Calcular bytes palabra
                dwBytesFaltantes = dwBytesPorLeer - lpBuffRD;
                if (dwBytesFaltantes > LP)
                    dwLonPalabra = LP;
                else
                {
                    dwLonPalabra = dwBytesFaltantes;
                    bUltimaPalabra = TRUE;
                }

                // Cargamos la palabra
                for( k = 0; k < dwLonPalabra; k++)
                    cad [k] = buffRD [lpBuffRD + k];

                // Incrementamos indice buffer
                lpBuffRD += dwLonPalabra;

                if (!bUltimaPalabra)
                {
                    // Reordenamos la palabra
                    for( k = 0; k < dwLonPalabra; k++)
                    {
                        if (bEncriptar)
                            c_t [k] = cad [v_ord [k]];
                        else
                            c_t [v_ord [k]] = cad [k];
                    }
                    for( k = 0; k < dwLonPalabra; k++)
                        cad [k] = c_t [k];

                    // Calculamos un nuevo codigo de ordenacion
                    if (!new_vect (s_ord))
                        goto SALIR_ERROR;
                    
                    // Recalculamos <v_ord>
                    ordig (LP, v_ord, s_ord [0] - 48);
                }

                // Codificamos la palabra
                for( k = 0; k < dwLonPalabra; k++)
                {
                    if (bEncriptar)
                        // el cast es necesario (BYTE: An 8-bit integer that is not signed.)
                        c_t [k] = v_cod [(BYTE) cad [k]];
                    else
                    {
                        for ( m = 0; m < LC; m++)
                            if (cad [k] == v_cod[m])
                                break;
                        c_t [k] = (BYTE) m;
                    }
                }

                if (!bUltimaPalabra)
                {
                    // Calculamos un nuevo codigo de codificacion
                    if (!new_vect (s_cod))
                        goto SALIR_ERROR;

                    // Recalculamos <v_cod>
                    ordig (LC, v_cod, s_cod [0] - 48);
                }
                
                // Copiamos la cadena al buffer de escritura
                for( k = 0; k < dwLonPalabra; k++)
                    buffWR [lpBuffWR + k] = c_t [k];

                // Incrementamos indice buffer
                lpBuffWR += dwLonPalabra;
            }
            
            // Refrescamos la barra de progreso
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
        {
SALIR_ERROR:
            MessageBox (NULL, "Error en modulo cripto", "Cripto", MB_OK | MB_ICONERROR);
            if (hwndProgress)
                DestroyWindow(hwndProgress);
            return FALSE;
        }

        // Escribir bytes procesados al archivo
        WriteFile (hArchivoDestino, buffWR, dwBytesPorLeer, &dw, NULL);
        dwBytesProcesados += dwBytesPorLeer;
    }

    // Comprobacion final
    if (dwBytesProcesados == dwLongitudArchivo)
    {
        if (hwndProgress)
            DestroyWindow(hwndProgress);
        return TRUE;
    }
    else
        goto SALIR_ERROR;
}



BOOL new_vect (char v [])
{

int     k;
int     tam;
int     sumdig = 0;
char    sznum [6];
char    szres [MAX_NUM];
    
    tam = lstrlen (v);
    // Verificar tamaño
    if (tam > MAX_NUM - 2)
        return FALSE;

    // Calcular la suma de digitos del numero
    for( k = 0; k < tam; k++)
        sumdig += (int) (v [k] - 48);
    
    // Convertimos el entero en cadena
    _itoa (sumdig, sznum, 10);

    // Sumamos ambos numeros
    if (!szSumar (v, sznum, szres, sizeof(szres)))
        return FALSE;

    // Remplazamos antiguo codigo
    lstrcpy (v, szres);

    return TRUE;
}


// Esta funcion suma cadenas de digitos
BOOL szSumar(char   a [],
             char   b [],
             char   c [],
             int    tam_c)
{

int     A = 1;
int     B = 1;
int     k;
int     iSuma;
int     iDig;
int     iLleva = 0;
int     max;
int     min;
int     t1, t2;


    t1 = lstrlen (a);
    t2 = lstrlen (b);

    if (t1 > t2)
    {
        max = t1;
        min = t2;
    }
    else
    {   max = t2;
        min = t1;
    }
    if (max > tam_c - 2)
        return FALSE;

    // sumar digitos cadena
    for( k = 0; k < max; k++)
    {
        if (k >= t1)
            A = 0;
        if (k >= t2)
            B = 0;
        iSuma = (A * (int)(a [k])) + (B * (int)(b [k])) + iLleva - (A + B) * 48;
        iDig =  iSuma % 10;
        if (iSuma > 9)
            iLleva = 1;
        else
            iLleva = 0;
        c [k] = (char)(iDig + 48);
    }
    
    // k ya ha sido incrementado en el bucle for, por eso:
    if (iLleva)
    {
        c [k] = '1';
        c [k + 1] = 0;
    }
    else
        c [k] = 0;

    return TRUE;

}



// Esta funcion verifica si hay espacio en el disco
//
BOOL VerificarEspacioDisco (char    szRuta [],
                            DWORD   dwEspacio)
{
ULARGE_INTEGER  uliEspacioLibreUser;
ULARGE_INTEGER  uliEspacioTotal;
ULARGE_INTEGER  uliEspacioTotalLibre;

    if (!GetDiskFreeSpaceEx (szRuta, &uliEspacioLibreUser, &uliEspacioTotal, &uliEspacioTotalLibre))
        return FALSE;
    else
    {
        if (dwEspacio >= uliEspacioLibreUser.LowPart)
        {
            MessageBox (NULL,"No hay suficiente espacio de almacenamiento en el destino!", NULL,MB_OK | MB_ICONERROR);
            return FALSE;
        }
        else
            return TRUE;
    }
}
