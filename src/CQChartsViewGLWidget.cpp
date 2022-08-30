#include <CQChartsViewGLWidget.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#include <CQGLControl.h>

#include <QMouseEvent>

#include <GL/glut.h>

struct CRGBAToFV {
  float fvalues[4];

  CRGBAToFV(const CRGBA &rgba) {
    fvalues[0] = rgba.getRedF  ();
    fvalues[1] = rgba.getGreenF();
    fvalues[2] = rgba.getBlueF ();
    fvalues[3] = rgba.getAlphaF();
  }
};

CQChartsViewGLWidget::
CQChartsViewGLWidget(CQChartsView *view) :
 QGLWidget(view), view_(view)
{
  setObjectName("glWidget");

  setMouseTracking(true);

  setAutoFillBackground(false);

  setFocusPolicy(Qt::StrongFocus);
}

void
CQChartsViewGLWidget::
initializeGL()
{
  //int         argc = 1;
  //const char *argv[] = { "CQCharts" };

  //glutInit(&argc, (char **) argv);

  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
}

void
CQChartsViewGLWidget::
resizeGL(int width, int height)
{
  if (! control_) {
    control_        = new CQGLControl(this);
    controlToolbar_ = new CQGLControlToolBar(control_);

    controlToolbar_->setParent(this);
    controlToolbar_->setVisible(true);
  }

  control_->handleResize(width, height);

  //----

  glEnable(GL_COLOR_MATERIAL);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  CRGBA ambient(0.4, 0.4, 0.4);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CRGBAToFV(ambient).fvalues);

  glEnable(GL_NORMALIZE);

  static GLfloat pos[4] = {100.0, 100.0, 100.0, 0.0};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  CRGBA diffuse(0.6, 0.2, 0.2);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, CRGBAToFV(diffuse).fvalues);

  glEnable(GL_LIGHT0);

  //----

  controlToolbar_->move(0, 0);
}

void
CQChartsViewGLWidget::
paintGL()
{
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  static GLfloat pos[4] = {5.0f, 5.0f, 10.0f, 10.0f};

  static float lambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
  static float ldiffuse[4] = {0.4f, 0.4f, 0.4f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  glLightfv(GL_LIGHT0, GL_AMBIENT, lambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiffuse);

  glEnable(GL_NORMALIZE);

  control_->updateGL();

  //---

  CQChartsView::Plots drawPlots;

  view_->getDrawPlots(drawPlots);

  for (const auto &plot : drawPlots) {
    if (plot->isVisible())
      plot->draw3D();
  }

  //---

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void
CQChartsViewGLWidget::
mousePressEvent(QMouseEvent *e)
{
  control_->handleMousePress(e);

  update();
}

void
CQChartsViewGLWidget::
mouseReleaseEvent(QMouseEvent *e)
{
  control_->handleMouseRelease(e);

  update();
}

void
CQChartsViewGLWidget::
mouseMoveEvent(QMouseEvent *e)
{
  control_->handleMouseMotion(e);

  update();

  GLint viewport[4]; /* note viewport[3] is height of window in pixels */
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble modelview[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

  GLdouble project[16];
  glGetDoublev(GL_PROJECTION_MATRIX, project);

  float x1 = float(e->x());
  float y1 = float(viewport[3] - e->y() - 1);
  float z1 = 0.0f;

  glReadPixels(e->x(), int(y1), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z1);

  GLdouble wx, wy, wz;

  gluUnProject(x1, y1, z1, modelview, project, viewport, &wx, &wy, &wz);

  auto posStr = QString("%1 %2 %3").arg(wx).arg(wy).arg(wz);

  view_->setPosText(posStr);
}

void
CQChartsViewGLWidget::
keyPressEvent(QKeyEvent *)
{
  update();
}

void
CQChartsViewGLWidget::
keyReleaseEvent(QKeyEvent *)
{
  update();
}
