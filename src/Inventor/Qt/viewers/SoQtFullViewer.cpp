/**************************************************************************
 *
 *  This file is part of the Coin SoQt GUI binding library.
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version
 *  2.1 as published by the Free Software Foundation.  See the file
 *  LICENSE.LGPL at the root directory of the distribution for all the
 *  details.
 *
 *  If you want to use Coin SoQt for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition License.
 *
 *  Systems in Motion, Prof Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
 **************************************************************************/

#if SOQT_DEBUG
static const char rcsid[] =
  "$Id$";
#endif

/*!
  \class SoQtFullViewer SoQtFullViewer.h Inventor/Qt/viewers/SoQtFullViewer.h
  \brief The SoQtFullViewer class adds decorations to the simple viewer of the parent.
  \ingroup soqtviewers

  FIXME: proper class documentation
  ...overview of what this class provides over parent class...
  ...keyboard combinations...mousebutton combinations...explain pan,
  rotate, zoom, dolly, seek...explain the fact that we're moving the
  camera, not the scene...
  ...viewer modes (examine vs interact) w/ explanation of what they are
  useful for...

  \sa SoQtConstrainedViewer, SoQtExaminerViewer, SoQtPlaneViewer
*/

// FIXME: this sourcefile is way too large -- this reeks of bad
// design. Should try to factor out lots of stuff, like the preference
// sheet. 20010821 mortene.

// FIXME: can hide more stuff from the public header file into
// SoQtFullViewerP -- most or all of the private methods, for
// instance. 20010821 mortene.

//    FIXME, low priority:
//     * add tooltips to the widgets?
//     * trim length of edit fields

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qmetaobject.h>
#include <moc_SoQtFullViewer.cpp>

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/errors/SoDebugError.h>

#include <soqtdefs.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/widgets/SoQtThumbWheel.h>
#include <Inventor/Qt/widgets/SoQtPopupMenu.h>
#include <Inventor/Qt/viewers/SoAnyFullViewer.h>
#include <Inventor/Qt/viewers/SoQtFullViewer.h>

// Button icons.
#include <Inventor/Qt/common/pixmaps/pick.xpm>
#include <Inventor/Qt/common/pixmaps/view.xpm>
#include <Inventor/Qt/common/pixmaps/help.xpm>
#include <Inventor/Qt/common/pixmaps/home.xpm>
#include <Inventor/Qt/common/pixmaps/set_home.xpm>
#include <Inventor/Qt/common/pixmaps/view_all.xpm>
#include <Inventor/Qt/common/pixmaps/seek.xpm>

// *************************************************************************

SOQT_OBJECT_ABSTRACT_SOURCE(SoQtFullViewer);

// *************************************************************************

/*!
  \enum SoQtFullViewer::BuildFlag
  FIXME: write documentation for enum
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_NONE
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_DECORATION
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_POPUP
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_ALL
  FIXME: write documentation for enum definition
*/

/*!
  \var QWidget * SoQtFullViewer::leftDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::leftWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::leftWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::leftWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::leftWheelVal
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::bottomWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::bottomWheelVal
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::rightWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::rightWheelVal
  FIXME: write doc
*/

/*!
  \var SoQtPopupMenu * SoQtFullViewer::prefmenu
  FIXME: write doc
*/

// *************************************************************************

static const int VIEWERBORDER = 2;
static const int ZOOMSLIDERRESOLUTION = 200;


///////// FIXME start //////////////////////////////////////////////////
// Do something clever about this Qt layout assistant code..
// 990222 mortene.

enum LayoutOrientation { LayoutVertical, LayoutHorizontal };

void
expandSize(QSize & result, const QSize & addend, LayoutOrientation o)
{
  if (o == LayoutHorizontal) {
    result.rwidth() += addend.width();
    if (result.height() < addend.height()) result.setHeight(addend.height());
  }
  else {
    result.rheight() += addend.height();
    if (result.width() < addend.width()) result.setWidth(addend.width());
  }
}

///////// FIXME end ////////////////////////////////////////////////////


// *************************************************************************

// Button index values.
enum {
  INTERACT_BUTTON = 0,
  EXAMINE_BUTTON,
  HELP_BUTTON,
  HOME_BUTTON,
  SET_HOME_BUTTON,
  VIEW_ALL_BUTTON,
  SEEK_BUTTON
};

// *************************************************************************

class SoQtFullViewerP {

public:

  // Constructor.
  SoQtFullViewerP(SoQtFullViewer * o)
    : owner(o)
  { }

  // Destructor.
  ~SoQtFullViewerP() { }

  // Return pointer to pushbutton in right-side decoration bar.
  QPushButton * getViewerbutton(const int idx)
  {
    return (QPushButton *)this->viewerbuttons->get(idx);
  }

  QWidget * viewerwidget, * canvas;
  QWidget * interactbutton, * viewbutton;
  SbBool decorations;
  SbString menutitle;
  SbBool menuenabled;
  QLayout * mainlayout;
  QLayout * appbuttonlayout;
  QWidget * appbuttonform;
  SbPList * appbuttonlist;
  SbPList * viewerbuttons;
  QWidget * prefwindow;
  SbString prefwindowtitle;
  QSlider * zoomslider;
  QLineEdit * zoomfield, * zoomrangefrom, * zoomrangeto;
  SbVec2f zoomrange;
  SoQtThumbWheel * seekdistancewheel;
  QLineEdit * seekdistancefield;
  QLabel * nearclippinglabel, * farclippinglabel;
  SoQtThumbWheel * nearclippingwheel, * farclippingwheel;
  QLineEdit * nearclippingedit, * farclippingedit;

private:
  SoQtFullViewer * owner;
};

#define PRIVATE(o) (o->pimpl)

// *************************************************************************

/*!
  Constructor. See parent class for explanation of arguments.

  Subclasses will probably usually call with the \a buildNow flag
  set to \c FALSE to be able to do delayed building of the OpenGL
  canvas after other setup tasks has been performed.
*/

SoQtFullViewer::SoQtFullViewer(
  QWidget * parent,
  const char * name,
  SbBool embed,
  SoQtFullViewer::BuildFlag buildFlag,
  SoQtViewer::Type type,
  SbBool build)
: inherited(parent, name, embed, type, FALSE)
, common(new SoAnyFullViewer(this))
{
  PRIVATE(this) = new SoQtFullViewerP(this);

  PRIVATE(this)->viewerwidget = NULL;
  PRIVATE(this)->canvas = NULL;
//  PRIVATE(this)->canvasparent = NULL;

  PRIVATE(this)->viewbutton = NULL;
  PRIVATE(this)->interactbutton = NULL;

  this->leftDecoration = NULL;
  this->bottomDecoration = NULL;
  this->rightDecoration = NULL;

  this->leftWheel = NULL;
  this->leftWheelLabel = NULL;
  this->leftWheelStr = NULL;
  this->leftWheelVal = 0.0f;

  this->bottomWheel = NULL;
  this->bottomWheelLabel = NULL;
  this->bottomWheelStr = NULL;
  this->bottomWheelVal = 0.0f;

  this->rightWheel = NULL;
  this->rightWheelLabel = NULL;
  this->rightWheelStr = NULL;
  this->rightWheelVal = 0.0f;

  this->setLeftWheelString("Motion X");
  this->setBottomWheelString("Motion Y");
  this->setRightWheelString("Motion Z");

  PRIVATE(this)->zoomrange = SbVec2f(1.0f, 140.0f);

  PRIVATE(this)->mainlayout = NULL;
  PRIVATE(this)->appbuttonlayout = NULL;

  PRIVATE(this)->prefwindow = NULL;
  PRIVATE(this)->prefwindowtitle = "Viewer Preference Sheet";

  PRIVATE(this)->menuenabled = buildFlag & SoQtFullViewer::BUILD_POPUP;
  PRIVATE(this)->decorations =
    (buildFlag & SoQtFullViewer::BUILD_DECORATION) ? TRUE : FALSE;

  this->prefmenu = NULL;
  PRIVATE(this)->menutitle = "Viewer Menu";

  PRIVATE(this)->viewerbuttons = new SbPList;
  PRIVATE(this)->appbuttonlist = new SbPList;
  PRIVATE(this)->appbuttonform = NULL;

  this->setSize(SbVec2s(500, 390));

  if (! build) return;

  this->setClassName("SoQtFullViewer");
  QWidget * viewer = this->buildWidget(this->getParentWidget());
  this->setBaseWidget(viewer);
} // SoQtFullViewer()

// *************************************************************************

/*!
  Destructor.
*/

SoQtFullViewer::~SoQtFullViewer(
  void)
{
  delete PRIVATE(this)->viewerbuttons;
  delete PRIVATE(this)->appbuttonlist;

  delete [] this->leftWheelStr;
  delete [] this->rightWheelStr;
  delete [] this->bottomWheelStr;

  delete PRIVATE(this);
} // ~SoQtFullViewer()

