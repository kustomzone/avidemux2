/***************************************************************************
                          Q_eq2.cpp  -  description

                flyDialog for MPlayer EQ2 filter
    copyright            : (C) 2002/2007 by mean Fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Q_eq2.h"
#include "ADM_toolkitQt.h"
#include <QSignalMapper>

//
//	Video is in YV12 Colorspace
//
//
  Ui_eq2Window::Ui_eq2Window(QWidget *parent, eq2 *param,ADM_coreVideoFilter *in) : QDialog(parent)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;

        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyEq2(this, width, height,in,canvas,ui.horizontalSlider);
        memcpy(&(myCrop->param),param,sizeof(eq2));
        myCrop->_cookie=&ui;
        myCrop->addControl(ui.toolboxLayout);
        myCrop->upload();
        myCrop->sliderChanged();
        myCrop->update();

        QSignalMapper *signalMapper = new QSignalMapper(this);
        connect( signalMapper,SIGNAL(mapped(QWidget*)),this,SLOT(resetSlider(QWidget*)));
        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
        QString rst = QString(QT_TRANSLATE_NOOP("eq2","Reset"));
#define SPINNER(x) connect( ui.horizontalSlider##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));\
                   ui.horizontalSlider##x->setContextMenuPolicy(Qt::ActionsContextMenu);\
                   QAction *reset##x = new QAction(rst,this);\
                   ui.horizontalSlider##x->addAction(reset##x);\
                   signalMapper->setMapping(reset##x,ui.horizontalSlider##x);\
                   connect( reset##x,SIGNAL(triggered(bool)),signalMapper,SLOT(map()));
        SPINNER(Red);
        SPINNER(Blue);
        SPINNER(Green);
         
        SPINNER(Contrast);
        SPINNER(Brightness);
        SPINNER(Saturation);

        SPINNER(Initial);
        SPINNER(Weight);

        setResetSliderEnabledState();

        show();
        myCrop->adjustCanvasPosition();
        canvas->parentWidget()->setMinimumSize(30,30); // allow resizing after the dialog has settled
  }
  void Ui_eq2Window::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_eq2Window::gather(eq2 *param)
  {
    
        myCrop->download();
        memcpy(param,&(myCrop->param),sizeof(eq2));
  }

const int Ui_eq2Window::initialValues[]
{
    100, // contrast
    0,   // brightness
    100, // saturation

    100, // initial
    100, // weight

    100, // red
    100, // blue
    100  // green
};

#define RESET_SLIDER(x,y) if(control == ui.horizontalSlider##x) qobject_cast<QSlider*>(ui.horizontalSlider##x)->setValue(y);
void Ui_eq2Window::resetSlider(QWidget *control)
{
    if(!control)
        return;
    RESET_SLIDER(Contrast,initialValues[0])
    RESET_SLIDER(Brightness,initialValues[1])
    RESET_SLIDER(Saturation,initialValues[2])

    RESET_SLIDER(Initial,initialValues[3])
    RESET_SLIDER(Weight,initialValues[4])

    RESET_SLIDER(Red,initialValues[5])
    RESET_SLIDER(Blue,initialValues[6])
    RESET_SLIDER(Green,initialValues[7])
}

#define CAN_RESET(x,y)\
    if(ui.horizontalSlider##x->value() == y)\
        ui.horizontalSlider##x->actions().at(0)->setEnabled(false);\
    else\
        ui.horizontalSlider##x->actions().at(0)->setEnabled(true);
void Ui_eq2Window::setResetSliderEnabledState(void)
{
    CAN_RESET(Contrast,initialValues[0])
    CAN_RESET(Brightness,initialValues[1])
    CAN_RESET(Saturation,initialValues[2])

    CAN_RESET(Initial,initialValues[3])
    CAN_RESET(Weight,initialValues[4])

    CAN_RESET(Red,initialValues[5])
    CAN_RESET(Blue,initialValues[6])
    CAN_RESET(Green,initialValues[7])
}

/**
    \fn dtor
*/
Ui_eq2Window::~Ui_eq2Window()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_eq2Window::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->download();
  myCrop->sameImage();
  setResetSliderEnabledState();
  lock--;
}

void Ui_eq2Window::resizeEvent(QResizeEvent *event)
{
    if(!canvas->height())
        return;
    uint32_t graphicsViewWidth = canvas->parentWidget()->width();
    uint32_t graphicsViewHeight = canvas->parentWidget()->height();
    myCrop->fitCanvasIntoView(graphicsViewWidth,graphicsViewHeight);
    myCrop->adjustCanvasPosition();
}

#define sliderSet(x,y) w->horizontalSlider##x->setValue((int)(param.y*100));
#define sliderGet(x,y) param.y=w->horizontalSlider##x->value()/100.;
//************************
uint8_t flyEq2::upload(void)
{
Ui_eq2Dialog *w=(Ui_eq2Dialog *)_cookie;


        sliderSet(Contrast,contrast);
        sliderSet(Brightness,brightness);
        sliderSet(Saturation,saturation);

        sliderSet(Red,rgamma);
        sliderSet(Green,ggamma);
        sliderSet(Blue,bgamma);

        sliderSet(Initial,gamma);
        sliderSet(Weight,gamma_weight);
        
       return 1;
}
uint8_t flyEq2::download(void)
{
	Ui_eq2Dialog *w=(Ui_eq2Dialog *)_cookie;

        sliderGet(Contrast,contrast);
        sliderGet(Brightness,brightness);
        sliderGet(Saturation,saturation);

        sliderGet(Red,rgamma);
        sliderGet(Green,ggamma);
        sliderGet(Blue,bgamma);

        sliderGet(Initial,gamma);
        sliderGet(Weight,gamma_weight);

return 1;
}
/**
      \fn     DIA_getEQ2Param
      \brief  Handle MPlayer EQ2 flyDialog
*/
uint8_t DIA_getEQ2Param(eq2 *param, ADM_coreVideoFilter *in)
{
        uint8_t ret=0;
        Ui_eq2Window dialog(qtLastRegisteredDialog(), param,in);

		qtRegisterDialog(&dialog);

        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }

		qtUnregisterDialog(&dialog);

        return ret;
}
//____________________________________
// EOF


