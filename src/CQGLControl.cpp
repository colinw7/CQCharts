#include <CQGLControl.h>
#include <CVector3D.h>
#include <CGLUtil.h>

#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QPixmap>
#include <QGLWidget>
#include <QOpenGLWidget>
#include <QMouseEvent>

#include <xpm/depth.xpm>
#include <xpm/cull.xpm>
#include <xpm/light.xpm>
#include <xpm/outline.xpm>
#include <xpm/front.xpm>
#include <xpm/smooth.xpm>

class CQToolButton : public QToolButton {
 public:
  CQToolButton(const char **xpm_data, const char *tip) :
   QToolButton(nullptr) {
    setIcon(QIcon(QPixmap(xpm_data)));

    setAutoRaise(true);
    setCheckable(true);

    setToolTip(tip);
  }
};

CQGLControl::
CQGLControl(QGLWidget *glW) :
 QObject(nullptr), glW_(glW)
{
  init();
}

CQGLControl::
CQGLControl(QOpenGLWidget *openglW) :
 QObject(nullptr), openglW_(openglW)
{
  init();
}

void
CQGLControl::
init()
{
  memset(matrix_ , 0, 16*sizeof(double));
  memset(imatrix_, 0, 16*sizeof(double));

  getMatrix();
}

CQGLControlToolBar *
CQGLControl::
createToolBar()
{
  toolbar_ = new CQGLControlToolBar(this);

  return toolbar_;
}

void
CQGLControl::
setSize(double size)
{
  size_ = size;
}

void
CQGLControl::
setCenter(double x, double y)
{
  center_x_ = x;
  center_y_ = y;
}

void
CQGLControl::
updateGL()
{
  getDepthTest  () ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  getCullFace   () ? glEnable(GL_CULL_FACE)  : glDisable(GL_CULL_FACE);
  getLighting   () ? glEnable(GL_LIGHTING)   : glDisable(GL_LIGHTING);
  getOutline    () ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) :
                     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  getFrontFace  () ? glFrontFace(GL_CW) : glFrontFace(GL_CCW);
  getSmoothShade() ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
}

void
CQGLControl::
depthSlot(bool flag)
{
  depth_test_ = flag;

  emit stateChanged();
}

void
CQGLControl::
cullSlot(bool flag)
{
  cull_face_ = flag;

  emit stateChanged();
}

void
CQGLControl::
lightSlot(bool flag)
{
  lighting_ = flag;

  emit stateChanged();
}

void
CQGLControl::
outlineSlot(bool flag)
{
  outline_ = flag;

  emit stateChanged();
}

void
CQGLControl::
frontSlot(bool flag)
{
  front_face_ = flag;

  emit stateChanged();
}

void
CQGLControl::
smoothSlot(bool flag)
{
  smooth_shade_ = flag;

  emit stateChanged();
}

void
CQGLControl::
handleResize(int w, int h)
{
  glViewport(0, 0, w, h);

  double aspect = double(w)/double(h ? h : 1);

  double size2 = size_/2.0;

  left_   = center_x_ - size2*aspect;
  right_  = center_x_ + size2*aspect;
  bottom_ = center_y_ - size2;
  top_    = center_y_ + size2;

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  glOrtho(left_, right_, bottom_, top_, znear_, zfar_);

  glGetDoublev(GL_PROJECTION_MATRIX, pmatrix_);

  glMatrixMode(GL_MODELVIEW);
}

void
CQGLControl::
handleMousePress(QMouseEvent *e)
{
  mouse_x_ = e->pos().x();
  mouse_y_ = e->pos().y();

  if      (e->button() == Qt::LeftButton ) mouse_left_   = true;
  else if (e->button() == Qt::MidButton  ) mouse_middle_ = true;
  else if (e->button() == Qt::RightButton) mouse_right_  = true;

  getMousePos(mouse_x_, mouse_y_, &drag_pos_x_, &drag_pos_y_, &drag_pos_z_);
}

void
CQGLControl::
handleMouseRelease(QMouseEvent *e)
{
  mouse_x_ = e->pos().x();
  mouse_y_ = e->pos().y();

  if      (e->button() == Qt::LeftButton ) mouse_left_   = false;
  else if (e->button() == Qt::MidButton  ) mouse_middle_ = false;
  else if (e->button() == Qt::RightButton) mouse_right_  = false;

  getMousePos(mouse_x_, mouse_y_, &drag_pos_x_, &drag_pos_y_, &drag_pos_z_);
}