// *************************************************************************

/*!
  Turn the viewer decorations on or off.

  \sa isDecoration()
*/

void
SoQtFullViewer::setDecoration(
  const SbBool enable)
{
#if SOQT_DEBUG
  if ((enable  && this->isDecoration()) ||
       (!enable && !this->isDecoration())) {
    SoDebugError::postWarning("SoQtFullViewer::setDecoration",
                              "decorations already turned %s",
                              enable ? "on" : "off");
    return;
  }
#endif // SOQT_DEBUG

  PRIVATE(this)->decorations = enable;
  if (PRIVATE(this)->viewerwidget)
    this->showDecorationWidgets(enable);
} // setDecoration()

// *************************************************************************

/*!
  Return \c TRUE if the viewer decorations are on, otherwise \c FALSE.

  \sa setDecoration()
*/

SbBool
SoQtFullViewer::isDecoration(
  void) const
{
  return PRIVATE(this)->decorations;
} // isDecoration()

// *************************************************************************

/*!
  Decide whether or not if clicking with the right mouse button on the
  OpenGL canvas should reveal a preferences popup menu when in viewing
  mode.

  \sa isPopupMenuEnabled()
*/

void
SoQtFullViewer::setPopupMenuEnabled(
  const SbBool enable)
{
#if SOQT_DEBUG
  if ((enable && this->isPopupMenuEnabled()) ||
       (!enable && !this->isPopupMenuEnabled())) {
    SoDebugError::postWarning("SoQtFullViewer::setPopupMenuEnabled",
                              "popup menu already turned %s",
                              enable ? "on" : "off");
    return;
  }
#endif // SOQT_DEBUG
  PRIVATE(this)->menuenabled = enable;
} // setPopupMenuEnabled()

// *************************************************************************

/*!
  Return \c TRUE if the popup preferences menu is enabled,
  otherwise \c FALSE.

  \sa setPopupMenuEnabled()
*/

SbBool
SoQtFullViewer::isPopupMenuEnabled(
  void) const
{
  return PRIVATE(this)->menuenabled;
} // isPopupMenuEnabled()

// *************************************************************************

/*!
  Returns the widget which is used as the parent of application
  specified buttons. The row of application buttons (if any) will be
  placed in the upper left corner.

  \sa addAppPushButton(), insertAppPushButton(), removeAppPushButton()
*/

QWidget *
SoQtFullViewer::getAppPushButtonParent(void) const
{
  return PRIVATE(this)->appbuttonform;
} // getAppPushButtonParent()

// *************************************************************************

/*!
  Add an application specific push button to the viewer decorations.
  Application buttons will be laid out in a vertical row from the
  upper left corner.

  The button will be added bottom-most.

  \sa insertAppPushButton(), removeAppPushButton(), getAppPushButtonParent()
*/

void
SoQtFullViewer::addAppPushButton(QWidget * newButton)
{
  PRIVATE(this)->appbuttonlist->append(newButton);
  this->layoutAppButtons(this->getAppPushButtonParent());
} // addAppPushButton()

// *************************************************************************

/*!
  Insert an application specific push button to the viewer decorations
  at the given \c index.

  \sa addAppPushButton(), removeAppPushButton(), getAppPushButtonParent()
*/

void
SoQtFullViewer::insertAppPushButton(QWidget * newButton, int index)
{
#if SOQT_DEBUG
  if ((index < 0) || (index > this->lengthAppPushButton())) {
    SoDebugError::postWarning("SoQtFullViewer::insertAppPushButton",
                              "index %d out of bounds", index);
    return;
  }
#endif // SOQT_DEBUG
  PRIVATE(this)->appbuttonlist->insert(newButton, index);
  this->layoutAppButtons(this->getAppPushButtonParent());
} // insertAppPushButton()

// *************************************************************************

/*!
  Remove one of the application specific buttons.

  \sa addAppPushButton(), insertAppPushButton()
*/

void
SoQtFullViewer::removeAppPushButton(QWidget * oldButton)
{
  int idx = PRIVATE(this)->appbuttonlist->find(oldButton);

#if SOQT_DEBUG
  if (idx == -1) {
    SoDebugError::postWarning("SoQtFullViewer::removeAppPushButton",
                              "tried to remove non-existant button");
    return;
  }
#endif // SOQT_DEBUG

  PRIVATE(this)->appbuttonlist->remove(idx);
  this->layoutAppButtons(this->getAppPushButtonParent());
} // removeAppPushButton()

// *************************************************************************

/*!
  Return the index of a particular button that has been specified by
  the application, or -1 of the button has not been added.

  \sa addAppPushButton()
*/

int
SoQtFullViewer::findAppPushButton(QWidget * oldButton) const
{
  return PRIVATE(this)->appbuttonlist->find(oldButton);
} // findAppPushButton()

// *************************************************************************

/*!
  Return number of application specific buttons added.

  \sa addAppPushButton(), insertAddAppPushButton()
*/

int
SoQtFullViewer::lengthAppPushButton(void) const
{
  return PRIVATE(this)->appbuttonlist->getLength();
} // lengthAppPushButton()

// *************************************************************************

/*!
  Returns the render area OpenGL canvas widget.
*/

QWidget *
SoQtFullViewer::getRenderAreaWidget(void)
{
  return PRIVATE(this)->canvas;
} // getRenderAreaWidget()

// *************************************************************************

// Doc in superclass.
void
SoQtFullViewer::setComponentCursor(const SoQtCursor & cursor)
{
  // FIXME: generic method, move to common code. 20011126 mortene.

  // Overridden to apply the new cursor only for the rendering canvas
  // widget. Otherwise, the default SoQtComponent setComponentCursor()
  // method will set the cursor for the top-most parent widget, which
  // makes it affect all sub-widgets, like the decorations stuff.

  SoQtComponent::setWidgetCursor(this->getRenderAreaWidget(), cursor);
}

// *************************************************************************

/*!
  Set a flag to indicate whether we're in viewing mode (where the user
  can drag the model or scene around), or in interaction mode (where
  all window events from mouse, keyboard, etc are sent to the scene
  graph).

  Overloaded from parent class to correctly set the user interface
  indicators on the current state, i.e. to flip the examine and
  interact pushbuttons and to grey out the seek mode activation button
  iff in interact mode.
*/

void
SoQtFullViewer::setViewing(
  SbBool enable)
{
  if ((enable && this->isViewing()) ||
       (!enable && !this->isViewing())) {
#if SOQT_DEBUG && 0 // debug
    SoDebugError::postWarning("SoQtFullViewer::setViewing",
                              "view mode already %s", on ? "on" : "off");
#endif // debug
    return;
  }

  inherited::setViewing(enable);

  // Must check that buttons have been built, in case this viewer
  // component was made without decorations.
  if (PRIVATE(this)->viewerbuttons->getLength() > 0) {
    PRIVATE(this)->getViewerbutton(EXAMINE_BUTTON)->setOn(enable);
    PRIVATE(this)->getViewerbutton(INTERACT_BUTTON)->setOn(enable ? FALSE : TRUE);
    PRIVATE(this)->getViewerbutton(SEEK_BUTTON)->setEnabled(enable);
  }
} // setViewing()

// *************************************************************************

/*!
  Overloaded from parent to make sure the user interface indicators on
  the camera features in the preferences sheet are updated correctly.
*/

void
SoQtFullViewer::setCamera(SoCamera * newCamera)
{
  inherited::setCamera(newCamera);

  if (PRIVATE(this)->prefwindow) {
    this->setZoomSliderPosition(this->getCameraZoom());
    this->setZoomFieldString(this->getCameraZoom());

    SbBool on = newCamera ? TRUE : FALSE;
    PRIVATE(this)->zoomslider->setEnabled(on);
    PRIVATE(this)->zoomfield->setEnabled(on);
    PRIVATE(this)->zoomrangefrom->setEnabled(on);
    PRIVATE(this)->zoomrangeto->setEnabled(on);
  }
} // setCamera()

// *************************************************************************

/*!
  Overloaded from parent class to make sure the preferences window
  will be hidden automatically whenever the viewer window is hidden.
*/

void
SoQtFullViewer::hide(void)
{
  inherited::hide();
  if (PRIVATE(this)->prefwindow) PRIVATE(this)->prefwindow->hide();
} // hide()

// *************************************************************************

/*!
  \internal

  Catch close events on the preferences window (to convert to hide
  events).
*/

bool
SoQtFullViewer::eventFilter(QObject * obj, QEvent * e)
{
  inherited::eventFilter(obj, e);

  // Catch pref window close events to avoid it actually being
  // destroyed.
  if (obj == PRIVATE(this)->prefwindow && e->type() == QEvent::Close) {
    ((QCloseEvent *)e)->ignore();
    PRIVATE(this)->prefwindow->hide();
    return TRUE;
  }

  return FALSE;
} // eventFilter()

