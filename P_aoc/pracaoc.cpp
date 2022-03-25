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


#include "pracaoc.h"


pracAOC::pracAOC(): QWidget()
{
	mainWin=new QMainForm();
	mainWin->setupUi(this);


	ctable.resize(256);
	for(int i=0; i < 256; i++)
		ctable[i] = qRgb(i,i,i);

    imgO=new uchar[IMAGEW*IMAGEH];
    imgD=new uchar[IMAGEW*IMAGEH];
    imgDC=new uchar[IMAGED*IMAGED];
    imgAux=new uchar[IMAGEW*IMAGEH];
	
    for(int i=0; i<IMAGEW*IMAGEH;i++)
		imgO[i]=0;

    qimgOrig = new QImage(imgO,IMAGEW, IMAGEH, QImage::Format_Indexed8);
	qimgOrig->setColorTable(ctable);
    qimgOrig->setColorCount(256);

    qimgDest = new QImage(imgD,IMAGEW, IMAGEH, QImage::Format_Indexed8);
	qimgDest->setColorTable(ctable);
    qimgOrig->setColorCount(256);

    connect ( mainWin->pushButtonCargar, SIGNAL (clicked()), this, SLOT( cargar() ) );
	connect ( mainWin->pushButtonGuardar, SIGNAL (clicked()), this, SLOT( guardar() ) );
    connect ( mainWin->pushButtonSalir, SIGNAL (clicked()), this, SLOT( close() ) );
    connect ( mainWin->hSliderZoom, SIGNAL (valueChanged(int)), this, SLOT( changeScrollLimits(int) ) );

    connect (&timer,SIGNAL(timeout()),this,SLOT(process()));
    timer.start(33);

}


pracAOC::~pracAOC()
{
}


void pracAOC::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

    painter.drawImage(QPoint(mainWin->frameOrig->x(), mainWin->frameOrig->y()), *qimgDest);
	
	painter.end();
}

void pracAOC::cargar()
{
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(process()));

    unsigned alignedWidth;
    QImage qimgRead(IMAGEW,IMAGEH,QImage::Format_Indexed8);
	
    QString fn = QFileDialog::getOpenFileName(this, "Seleccione un fichero",".", "Images (*.png *.xpm *.jpg)");
	if(qimgRead.load(fn))
	{
		QImage qimgCRead(qimgRead.width(),qimgRead.height(),QImage::Format_Indexed8);

		qimgCRead=qimgRead.convertToFormat(QImage::Format_Indexed8, ctable);

        alignedWidth = ceil(qimgCRead.width()/4.) * 4;

        for(unsigned y=0; y<IMAGEH && y<(unsigned) qimgCRead.height();y++)
            for(unsigned x=0; x<IMAGEW && x<(unsigned) qimgCRead.width(); x++)
                imgO[y*IMAGEW+x]=(qimgCRead.bits())[(y*alignedWidth+x)];

		update();
	}

    connect(&timer,SIGNAL(timeout()),this,SLOT(process()));
}

void pracAOC::guardar()
{
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(process()));

    QImage qimgWrite(IMAGEW,IMAGEH,QImage::Format_Indexed8);
    QString fn = QFileDialog::getSaveFileName(this,"Seleccione un fichero",".", "Images (*.png *.xpm *.jpg)");
	qimgWrite.setColorTable(ctable);
    for(int y=0; y<IMAGEH;y++)
        for(int x=0; x<IMAGEW; x++)
            (qimgWrite.bits())[(y*IMAGEW+x)]=imgD[y*IMAGEW+x];

	qimgWrite.save(fn);

    connect(&timer,SIGNAL(timeout()),this,SLOT(process()));
}

void pracAOC::copyImgDC2ImgD()
{
    int dx = mainWin->horizontalScrollBar->value()-320;
    if(dx<0) dx = 0;
    if(dx>=IMAGED-IMAGEW) dx =IMAGED-IMAGEW-1;
    int dy = mainWin->verticalScrollBar->value()-240;
    if(dy<0) dy = 0;
    if(dy>=IMAGED-IMAGEH) dy =IMAGED-IMAGEH-1;

    for(int f=0; f<IMAGEH; f++)
        memcpy(&imgD[f*IMAGEW], &imgDC[(f+dy)*IMAGED+dx], IMAGEW);

}