bool
CQGLControl::
handleMouseMotion(QMouseEvent *e)
{
  int dx = e->pos().x() - mouse_x_;
  int dy = e->pos().y() - mouse_y_;

  if (dx == 0 && dy == 0)
    return false;

  // zoom
  if      (mouse_middle_ || (mouse_left_ && mouse_right_)) {
    double s = exp(double(dy)*0.01);

    glTranslatef( refPoint_.x,  refPoint_.y,  refPoint_.z);

    glScalef(s, s, s);

    glTranslatef(-refPoint_.x, -refPoint_.y, -refPoint_.z);
  }
  // rotate
  else if (mouse_left_) {
    double ax = dy;
    double ay = dx;
    double az = 0.0;

    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    double angle = CVector3D(ax, ay, az).length()/(double)(viewport[2]+1)*180.0;

    /* Use inverse matrix to determine local axis of rotation */

    double bx = imatrix_[0]*ax + imatrix_[4]*ay + imatrix_[8] *az;
    double by = imatrix_[1]*ax + imatrix_[5]*ay + imatrix_[9] *az;
    double bz = imatrix_[2]*ax + imatrix_[6]*ay + imatrix_[10]*az;

    glTranslatef( refPoint_.x,  refPoint_.y,  refPoint_.z);

    glRotatef(angle, bx, by, bz);

    glTranslatef(-refPoint_.x, -refPoint_.y, -refPoint_.z);
  }
  // move
  else if (mouse_right_) {
    double px, py, pz;

    getMousePos(e->pos().x(), e->pos().y(), &px, &py, &pz);

    glLoadIdentity();

    glTranslatef(px - drag_pos_x_, py - drag_pos_y_, pz - drag_pos_z_);

    glMultMatrixd(matrix_);

    drag_pos_x_ = px;
    drag_pos_y_ = py;
    drag_pos_z_ = pz;
  }

  mouse_x_ = e->pos().x();
  mouse_y_ = e->pos().y();

  getMatrix();

  return true;
}

void
CQGLControl::
getMousePos(int x, int y, double *px, double *py, double *pz)
{
  /* Use the ortho projection and viewport information to map from mouse
     co-ordinates back into world co-ordinates */

  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  *px = double(x - viewport[0])/double(viewport[2]);
  *py = double(y - viewport[1])/double(viewport[3]);

  *px = left_ + (*px)*(right_  - left_);
  *py = top_  + (*py)*(bottom_ - top_ );
  *pz = znear_;
}

void
CQGLControl::
getCameraPos(double *px, double *py, double *pz)
{
  *px = center_x_;
  *py = center_y_;
  *pz = znear_;
}

void
CQGLControl::
getMatrix()
{
  glGetDoublev(GL_MODELVIEW_MATRIX, matrix_);

  CGLUtil::invertMatrix(matrix_, imatrix_);
}

void
CQGLControl::
setMatrix(const Matrix &matrix)
{
  memcpy(&matrix_[0], &matrix[0], 16*sizeof(double));

  CGLUtil::invertMatrix(matrix_, imatrix_);
}

//------

CQGLControlToolBar::
CQGLControlToolBar(CQGLControl *control) :
 QWidget(nullptr), control_(control)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0), layout->setSpacing(0);

  depthButton_   = new CQToolButton(depth_data  , "Depth Test");
  cullButton_    = new CQToolButton(cull_data   , "Cull Face");
  lightButton_   = new CQToolButton(light_data  , "Lighting");
  outlineButton_ = new CQToolButton(outline_data, "Outline");
  frontButton_   = new CQToolButton(front_data  , "Front Face Orientation");
  smoothButton_  = new CQToolButton(smooth_data , "Smooth Shade");

  connect(depthButton_  , SIGNAL(clicked(bool)), control, SLOT(depthSlot(bool)));
  connect(cullButton_   , SIGNAL(clicked(bool)), control, SLOT(cullSlot(bool)));
  connect(lightButton_  , SIGNAL(clicked(bool)), control, SLOT(lightSlot(bool)));
  connect(outlineButton_, SIGNAL(clicked(bool)), control, SLOT(outlineSlot(bool)));
  connect(frontButton_  , SIGNAL(clicked(bool)), control, SLOT(frontSlot(bool)));
  connect(smoothButton_ , SIGNAL(clicked(bool)), control, SLOT(smoothSlot(bool)));

  layout->addWidget(depthButton_);
  layout->addWidget(cullButton_);
  layout->addWidget(lightButton_);
  layout->addWidget(outlineButton_);
  layout->addWidget(frontButton_);
  layout->addWidget(smoothButton_);

  layout->addStretch();

  setFixedHeight(32);

  connect(control, SIGNAL(stateChanged()), this, SIGNAL(stateChanged()));
}