// *************************************************************************

/*!
  This will build the main view widgets, along with the decorations
  widgets and popup menu if they are enabled.
*/

QWidget *
SoQtFullViewer::buildWidget(
  QWidget * parent)
{
#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewer::buildWidget", "[invoked]");
#endif // SOQT_DEBUG
  PRIVATE(this)->viewerwidget = new QWidget(parent);
  this->registerWidget(PRIVATE(this)->viewerwidget);
//  PRIVATE(this)->viewerwidget->installEventFilter(this);

  PRIVATE(this)->viewerwidget->move(0, 0);
#if SOQT_DEBUG && 0
  PRIVATE(this)->viewerwidget->setBackgroundColor(QColor(250, 0, 0));
#endif // SOQT_DEBUG

  // Build and layout the widget components of the viewer window on
  // top of the manager widget.

//  PRIVATE(this)->canvasparent = new QWidget(PRIVATE(this)->viewerwidget);
//  PRIVATE(this)->canvasparent->move(0, 0);

  PRIVATE(this)->canvas = inherited::buildWidget(PRIVATE(this)->viewerwidget);
  if (PRIVATE(this)->decorations) {
    PRIVATE(this)->canvas->move(30, 0);
    PRIVATE(this)->canvas->resize(
      QSize(PRIVATE(this)->viewerwidget->size().width() - 60,
             PRIVATE(this)->viewerwidget->size().height() - 30));
  } else {
    PRIVATE(this)->canvas->move(0, 0);
    PRIVATE(this)->canvas->resize(PRIVATE(this)->viewerwidget->size());
  }

  if (PRIVATE(this)->decorations) {
    this->buildDecoration(PRIVATE(this)->viewerwidget);
    this->showDecorationWidgets(TRUE);
  }

  if (PRIVATE(this)->menuenabled)
    this->buildPopupMenu();

  return PRIVATE(this)->viewerwidget;
} // buildWidget()

// *************************************************************************

/*!
  Build viewer decorations.
*/

void
SoQtFullViewer::buildDecoration(
  QWidget * parent)
{
  this->leftDecoration = this->buildLeftTrim(parent);
  this->bottomDecoration = this->buildBottomTrim(parent);
  this->rightDecoration = this->buildRightTrim(parent);
} // buildDecoration()

// *************************************************************************

/*!
  Build decorations on the left of the render canvas.  Overload this
  method in subclasses if you want your own decorations on the viewer
  window.

  The decoration will be 30 pixels wide.
*/

QWidget *
SoQtFullViewer::buildLeftTrim(
  QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  w->setFixedWidth(30);

  QGridLayout * gl = new QGridLayout(w, 3, 1, 2, -1);
  gl->addWidget(this->buildAppButtons(w), 0, 0);

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Vertical, w);
  this->leftWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  this->leftWheelVal = t->value();

  QObject::connect(t, SIGNAL(wheelMoved(float)), this, SLOT(leftWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()), this, SLOT(leftWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()), this, SLOT(leftWheelReleased()));

  gl->addWidget(t, 2, 0, AlignBottom | AlignHCenter);
  gl->activate();

  return w;
} // buildLeftTrim()

// *************************************************************************

/*!
  Build decorations on the bottom of the render canvas. Overload this
  method in subclasses if you want your own decorations on the viewer window.
*/

QWidget *
SoQtFullViewer::buildBottomTrim(
  QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  w->setFixedHeight(30);

  QLabel * label = new QLabel(this->leftWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignLeft | AlignTop);
  label->setMargin(2);
  this->leftWheelLabel = label;

  label = new QLabel(this->bottomWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignRight | AlignVCenter);
  label->setMargin(2);
  this->bottomWheelLabel = label;

  label = new QLabel(this->rightWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignRight | AlignTop);
  label->setMargin(2);
  this->rightWheelLabel = label;

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, w);
  this->bottomWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);

  QObject::connect(t, SIGNAL(wheelMoved(float)),
                    this, SLOT(bottomWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()),
                    this, SLOT(bottomWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()),
                    this, SLOT(bottomWheelReleased()));

  this->bottomWheelVal = t->value();

  QGridLayout * gl = new QGridLayout(w, 1, 5);
  gl->setColStretch(0, 0);
  gl->setColStretch(1, 0);
  gl->setColStretch(2, 0);
  gl->setColStretch(3, 1);
  gl->setColStretch(4, 0);

  gl->addWidget(this->leftWheelLabel, 0, 0, AlignVCenter | AlignHCenter);
  gl->addWidget(this->bottomWheelLabel, 0, 1, AlignVCenter | AlignRight);
  gl->addWidget(t, 0, 2, AlignVCenter | AlignLeft);
  gl->addWidget(this->rightWheelLabel, 0, 4, AlignVCenter | AlignRight);

  gl->activate();

  return w;
} // buildBottomTrim()

// *************************************************************************

/*!
  Build decorations on the right side of the render canvas. Overload this
  method in subclasses if you want your own decorations on the viewer window.
*/

QWidget *
SoQtFullViewer::buildRightTrim(
  QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  // FIXME: should be set according to width of viewer
  // buttons. 20000424 mortene.
  w->setFixedWidth(30);
  // FIXME: nope, trims are actually guaranteed to be 30 pixels wide

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Vertical, w);
  this->rightWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  this->rightWheelVal = t->value();

  QObject::connect(t, SIGNAL(wheelMoved(float)),
                   this, SLOT(rightWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()),
                   this, SLOT(rightWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()),
                   this, SLOT(rightWheelReleased()));

  QGridLayout * l = new QGridLayout(w, 3, 1, 2, -1);
  l->setMargin(0);
  l->addWidget(this->buildViewerButtons(w), 0, 0);
  l->addWidget(t, 2, 0, AlignBottom|AlignHCenter);
  l->activate();

  return w;
} // buildRightTrim()

// *************************************************************************

/*!
  Build the application specified button row (if any buttons were
  set up).
*/

QWidget *
SoQtFullViewer::buildAppButtons(QWidget * parent)
{
  PRIVATE(this)->appbuttonform = new QWidget(parent);

  if (this->lengthAppPushButton() > 0)
    this->layoutAppButtons(PRIVATE(this)->appbuttonform);

  return PRIVATE(this)->appbuttonform;
} // buildAppButtons()

// *************************************************************************

/*!
  Build and layout viewer specified button row.
*/

QWidget *
SoQtFullViewer::buildViewerButtons(QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  this->createViewerButtons(w, PRIVATE(this)->viewerbuttons);
  w->move(0, 0);

  // assert(PRIVATE(this)->viewerbuttons->getLength() != 0);
  QGridLayout * l =
    new QGridLayout(w, PRIVATE(this)->viewerbuttons->getLength(), 1);
  l->setMargin(0);
  l->setSpacing(0);

  const int numViewerButtons = PRIVATE(this)->viewerbuttons->getLength();
  for (int i = 0; i < numViewerButtons; i++) {
    QButton * b = PRIVATE(this)->getViewerbutton(i);
    b->setFixedSize(30, 30);
    b->setFocusPolicy(QWidget::NoFocus);
    l->addWidget(b, i, 0);
  }

  l->activate();
  return w;
} // buildViewerButtons()

// *************************************************************************

/*!
  Set up the viewer buttons with pixmaps and event connections.
*/

void
SoQtFullViewer::createViewerButtons(QWidget * parent, SbPList * buttonlist)
{
  for (int i=0; i <= SEEK_BUTTON; i++) {
    QPushButton * p = new QPushButton(parent);
    // Button focus doesn't really make sense in the way we're using
    // the pushbuttons.
    p->setFocusPolicy(QWidget::NoFocus);

    // FIXME: its wrong to cast xpm data to (const char **), it
    // _should_ be (const char * const []), but this is not valid ANSI
    // C++. Not sure what to do about this (and the wrong cast can
    // (and has) in fact lead to bugs, so the situation should be
    // resolved). 20000505 mortene.

    switch (i) {
    case INTERACT_BUTTON:
      PRIVATE(this)->interactbutton = p;
      p->setToggleButton(TRUE);
      p->setPixmap(QPixmap((const char **)pick_xpm));
      p->setOn(this->isViewing() ? FALSE : TRUE);
      QObject::connect(p, SIGNAL(clicked()),
                       this, SLOT(interactbuttonClicked()));
      break;
    case EXAMINE_BUTTON:
      PRIVATE(this)->viewbutton = p;
      p->setToggleButton(TRUE);
      p->setPixmap(QPixmap((const char **)view_xpm));
      p->setOn(this->isViewing());
      QObject::connect(p, SIGNAL(clicked()),
                       this, SLOT(viewbuttonClicked()));
      break;
    case HELP_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), this, SLOT(helpbuttonClicked()));
      p->setPixmap(QPixmap((const char **)help_xpm));
      break;
    case HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), this, SLOT(homebuttonClicked()));
      p->setPixmap(QPixmap((const char **)home_xpm));
      break;
    case SET_HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
                       this, SLOT(sethomebuttonClicked()));
      p->setPixmap(QPixmap((const char **)set_home_xpm));
      break;
    case VIEW_ALL_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
                       this, SLOT(viewallbuttonClicked()));
      p->setPixmap(QPixmap((const char **)view_all_xpm));
      break;
    case SEEK_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), this, SLOT(seekbuttonClicked()));
      p->setPixmap(QPixmap((const char **)seek_xpm));
      break;
    default:
      assert(0);
      break;
    }

    p->adjustSize();
    buttonlist->append(p);
  }
} // createViewerButtons()