void pracAOC::copyImgD2ImgDC()
{
    int dx = mainWin->horizontalScrollBar->value()-320;
    if(dx<0) dx = 0;
    if(dx>=IMAGED-IMAGEW) dx =IMAGED-IMAGEW-1;
    int dy = mainWin->verticalScrollBar->value()-240;
    if(dy<0) dy = 0;
    if(dy>=IMAGED-IMAGEH) dy =IMAGED-IMAGEH-1;

    memset(imgDC, 0, IMAGED*IMAGED);
    for(int f=0; f<IMAGEH; f++)
        memcpy(&imgDC[(f+dy)*IMAGED+dx], &imgD[f*IMAGEW], IMAGEW);
}


void pracAOC::process()
{
    memcpy(imgD, imgO, IMAGEW*IMAGEH);
    copyImgD2ImgDC();
    imageprocess::rotar(imgO, imgDC, (mainWin->dialRot->value()*M_PI)/180.);

    float s;
    if(mainWin->hSliderZoom->value()>10)
        s = mainWin->hSliderZoom->value()-9;
    else
        if(mainWin->hSliderZoom->value()<10)
            s =  1./(10.-mainWin->hSliderZoom->value());
        else
            s = 1.;

    copyImgDC2ImgD();

    imageprocess::zoom(imgDC, imgD, s, mainWin->horizontalScrollBar->value()-320, mainWin->verticalScrollBar->value()-240);

    if(mainWin->checkBoxVHor->isChecked())
    {
        memcpy(imgAux, imgD, IMAGEW*IMAGEH);
        imageprocess::volteoHorizontal(imgAux, imgD);
    }
    if(mainWin->checkBoxVVer->isChecked())
    {
        memcpy(imgAux, imgD, IMAGEW*IMAGEH);
        imageprocess::volteoVertical(imgAux, imgD);
    }

    memcpy(imgAux, imgD, IMAGEW*IMAGEH);
    if(!mainWin->checkBoxIlumMejorada->isChecked())
    {
        if(mainWin->hSliderIlum->value()>=0)
            imageprocess::iluminarLUT(tLUT, 255-mainWin->hSliderIlum->value());
        else
            imageprocess::oscurecerLUT(tLUT, -mainWin->hSliderIlum->value());
    }
    else
    {
        if(mainWin->hSliderIlumMejorada->value()>=0)
            imageprocess::iluminarLUTMejorado(tLUT, mainWin->hSliderIlumMejorada->value());
        else
            imageprocess::oscurecerLUTMejorado(tLUT, -mainWin->hSliderIlumMejorada->value());
    }

    imageprocess::aplicarTablaLUT(tLUT, imgAux, imgD);

    update();
}

void pracAOC::changeScrollLimits(int nz)
{
    float s;
    int nD, nMw, nMh, xC, yC;

    if(nz>10)
        s = nz-9;
    else
        if(nz<10)
            s =  1./(10.-nz);
        else
            s = 1.;

    nD = 800*s;
    xC = nD/2;
    yC = nD/2;
    nMw = nD - 320;
    nMh = nD - 240;

    if(nMw>xC)
    {
        mainWin->horizontalScrollBar->setMinimum(320);
        mainWin->horizontalScrollBar->setMaximum(nMw);
        mainWin->horizontalScrollBar->setValue(xC);
        mainWin->verticalScrollBar->setMinimum(240);
        mainWin->verticalScrollBar->setMaximum(nMh);
        mainWin->verticalScrollBar->setValue(yC);
    }
    else
    {
        mainWin->horizontalScrollBar->setMinimum(xC);
        mainWin->horizontalScrollBar->setMaximum(xC);
        mainWin->verticalScrollBar->setMinimum(yC);
        mainWin->verticalScrollBar->setMaximum(yC);
    }



}

void pracAOC::closeEvent(QCloseEvent *event)
{


}
