/**************************************************************************\
 *
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

//  $Id$

#ifndef __QT_THUMBWHEEL_H__
#define __QT_THUMBWHEEL_H__

#include <qwidget.h>
#include <qrangecontrol.h>

class QPixmap;
class ThumbWheel;

// *************************************************************************

class QtThumbwheel : public QWidget, public QRangeControl
{
  Q_OBJECT
public:
  enum Orientation { Horizontal, Vertical };

  QtThumbwheel( QWidget * parent = 0, const char * name = 0 );
  QtThumbwheel( Orientation, QWidget * parent = 0, const char * name = 0 );
  ~QtThumbwheel(void);

  void setOrientation( Orientation );
  Orientation orientation(void) const;

  void setValue( float value );
  float value(void) const;

  void setEnabled( bool enable );
  bool isEnabled(void) const;

  enum boundaryHandling {
    CLAMP,
    MODULATE,
    ACCUMULATE
  };
  void setRangeBoundaryHandling( boundaryHandling handling );
  boundaryHandling getRangeBoundaryHandling(void) const;

  QSize sizeHint(void) const;

signals:
  void wheelPressed(void);
  void wheelMoved(float value);
  void wheelReleased(void);

protected:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);

private:
  void constructor( Orientation );

  QtThumbwheel( const QtThumbwheel & wheel );
  QtThumbwheel & operator = ( const QtThumbwheel & wheel );

  enum State { Idle, Dragging, Disabled } state;

  Orientation orient;
  float wheelValue, tempWheelValue;
  int mouseDownPos, mouseLastPos;

  void initWheel( int diameter, int width );

  ThumbWheel * wheel;
  QPixmap ** pixmaps;
  int numPixmaps;
  int currentPixmap;

}; // class QtThumbwheel

// *************************************************************************

#endif // ! __QT_THUMBWHEEL_H__