// *************************************************************************

/*!
  Make a popup menu with preferences settings.
*/

void
SoQtFullViewer::buildPopupMenu(
  void)
{
  this->prefmenu = this->common->setupStandardPopupMenu();
} // buildPopupMenu()

// *************************************************************************

/*!
  Set title of popup menu.
*/

void
SoQtFullViewer::setPopupMenuString(const char * str)
{
  this->common->setPopupMenuString(str);
} // setPopupMenuString()

// *************************************************************************

/*!
  NOTE: This method is not part of the original InventorXt API.
*/

void
SoQtFullViewer::openPopupMenu(// protected
  const SbVec2s position)
{
  if (! this->isPopupMenuEnabled()) return;
  if (this->prefmenu == NULL)
    this->buildPopupMenu();
  int x = 2 + position[0];
  int y = 2 + this->getGLSize()[1] - position[1];

  this->common->prepareMenu(this->prefmenu);
  this->prefmenu->popUp(this->getGLWidget(), x, y);
} // openPopupMenu()

// *************************************************************************

/*!
  Overload this method in subclass viewers to append more widgets to
  the bottom of the preferences sheet window.
*/

QWidget *
SoQtFullViewer::makeSubPreferences(QWidget * parent)
{
  return NULL;
} // makeSubPreferences()

// *************************************************************************

/*!
  Set title of preferences sheet.
*/

void
SoQtFullViewer::setPrefSheetString(const char * title)
{
  PRIVATE(this)->prefwindowtitle = title ? title : "";
  if (PRIVATE(this)->prefwindow)
    PRIVATE(this)->prefwindow->setCaption(PRIVATE(this)->prefwindowtitle.getString());
} // setPrefSheetString()

// *************************************************************************

/*!
  Called when the user start to drag the thumbwheel in the left frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa leftWheelMotion(), leftWheelFinish()
  \sa bottomWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::leftWheelStart(void)
{
  this->interactiveCountInc();
} // leftWheelStart()

/*!
  Called repeatedly as the user drags the thumbwheel in the left frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa leftWheelStart(), leftWheelFinish()
  \sa bottomWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::leftWheelMotion(
  float value)
{
  this->leftWheelVal = value;
} // leftWheelMotion()

/*!
  Called as the user let go of the thumbwheel in the left frame after
  a drag operation. Overload this method in subclassed viewers to provide
  your own functionality on the thumbwheel.

  \sa leftWheelStart(), leftWheelMotion()
  \sa bottomWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::leftWheelFinish(void)
{
  this->interactiveCountDec();
} // leftWheelFinish()

/*!
  Get current value of the left thumbwheel.

  \sa leftWheelMotion()
*/

float
SoQtFullViewer::getLeftWheelValue(
  void) const
{
  return this->leftWheelVal;
} // getLeftWheelValue()

/*!
  FIXME: write doc
*/

void
SoQtFullViewer::setLeftWheelValue(
  const float value)
{
  this->leftWheelVal = value;
  ((SoQtThumbWheel *) this->leftWheel)->setValue(value);
} // setLeftWheelValue()

// *************************************************************************

/*!
  Called when the user start to drag the thumbwheel in the bottom frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa bottomWheelMotion(), bottomWheelFinish()
  \sa leftWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::bottomWheelStart(void)
{
  this->interactiveCountInc();
} // bottomWheelStart()

/*!
  Called repeatedly as the user drags the thumbwheel in the bottom frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa bottomWheelStart(), bottomWheelFinish()
  \sa leftWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::bottomWheelMotion(
  float value)
{
  this->bottomWheelVal = value;
} // bottomWheelMotion()

/*!
  Called as the user let go of the thumbwheel in the bottom frame after
  a drag operation. Overload this method in subclassed viewers to provide
  your own functionality on the thumbwheel.

  \sa bottomWheelStart(), bottomWheelMotion()
  \sa leftWheelStart(), rightWheelStart()
*/

void
SoQtFullViewer::bottomWheelFinish(void)
{
  this->interactiveCountDec();
} // bottomWheelFinish()

/*!
  Get current value of the bottom thumbwheel.

  \sa bottomWheelMotion()
*/

float
SoQtFullViewer::getBottomWheelValue(
  void) const
{
  return this->bottomWheelVal;
} // getBottomWheelValue()

/*!
  FIXME: write doc
*/

void
SoQtFullViewer::setBottomWheelValue(
  const float value)
{
  this->bottomWheelVal = value;
  ((SoQtThumbWheel *) this->bottomWheel)->setValue(value);
} // setBottomWheelValue()

// *************************************************************************

/*!
  Called when the user start to drag the thumbwheel in the right frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa rightWheelMotion(), rightWheelFinish()
  \sa leftWheelStart(), bottomWheelStart()
*/

void
SoQtFullViewer::rightWheelStart(void)
{
  this->interactiveCountInc();
} // rightWheelStart()

/*!
  Called repeatedly as the user drags the thumbwheel in the right frame.
  Overload this method in subclassed viewers to provide your own
  functionality on the thumbwheel.

  \sa rightWheelStart(), rightWheelFinish()
  \sa leftWheelStart(), bottomWheelStart()
*/

void
SoQtFullViewer::rightWheelMotion(
  float value)
{
  this->rightWheelVal = value;
} // rightWheelMotion()

/*!
  Called as the user let go of the thumbwheel in the right frame after
  a drag operation. Overload this method in subclassed viewers to provide
  your own functionality on the thumbwheel.

  \sa rightWheelStart(), rightWheelMotion()
  \sa leftWheelStart(), bottomWheelStart()
*/

void
SoQtFullViewer::rightWheelFinish(void)
{
  this->interactiveCountDec();
} // rightWheelFinish()

/*!
  Get current value of the right thumbwheel.

  \sa rightWheelMotion()
*/

float
SoQtFullViewer::getRightWheelValue(
  void) const
{
  return this->rightWheelVal;
} // getRightWheelValue()

/*!
  FIXME: write doc
*/

void
SoQtFullViewer::setRightWheelValue(
  const float value)
{
  this->rightWheelVal = value;
  ((SoQtThumbWheel *) this->rightWheel)->setValue(value);
} // setRightWheelValue()

// *************************************************************************

// These are all private slots for catching Qt events.
void SoQtFullViewer::leftWheelPressed(void) { this->leftWheelStart(); }
void SoQtFullViewer::leftWheelChanged(float v) { this->leftWheelMotion(v); }
void SoQtFullViewer::leftWheelReleased(void) { this->leftWheelFinish(); }
void SoQtFullViewer::bottomWheelPressed(void) { this->bottomWheelStart(); }
void SoQtFullViewer::bottomWheelChanged(float v) { this->bottomWheelMotion(v);}
void SoQtFullViewer::bottomWheelReleased(void) { this->bottomWheelFinish(); }
void SoQtFullViewer::rightWheelPressed(void) { this->rightWheelStart(); }
void SoQtFullViewer::rightWheelChanged(float v) { this->rightWheelMotion(v); }
void SoQtFullViewer::rightWheelReleased(void) { this->rightWheelFinish(); }

// *************************************************************************

/*!
  Set label of the left thumbwheel.
*/

void
SoQtFullViewer::setLeftWheelString(
  const char * const string)
{
  delete [] this->leftWheelStr;
  this->leftWheelStr = NULL;

  if (string)
    this->leftWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->leftWheelLabel)
    ((QLabel *)this->leftWheelLabel)->setText(string ? string : "");
} // setLeftWheelString()

// *************************************************************************

/*!
  Set label of the bottom thumbwheel.
*/

void
SoQtFullViewer::setBottomWheelString(
  const char * const string)
{
  delete [] this->bottomWheelStr;
  this->bottomWheelStr = NULL;

  if (string)
    this->bottomWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->bottomWheelLabel)
    ((QLabel *)this->bottomWheelLabel)->setText(string ? string : "");
} // setBottomWheelString()

// *************************************************************************

/*!
  Set label of the right thumbwheel.
*/

