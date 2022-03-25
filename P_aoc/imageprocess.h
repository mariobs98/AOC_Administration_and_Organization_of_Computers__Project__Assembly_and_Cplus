/***************************************************************************
 *   Copyright (C) 2018 by pilar   *
 *   pilarb@unex.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <QtCore>

namespace imageprocess{
    extern void rotar(uchar * imgO, uchar * imgD, float angle);
    extern void zoom(uchar * imgO, uchar * imgD, float s, int dx, int dy);
    extern void volteoHorizontal(uchar * imgO, uchar * imgD);
    extern void volteoVertical(uchar * imgO, uchar * imgD);
    extern void iluminarLUT(uchar * tablaLUT, uchar gW);
    extern void oscurecerLUT(uchar * tablaLUT, uchar gB);
    extern void iluminarLUTMejorado(uchar * tablaLUT, uchar gW);
    extern void oscurecerLUTMejorado(uchar * tablaLUT, uchar gB);
    extern void aplicarTablaLUT(uchar * tablaLUT, uchar * imgO, uchar * imgD);    
}

#endif
