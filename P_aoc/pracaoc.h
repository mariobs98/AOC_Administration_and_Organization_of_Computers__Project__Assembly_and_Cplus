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


#ifndef PRACAOC_H
#define PRACAOC_H

#include <ui_mainForm.h>
#include <QtCore>
#include <QPainter>
#include <QtWidgets/QFileDialog>
#include <QMouseEvent>
#include <iostream>
#include "imageprocess.h"

#define IMAGEW 640
#define IMAGEH 480
#define IMAGED 800

using namespace Ui;


class pracAOC:public QWidget
{
    Q_OBJECT

public:
    pracAOC();
    ~pracAOC();
		
		

private:

		QMainForm * mainWin;
		QImage * qimgOrig, * qimgDest;
        uchar * imgO, * imgD, * imgDC, * imgAux;
        uchar tLUT[256];

		QVector<QRgb> ctable;

        QTimer timer;

        void copyImgD2ImgDC();
        void copyImgDC2ImgD();

protected:
		void closeEvent(QCloseEvent *event);

public slots:

		void paintEvent(QPaintEvent *);
		void cargar();
		void guardar();
        void process();
        void changeScrollLimits(int nz);
};




#endif