void
SoQtFullViewer::setRightWheelString(
  const char * const string)
{
  delete [] this->rightWheelStr;
  this->rightWheelStr = NULL;

  if (string)
    this->rightWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->rightWheelLabel)
    ((QLabel *)this->rightWheelLabel)->setText(string ? string : "");
} // setRightWheelString()

// *************************************************************************

/*!
  Overload this method to provide functionality when the user clicks
  the Help button. Default implementation does nothing.
*/

void
SoQtFullViewer::openViewerHelpCard(void)
{
} // openViewerHelpCard()

// *************************************************************************

/*!
  \internal

  Show or hide decorations. Will make and activate a Qt layout grid
  if we're turning the decorations on.
*/

void
SoQtFullViewer::showDecorationWidgets(
  SbBool onOff)
{
#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewer::showDecorationWidgets", "[invoked]");
#endif // SOQT_DEBUG

  if (PRIVATE(this)->mainlayout) delete PRIVATE(this)->mainlayout;

  assert(PRIVATE(this)->viewerwidget);
//  assert(PRIVATE(this)->canvasparent);

  assert(this->leftDecoration && this->bottomDecoration && this->rightDecoration);
  if (onOff) {
    this->leftDecoration->show();
    this->bottomDecoration->show();
    this->rightDecoration->show();

    QGridLayout * g = new QGridLayout(PRIVATE(this)->viewerwidget, 2, 1, 0, -1); // VIEWERBORDER);

    g->addWidget(this->bottomDecoration, 1, 0);

    QGridLayout * subLayout = new QGridLayout(1, 3, 0);
    g->addLayout(subLayout, 0, 0);

    subLayout->addWidget(this->leftDecoration, 0, 0);
    subLayout->addWidget(PRIVATE(this)->canvas, 0, 1);
    subLayout->addWidget(this->rightDecoration, 0, 2);

//     subLayout->setColStretch(1, 1);
//     g->setRowStretch(0, 1);

    PRIVATE(this)->mainlayout = g;
  } else {
    QGridLayout * g = new QGridLayout(PRIVATE(this)->viewerwidget, 1, 1, 0, -1);
    g->addWidget(PRIVATE(this)->canvas, 0, 0);
    PRIVATE(this)->mainlayout = g;

    this->leftDecoration->hide();
    this->bottomDecoration->hide();
    this->rightDecoration->hide();
  }

  PRIVATE(this)->mainlayout->activate();
  QSize size = PRIVATE(this)->viewerwidget->size();
  SbVec2s rasize = SbVec2s(size.width(), size.height());
  SoQtFullViewer::sizeChanged(rasize);
} // showDecorationWidgets()

// *************************************************************************

/*!
  Layout application specified buttons.
*/

void
SoQtFullViewer::layoutAppButtons(QWidget * form)
{
  delete PRIVATE(this)->appbuttonlayout; PRIVATE(this)->appbuttonlayout = NULL;

  int nrbuttons = PRIVATE(this)->appbuttonlist->getLength();
  if (nrbuttons == 0) return;

  PRIVATE(this)->appbuttonlayout = new QGridLayout(form, nrbuttons, 1);

  for (int i=0; i < nrbuttons; i++) {
    QWidget * button = (QWidget *)((*(PRIVATE(this)->appbuttonlist))[i]);
    button->show();
    ((QGridLayout *)PRIVATE(this)->appbuttonlayout)->addWidget(button, i, 0);
  }

  PRIVATE(this)->appbuttonlayout->activate();
} // layoutAppButtons()

// *************************************************************************

/*!
  Create preferences sheet.
*/

QWidget *
SoQtFullViewer::makePreferencesWindow(void)
{
  QWidget * top = new QWidget(NULL);
  top->setCaption(PRIVATE(this)->prefwindowtitle.getString());
  top->setIconText(PRIVATE(this)->prefwindowtitle.getString());

  QVBoxLayout * layout = new QVBoxLayout(top, 10);

  QWidget * w;
  w = this->makeSeekPreferences(top);
  if (w) layout->addWidget(w, w->height());
  w = this->makeSeekDistancePreferences(top);
  if (w) layout->addWidget(w, w->height());
  w = this->makeZoomPreferences(top);
  if (w) layout->addWidget(w, w->height());
  w = this->makeAutoclipPreferences(top);
  if (w) layout->addWidget(w, w->height());

  w = this->makeSubPreferences(top);
  if (w) layout->addWidget(w, w->height());

  layout->activate();
  top->adjustSize();
  return top;
} // makePreferencesWindow()

// *************************************************************************

/*!
  Create the UI representation of the preferences' settings for the
  seek-to-point functionality.
*/

QWidget *
SoQtFullViewer::makeSeekPreferences(QWidget * parent)
{
  // FIXME: it'd be nice to show separation in the prefs sheet, but
  // this code doesn't work for some reason (because of the
  // layout objects?). 990221 mortene.
#if 0
  QFrame * w = new QFrame(parent);
  w->setFrameStyle(QFrame::Box | QFrame::Plain);
  w->setLineWidth(1);
#else
  QWidget * w = new QWidget(parent);
#endif

  // Initialize objects keeping track of geometry data.

  QSize tmpsize(0, 0);
  QSize totalsize(0, 0);

  QVBoxLayout * toplayout = new QVBoxLayout(w);

  // First, do the three widgets on the uppermost row (label,
  // lineedit, label).

  QLabel * l1 = new QLabel("Seek animation time:", w);
  l1->adjustSize();
  expandSize(tmpsize, l1->size(), LayoutHorizontal);

  QLineEdit * le = new QLineEdit(w);
#if QT_VERSION < 200 // Qt 2.xx
  QObject::connect(le, SIGNAL(textChanged(const char *)),
                   this, SLOT(seekAnimationTimeChanged(const char *)));
#else // Qt 1.xx
  QObject::connect(le, SIGNAL(textChanged(const QString &)),
                   this, SLOT(seekAnimationTimeChanged(const QString &)));
#endif // Qt 1.xx
  QString s;
  s.setNum(this->getSeekTime(), 'f', 2);
  le->setText(s);
  le->adjustSize();
  expandSize(tmpsize, le->size(), LayoutHorizontal);

  QLabel * l2 = new QLabel("seconds", w);
  l2->adjustSize();
  expandSize(tmpsize, l2->size(), LayoutHorizontal);

  // Layout row 1.
  QBoxLayout * layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(l1, l1->width());
  layout->addWidget(le, le->width());
  layout->addWidget(l2, l2->width());
  expandSize(totalsize, tmpsize, LayoutVertical);


  // Do the three widgets on the second row (label, radiobutton,
  // radiobutton).

  tmpsize = QSize(0, 0);

  QLabel * l3 = new QLabel("Seek to:", w);
  l3->adjustSize();
  expandSize(tmpsize, l3->size(), LayoutHorizontal);

  QButtonGroup * bg = new QButtonGroup;
  QRadioButton * r1 = new QRadioButton("point", w);
  r1->setChecked(this->isDetailSeek());
  r1->adjustSize();
  expandSize(tmpsize, r1->size(), LayoutHorizontal);
  bg->insert(r1);

  QRadioButton * r2 = new QRadioButton("object", w);
  r2->setChecked(!this->isDetailSeek());
  r2->adjustSize();
  expandSize(tmpsize, r2->size(), LayoutHorizontal);
  bg->insert(r2);

  bg->setExclusive(TRUE);
  QObject::connect(bg, SIGNAL(clicked(int)),
                   this, SLOT(seekDetailToggled(int)));

  // Layout row 2.
  layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(l3, l3->width());
  layout->addWidget(r1, r1->width());
  layout->addWidget(r2, r2->width());
  expandSize(totalsize, tmpsize, LayoutVertical);

  w->resize(totalsize);
  toplayout->activate();
  return w;
} // makeSeekPreferences()

// *************************************************************************

/*!
  Create the UI representation of the preferences' settings for the
  seek-to-point functionality.
*/

QWidget *
SoQtFullViewer::makeSeekDistancePreferences(QWidget * parent)
{
  QWidget * w = new QWidget(parent);

  // Initialize objects keeping track of geometry data.

  QSize tmpsize(0, 0);
  QSize totalsize(0, 0);

  QVBoxLayout * toplayout = new QVBoxLayout(w);

  // First, do the three widgets on the uppermost row (label,
  // thumbwheel, lineedit).

  QLabel * l = new QLabel("Seek distance:", w);
  l->adjustSize();
  expandSize(tmpsize, l->size(), LayoutHorizontal);

  PRIVATE(this)->seekdistancewheel = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, w);
  PRIVATE(this)->seekdistancewheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  PRIVATE(this)->seekdistancewheel->setValue(sqrt(this->getSeekDistance()));
  PRIVATE(this)->seekdistancewheel->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->seekdistancewheel->size(),
                             LayoutHorizontal);
  QObject::connect(PRIVATE(this)->seekdistancewheel, SIGNAL(wheelMoved(float)),
                   this, SLOT(seekDistanceWheelChanged(float)));

  PRIVATE(this)->seekdistancefield = new QLineEdit(w);
  QString s;
  s.setNum(this->getSeekDistance(), 'f', 2);
  PRIVATE(this)->seekdistancefield->setText(s);
  PRIVATE(this)->seekdistancefield->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->seekdistancefield->size(),
                             LayoutHorizontal);
  QObject::connect(PRIVATE(this)->seekdistancefield, SIGNAL(returnPressed()),
                   this, SLOT(seekDistanceEdit()));

  // Layout row 1.
  QBoxLayout * layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(l, l->width());
  layout->addWidget(PRIVATE(this)->seekdistancewheel, PRIVATE(this)->seekdistancewheel->width());
  layout->addWidget(PRIVATE(this)->seekdistancefield, PRIVATE(this)->seekdistancefield->width());
  expandSize(totalsize, tmpsize, LayoutVertical);


  // Do the two widgets on the second row (radiobutton, radiobutton).

  tmpsize = QSize(0, 0);

  QButtonGroup * bg = new QButtonGroup;
  QRadioButton * r1 = new QRadioButton("percentage", w);
  r1->setChecked(this->isSeekValuePercentage());
  r1->adjustSize();
  expandSize(tmpsize, r1->size(), LayoutHorizontal);
  bg->insert(r1);

  QRadioButton * r2 = new QRadioButton("absolute", w);
  r2->setChecked(!this->isSeekValuePercentage());
  r2->adjustSize();
  expandSize(tmpsize, r2->size(), LayoutHorizontal);
  bg->insert(r2);

  bg->setExclusive(TRUE);
  QObject::connect(bg, SIGNAL(clicked(int)),
                   this, SLOT(seekDistanceTypeToggle(int)));

  // Layout row 2.
  layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(r1, r1->width());
  layout->addWidget(r2, r2->width());
  expandSize(totalsize, tmpsize, LayoutVertical);

  w->resize(totalsize);
  toplayout->activate();

  return w;
} // makeSeekDistancePreferences()

// *************************************************************************

/*!
  Create the UI representation of the preferences' settings for the
  camera zoom functionality.
*/

QWidget *
SoQtFullViewer::makeZoomPreferences(QWidget * parent)
{
  QWidget * w = new QWidget(parent);

  // Initialize objects keeping track of geometry data.

  QSize tmpsize(0, 0);
  QSize totalsize(0, 0);

  QVBoxLayout * toplayout = new QVBoxLayout(w);

  // First, do the three widgets on the uppermost row (label,
  // slider, lineedit).

  QLabel * l1 = new QLabel("Camera zoom", w);
  l1->adjustSize();
  expandSize(tmpsize, l1->size(), LayoutHorizontal);

  PRIVATE(this)->zoomslider = new QSlider(0, ZOOMSLIDERRESOLUTION,
                                 1, 0, QSlider::Horizontal, w);
  PRIVATE(this)->zoomslider->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->zoomslider->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->zoomslider, SIGNAL(sliderPressed()),
                   this, SLOT(increaseInteractiveCount()));
  QObject::connect(PRIVATE(this)->zoomslider, SIGNAL(valueChanged(int)),
                   this, SLOT(zoomSliderMoved(int)));
  QObject::connect(PRIVATE(this)->zoomslider, SIGNAL(sliderReleased()),
                   this, SLOT(decreaseInteractiveCount()));

  PRIVATE(this)->zoomfield = new QLineEdit(w);
  PRIVATE(this)->zoomfield->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->zoomfield->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->zoomfield, SIGNAL(returnPressed()),
                   this, SLOT(zoomFieldChanged()));


  // Layout row 1.
  QBoxLayout * layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(l1, l1->width());
  layout->addWidget(PRIVATE(this)->zoomslider, PRIVATE(this)->zoomslider->width());
  layout->addWidget(PRIVATE(this)->zoomfield, PRIVATE(this)->zoomfield->width());
  expandSize(totalsize, tmpsize, LayoutVertical);


  // Do the four widgets on the second row (label, lineedit, label,
  // lineedit).

  tmpsize = QSize(0, 0);

  QLabel * l2 = new QLabel("Zoom slider ranges from:", w);
  l2->adjustSize();
  expandSize(tmpsize, l2->size(), LayoutHorizontal);

  PRIVATE(this)->zoomrangefrom = new QLineEdit(w);
  QString s;
  s.setNum(PRIVATE(this)->zoomrange[0], 'f', 1);
  PRIVATE(this)->zoomrangefrom->setText(s);
  PRIVATE(this)->zoomrangefrom->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->zoomrangefrom->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->zoomrangefrom, SIGNAL(returnPressed()),
                   this, SLOT(zoomRangeChanged1()));

  QLabel * l3 = new QLabel("to:", w);
  l3->adjustSize();
  expandSize(tmpsize, l3->size(), LayoutHorizontal);

  PRIVATE(this)->zoomrangeto = new QLineEdit(w);
  s.setNum(PRIVATE(this)->zoomrange[1], 'f', 1);
  PRIVATE(this)->zoomrangeto->setText(s);
  PRIVATE(this)->zoomrangeto->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->zoomrangeto->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->zoomrangeto, SIGNAL(returnPressed()),
                   this, SLOT(zoomRangeChanged2()));


  // Layout row 2.
  layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(l2, l2->width());
  layout->addWidget(PRIVATE(this)->zoomrangefrom, PRIVATE(this)->zoomrangefrom->width());
  layout->addWidget(l3, l3->width());
  layout->addWidget(PRIVATE(this)->zoomrangeto, PRIVATE(this)->zoomrangeto->width());
  expandSize(totalsize, tmpsize, LayoutVertical);


  SoCamera * cam = this->getCamera();
  if (cam) {
    this->setZoomSliderPosition(this->getCameraZoom());
    this->setZoomFieldString(this->getCameraZoom());
  }
  else {
    PRIVATE(this)->zoomslider->setEnabled(FALSE);
    PRIVATE(this)->zoomfield->setEnabled(FALSE);
    PRIVATE(this)->zoomrangefrom->setEnabled(FALSE);
    PRIVATE(this)->zoomrangeto->setEnabled(FALSE);
  }

  w->resize(totalsize);
  toplayout->activate();

  return w;
} // makeZoomPreferences()

// *************************************************************************

/*!
  Enable or disable interaction through the near and far clipping
  widgets.
*/

void
SoQtFullViewer::setEnabledClippingWidgets(SbBool flag)
{
  PRIVATE(this)->nearclippinglabel->setEnabled(flag);
  PRIVATE(this)->nearclippingwheel->setEnabled(flag);
  PRIVATE(this)->nearclippingedit->setEnabled(flag);
  PRIVATE(this)->farclippinglabel->setEnabled(flag);
  PRIVATE(this)->farclippingwheel->setEnabled(flag);
  PRIVATE(this)->farclippingedit->setEnabled(flag);

  SoCamera * cam = this->getCamera();
  if (!cam) return;

  PRIVATE(this)->nearclippingwheel->setValue(cam->nearDistance.getValue());
  PRIVATE(this)->farclippingwheel->setValue(cam->farDistance.getValue());

  QString s;
  s.setNum(cam->nearDistance.getValue(), 'f', 3);
  PRIVATE(this)->nearclippingedit->setText(s);
  s.setNum(cam->farDistance.getValue(), 'f', 3);
  PRIVATE(this)->farclippingedit->setText(s);
} // setEnabledClippingWidgets()

// *************************************************************************

/*!
  Create the UI representation of the preferences' settings for the
  manual control of the near and far clippping planes.
*/

QWidget *
SoQtFullViewer::makeAutoclipPreferences(QWidget * dialog)
{
  QWidget * w = new QWidget(dialog);

  // Initialize objects keeping track of geometry data.

  QSize totalsize(0, 0);

  QVBoxLayout * toplayout = new QVBoxLayout(w);

  // First, do the checkbutton widget.

  QCheckBox * c = new QCheckBox("Auto clipping planes", w);
  c->adjustSize();
  c->setChecked(this->isAutoClipping());
  QObject::connect(c, SIGNAL(toggled(bool)),
                   this, SLOT(clippingToggled(bool)));

  // Layout row 1.
  toplayout->addWidget(c, c->height());
  expandSize(totalsize, c->size(), LayoutVertical);

  // Do the three widgets on the second row (label, thumbwheel,
  // lineedit).

  QSize tmpsize = QSize(0, 0);

  PRIVATE(this)->nearclippinglabel = new QLabel("near plane", w);
  PRIVATE(this)->nearclippinglabel->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->nearclippinglabel->size(), LayoutHorizontal);

  PRIVATE(this)->nearclippingwheel = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, w);
  PRIVATE(this)->nearclippingwheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  PRIVATE(this)->nearclippingwheel->adjustSize();
  PRIVATE(this)->nearclippingwheel->setEnabled(FALSE);
  expandSize(tmpsize, PRIVATE(this)->nearclippingwheel->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->nearclippingwheel, SIGNAL(wheelPressed()),
                   this, SLOT(increaseInteractiveCount()));
  QObject::connect(PRIVATE(this)->nearclippingwheel, SIGNAL(wheelMoved(float)),
                   this, SLOT(nearclippingwheelMoved(float)));
  QObject::connect(PRIVATE(this)->nearclippingwheel, SIGNAL(wheelReleased()),
                   this, SLOT(decreaseInteractiveCount()));

  PRIVATE(this)->nearclippingedit = new QLineEdit(w);
  PRIVATE(this)->nearclippingedit->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->nearclippingedit->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->nearclippingedit, SIGNAL(returnPressed()),
                   this, SLOT(nearclipEditPressed()));


  // Layout row 2.
  QBoxLayout * layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(PRIVATE(this)->nearclippinglabel, PRIVATE(this)->nearclippinglabel->width());
  layout->addWidget(PRIVATE(this)->nearclippingwheel, PRIVATE(this)->nearclippingwheel->width());
  layout->addWidget(PRIVATE(this)->nearclippingedit, PRIVATE(this)->nearclippingedit->width());
  expandSize(totalsize, tmpsize, LayoutVertical);


  // Do the three widgets on the third row (label, thumbwheel,
  // lineedit).

  tmpsize = QSize(0, 0);

  PRIVATE(this)->farclippinglabel = new QLabel("far plane", w);
  PRIVATE(this)->farclippinglabel->adjustSize();
  expandSize(tmpsize,
             QSize(QMAX(PRIVATE(this)->nearclippinglabel->width(),
                        PRIVATE(this)->farclippinglabel->width()),
                   PRIVATE(this)->farclippinglabel->height()),
             LayoutHorizontal);

  PRIVATE(this)->farclippingwheel = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, w);
  PRIVATE(this)->farclippingwheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  PRIVATE(this)->farclippingwheel->adjustSize();
  PRIVATE(this)->farclippingwheel->setEnabled(FALSE);
  expandSize(tmpsize, PRIVATE(this)->farclippingwheel->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->farclippingwheel, SIGNAL(wheelPressed()),
                   this, SLOT(increaseInteractiveCount()));
  QObject::connect(PRIVATE(this)->farclippingwheel, SIGNAL(wheelMoved(float)),
                   this, SLOT(farclippingwheelMoved(float)));
  QObject::connect(PRIVATE(this)->farclippingwheel, SIGNAL(wheelReleased()),
                   this, SLOT(decreaseInteractiveCount()));

  PRIVATE(this)->farclippingedit = new QLineEdit(w);
  PRIVATE(this)->farclippingedit->adjustSize();
  expandSize(tmpsize, PRIVATE(this)->farclippingedit->size(), LayoutHorizontal);
  QObject::connect(PRIVATE(this)->farclippingedit, SIGNAL(returnPressed()),
                   this, SLOT(farclipEditPressed()));

  // Layout row 3.
  layout = new QHBoxLayout;
  toplayout->addLayout(layout, tmpsize.height());
  layout->addWidget(PRIVATE(this)->farclippinglabel,
                    QMAX(PRIVATE(this)->nearclippinglabel->width(),
                         PRIVATE(this)->farclippinglabel->width()));
  layout->addWidget(PRIVATE(this)->farclippingwheel, PRIVATE(this)->farclippingwheel->width());
  layout->addWidget(PRIVATE(this)->farclippingedit, PRIVATE(this)->farclippingedit->width());
  expandSize(totalsize, tmpsize, LayoutVertical);

  this->setEnabledClippingWidgets(!this->isAutoClipping());

  w->resize(totalsize);
  toplayout->activate();

  return w;
} // makeAutoclipPreferences()

// *************************************************************************

/*!
  Set camera zoom value.
*/

void
SoQtFullViewer::setCameraZoom(const float val)
{
  SoCamera * cam = this->getCamera();
  if (! cam) return; // can happen for empty scenegraph

  SoType t = cam->getTypeId();

  if (t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
    ((SoPerspectiveCamera *)cam)->heightAngle = val * 2.0f * M_PI / 360.0f;
  else if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId()))
    ((SoOrthographicCamera *)cam)->height = val;
#if SOQT_DEBUG
  else assert(0);
#endif // SOQT_DEBUG
} // setCameraZoom()

// *************************************************************************

/*!
  Return camera zoom value.
*/

float
SoQtFullViewer::getCameraZoom(void)
{
  SoCamera * cam = this->getCamera();
  if (! cam) return 0.0f; // can happen for empty scenegraph

  SoType t = cam->getTypeId();

  if (t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
    return ((SoPerspectiveCamera *)cam)->heightAngle.getValue() /
      2.0f * 360.0f / M_PI;
  else if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId()))
    return ((SoOrthographicCamera *)cam)->height.getValue();

#if SOQT_DEBUG
  assert(0);
#endif // SOQT_DEBUG
  return 0.0f;
} // getCameraZoom()

// *************************************************************************

/*!
  Update the Qt slider representing the camera zoom.
*/

void
SoQtFullViewer::setZoomSliderPosition(float zoom)
{
  if (!PRIVATE(this)->prefwindow) return;

  float f =
    (zoom - PRIVATE(this)->zoomrange[0]) / (PRIVATE(this)->zoomrange[1] - PRIVATE(this)->zoomrange[0]);
  f = SoQtClamp(f, 0.0f, 1.0f) * ZOOMSLIDERRESOLUTION;

  PRIVATE(this)->zoomslider->setValue(f);
} // setZoomSliderPosition()

// *************************************************************************

/*!
  Set string in the camera zoom edit field.
*/

void
SoQtFullViewer::setZoomFieldString(float zoom)
{
  if (!PRIVATE(this)->prefwindow) return;

  QString s;
  s.setNum(zoom, 'f', 1);
  PRIVATE(this)->zoomfield->setText(s);
} // setZoomFieldString()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::interactbuttonClicked(
  void)
{
  if (PRIVATE(this)->interactbutton)
    ((QPushButton *)PRIVATE(this)->interactbutton)->setOn(TRUE);
  if (PRIVATE(this)->viewbutton)
    ((QPushButton *)PRIVATE(this)->viewbutton)->setOn(FALSE);
  if (this->isViewing())
    this->setViewing(FALSE);
} // interactbuttonToggled()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::viewbuttonClicked(
  void)
{
  if (PRIVATE(this)->interactbutton)
    ((QPushButton *)PRIVATE(this)->interactbutton)->setOn(FALSE);
  if (PRIVATE(this)->viewbutton)
    ((QPushButton *)PRIVATE(this)->viewbutton)->setOn(TRUE);
  if (! this->isViewing())
    this->setViewing(TRUE);
} // viewbuttonToggled()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::helpbuttonClicked()
{
  this->openViewerHelpCard();
} // helpbuttonClicked()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::homebuttonClicked()
{
  this->resetToHomePosition();
} // homebuttonClicked()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::sethomebuttonClicked()
{
  this->saveHomePosition();
} // sethomebuttonClicked()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::viewallbuttonClicked()
{
  this->viewAll();
} // viewallbuttonClicked()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekbuttonClicked()
{
  this->setSeekMode(this->isSeekMode() ? FALSE : TRUE);
} // seekbuttonClicked()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::selectedViewing()
{
  this->setViewing(this->isViewing() ? FALSE : TRUE);
} // selectedViewing()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::selectedDecoration()
{
  this->setDecoration(this->isDecoration() ? FALSE : TRUE);
} // selectedDecoration()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::selectedHeadlight()
{
  this->setHeadlight(this->isHeadlight() ? FALSE : TRUE);
} // selectedHeadlight()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::copyviewSelected()
{
  this->copyView(SbTime::getTimeOfDay());
} // copyviewSelected()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::pasteviewSelected()
{
  this->pasteView(SbTime::getTimeOfDay());
} // pasteviewSelected()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::selectedPrefs(void)
{
  if (!PRIVATE(this)->prefwindow) PRIVATE(this)->prefwindow = this->makePreferencesWindow();
  PRIVATE(this)->prefwindow->show();
  PRIVATE(this)->prefwindow->raise();
} // selectedPrefs()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekAnimationTimeChanged(const char * s)
{
  float val;
  if ((sscanf(s, "%f", &val) == 1) && (val >= 0.0f)) this->setSeekTime(val);
} // seekAnimationTimeChanged()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekAnimationTimeChanged(const QString & s)
{
  bool ok;
  float val = s.toFloat(&ok);
  if (ok && (val >= 0.0f)) this->setSeekTime(val);
} // seekAnimationTimeChanged()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekDetailToggled(int id)
{
  if (id == 0) this->setDetailSeek(TRUE);
  else if (id == 1) this->setDetailSeek(FALSE);
  else assert(0);
} // seekDetailToggled()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekDistanceWheelChanged(float val)
{
  if (val < 0.1f) {
    val = 0.1f;
    PRIVATE(this)->seekdistancewheel->setValue(val);
  }

  this->setSeekDistance(val * val);

  QString s;
  s.setNum(this->getSeekDistance(), 'f', 2);
  PRIVATE(this)->seekdistancefield->setText(s);
} // seekDistanceWheelChanged()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekDistanceEdit()
{
  float val;
  if ((sscanf(PRIVATE(this)->seekdistancefield->text(), "%f", &val) == 1) &&
      (val > 0.0f)) {
    this->setSeekDistance(val);
    PRIVATE(this)->seekdistancewheel->setValue(sqrt(val));
  }
  else {
    QString s;
    s.setNum(this->getSeekDistance(), 'f', 2);
    PRIVATE(this)->seekdistancefield->setText(s);
  }
} // seekDistanceEdit()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::seekDistanceTypeToggle(int id)
{
  this->setSeekValueAsPercentage(id == 0 ? TRUE : FALSE);
} // seekDistanceTypeToggle()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::zoomSliderMoved(int val)
{
  float f = val / float(ZOOMSLIDERRESOLUTION);
  f = PRIVATE(this)->zoomrange[0] + f * (PRIVATE(this)->zoomrange[1] - PRIVATE(this)->zoomrange[0]);

  this->setCameraZoom(f);
  this->setZoomFieldString(f);
} // zoomSliderMoved()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::zoomFieldChanged()
{
  float val;
  if (sscanf(PRIVATE(this)->zoomfield->text(), "%f", &val) == 1) {
    val = SoQtClamp(val, 0.001f, 179.999f);
    this->setCameraZoom(val);
    this->setZoomSliderPosition(val);
  }
  else {
    QString s;
    s.setNum(this->getCameraZoom(), 'f', 1);
    PRIVATE(this)->zoomfield->setText(s);
  }
} // zoomFieldChanged()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::zoomRangeChanged1()
{
  float val;
  if (sscanf(PRIVATE(this)->zoomrangefrom->text(), "%f", &val) == 1) {
    if (val > 0.0f && val < 180.0f && val < PRIVATE(this)->zoomrange[1]) {
      PRIVATE(this)->zoomrange[0] = val;
      this->setZoomSliderPosition(this->getCameraZoom());
    }
  }
  else {
    QString s;
    s.setNum(PRIVATE(this)->zoomrange[0], 'f', 1);
    PRIVATE(this)->zoomrangefrom->setText(s);
  }
} // zoomRangeChanged1()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::zoomRangeChanged2()
{
  float val;
  if (sscanf(PRIVATE(this)->zoomrangeto->text(), "%f", &val) == 1) {
    if (val > 0.0f && val < 180.0f && val > PRIVATE(this)->zoomrange[0]) {
      PRIVATE(this)->zoomrange[1] = val;
      this->setZoomSliderPosition(this->getCameraZoom());
    }
  }
  else {
    QString s;
    s.setNum(PRIVATE(this)->zoomrange[1], 'f', 1);
    PRIVATE(this)->zoomrangeto->setText(s);
  }
} // zoomRangeChanged2()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::clippingToggled(bool flag)
{
  this->setAutoClipping(flag);
  this->setEnabledClippingWidgets(!flag);
} // clippingToggled()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::increaseInteractiveCount()
{
  this->interactiveCountInc();
} // increaseInteractiveCount()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::decreaseInteractiveCount()
{
  this->interactiveCountDec();
} // decreaseInteractiveCount()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::nearclippingwheelMoved(float val)
{
  assert(PRIVATE(this)->nearclippingwheel != NULL &&
          PRIVATE(this)->farclippingwheel != NULL);
  if (val < 0.001f) {
    val = 0.001f;
    PRIVATE(this)->nearclippingwheel->setValue(val);
  }
  float farval = PRIVATE(this)->farclippingwheel->value();
  if (val >= farval) {
    val = farval - 0.001f;
    PRIVATE(this)->nearclippingwheel->setValue(val);
  }

  SoCamera * cam = this->getCamera();
  if (!cam) return;

  // FIXME: cut off at 0.0? And against far clipping value? 19990223 mortene.
  cam->nearDistance = val;

  QString s;
  s.setNum(cam->nearDistance.getValue(), 'f', 3);
  PRIVATE(this)->nearclippingedit->setText(s);
} // nearclippingwheelMoved()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::farclippingwheelMoved(float val)
{
  assert(PRIVATE(this)->nearclippingwheel != NULL &&
          PRIVATE(this)->farclippingwheel != NULL);
  float nearval = PRIVATE(this)->nearclippingwheel->value();
  if (nearval < 0.001f) nearval = 0.001f;
  if (val <= nearval) {
    val = nearval + 0.001f;
    PRIVATE(this)->farclippingwheel->setValue(val);
  }

  SoCamera * cam = this->getCamera();
  if (!cam) return;

  // FIXME: cut off at 0.0? And against near clipping value? 990223 mortene.
  cam->farDistance = val;

  QString s;
  s.setNum(cam->farDistance.getValue(), 'f', 3);
  PRIVATE(this)->farclippingedit->setText(s);
} // farclippingwheelMoved()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::nearclipEditPressed()
{
  SoCamera * cam = this->getCamera();
  if (!cam) return;

  float val;
  if (sscanf(PRIVATE(this)->nearclippingedit->text(), "%f", &val) == 1) {
    // FIXME: sanity check on val? 990223 mortene.
    cam->nearDistance = val;
    PRIVATE(this)->nearclippingwheel->setValue(val);
  }
  else {
    QString s;
    s.setNum(cam->nearDistance.getValue(), 'f', 3);
    PRIVATE(this)->nearclippingedit->setText(s);
  }
} // nearclipEditPressed()

// *************************************************************************

/*!
  \internal
  Qt slot.
*/

void
SoQtFullViewer::farclipEditPressed()
{
  SoCamera * cam = this->getCamera();
  if (!cam) return;

  float val;
  if (sscanf(PRIVATE(this)->farclippingedit->text(), "%f", &val) == 1) {
    // FIXME: sanity check on val? 990223 mortene.
    cam->farDistance = val;
    PRIVATE(this)->farclippingwheel->setValue(val);
  }
  else {
    QString s;
    s.setNum(cam->farDistance.getValue(), 'f', 3);
    PRIVATE(this)->farclippingedit->setText(s);
  }
} // farclipEditPressed()

// *************************************************************************

// doc in super
void          // virtual
SoQtFullViewer::sizeChanged(const SbVec2s & size)
{
#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[invoked (%d, %d)]",
                         size[0], size[1]);
#endif // SOQT_DEBUG

  if (PRIVATE(this)->decorations) {
    if (size[0] <= 60 || size[1] <= 30) return; // bogus
    if (PRIVATE(this)->viewerwidget) {
      // SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[resizing]");
      PRIVATE(this)->viewerwidget->setGeometry(0, 0, size[0], size[1]);
      PRIVATE(this)->canvas->setGeometry(30, 0, size[0] - 60, size[1] - 30);
      this->leftDecoration->resize(30, size[1] - 30);
      this->rightDecoration->setGeometry(size[0]-30, 0, size[0], size[1] - 30);
      this->bottomDecoration->setGeometry(0, size[1]-30, size[0], size[1]);
    }
    const SbVec2s rasize = SbVec2s(size[0] - 60, size[1] - 30);
    inherited::sizeChanged(rasize);
  } else {
    if (size[0] <= 0 || size[1] <= 0) return;
    if (PRIVATE(this)->viewerwidget && PRIVATE(this)->canvas) {
      // SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[resizing]");
      PRIVATE(this)->viewerwidget->setGeometry(0, 0, size[0], size[1]);
      PRIVATE(this)->canvas->setGeometry(0, 0, size[0], size[1]);
    }
    inherited::sizeChanged(size);
  }
} // sizeChanged()

// *************************************************************************

/*!
*/

SbBool
SoQtFullViewer::processSoEvent(
  const SoEvent * const event)
{
  if (common->processSoEvent(event) ||
       inherited::processSoEvent(event))
    return TRUE;
  return FALSE;
} // processSoEvent()

// *************************************************************************

void
SoQtFullViewer::afterRealizeHook(// virtual
  void)
{
  this->sizeChanged(this->getSize());
  inherited::afterRealizeHook();
} // afterRealizeHook()

// *************************************************************************

#if SOQT_DEBUG
static const char * getSoQtFullViewerRCSId(void) { return rcsid; }
#endif // SOQT_DEBUG

