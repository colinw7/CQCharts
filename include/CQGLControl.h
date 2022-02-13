#ifndef CQGLControl_H
#define CQGLControl_H

#include <QWidget>

class CQGLControlToolBar;
class CQToolButton;
class QGLWidget;
class QOpenGLWidget;

class CQGLControl : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool depthTest   READ getDepthTest   WRITE setDepthTest  )
  Q_PROPERTY(bool cullFace    READ getCullFace    WRITE setCullFace   )
  Q_PROPERTY(bool lighting    READ getLighting    WRITE setLighting   )
  Q_PROPERTY(bool outline     READ getOutline     WRITE setOutline    )
  Q_PROPERTY(bool frontFace   READ getFrontFace   WRITE setFrontFace  )
  Q_PROPERTY(bool smoothShade READ getSmoothShade WRITE setSmoothShade)

 public:
  struct Point {
    Point(double x=0.0, double y=0.0, double z=0.0) :
     x(x), y(y), z(z) {
    }

    double x { 0.0 };
    double y { 0.0 };
    double z { 0.0 };
  };

  using Matrix = double [16];

 public:
  CQGLControl(QGLWidget     *w);
  CQGLControl(QOpenGLWidget *w);

  void setSize(double size=2.0);
  void setCenter(double x=0.0, double y=0.0);

  void handleResize(int w, int h);

  void handleMousePress  (QMouseEvent *e);
  void handleMouseRelease(QMouseEvent *);
  bool handleMouseMotion (QMouseEvent *e);

  CQGLControlToolBar *createToolBar();

  void init();

  void updateGL();

  bool getDepthTest() const { return depth_test_; }
  void setDepthTest(bool b) { depth_test_ = b; }

  bool getCullFace() const { return cull_face_; }
  void setCullFace(bool b) { cull_face_ = b; }

  bool getLighting() const { return lighting_; }
  void setLighting(bool b) { lighting_ = b; }

  bool getOutline() const { return outline_; }
  void setOutline(bool b) { outline_ = b; }

  bool getFrontFace() const { return front_face_; }
  void setFrontFace(bool b) { front_face_ = b; }

  bool getSmoothShade() const { return smooth_shade_; }
  void setSmoothShade(bool b) { smooth_shade_ = b; }

  const double *pmatrix() const { return &pmatrix_[0]; }
  const double *matrix () const { return &matrix_ [0]; }
  const double *imatrix() const { return &imatrix_[0]; }

  void setMatrix(const Matrix &matrix);

  void getMousePos(int x, int y, double *px, double *py, double *pz);

  void getCameraPos(double *px, double *py, double *pz);

 private:
  void getMatrix();

 signals:
  void stateChanged();

 private slots:
  void depthSlot  (bool);
  void cullSlot   (bool);
  void lightSlot  (bool);
  void outlineSlot(bool);
  void frontSlot  (bool);
  void smoothSlot (bool);

 private:
  QGLWidget*          glW_     { nullptr };
  QOpenGLWidget*      openglW_ { nullptr };
  CQGLControlToolBar* toolbar_ { nullptr };

  bool depth_test_   { false };
  bool cull_face_    { false };
  bool lighting_     { false };
  bool outline_      { false };
  bool front_face_   { false };
  bool smooth_shade_ { false };

  double left_         { 0.0 };
  double right_        { 0.0 };
  double bottom_       { 1.0 };
  double top_          { 1.0 };
  double size_         { 2.0 };
  double center_x_     { 0.0 };
  double center_y_     { 0.0 };
  double znear_        { -10.0 };
  double zfar_         {  10.0 };
  double fov_          { 45.0 };
  int    mouse_x_      { 0 };
  int    mouse_y_      { 0 };
  bool   mouse_left_   { false };
  double mouse_middle_ { false };
  double mouse_right_  { false };
  double drag_pos_x_   { 0.0 };
  double drag_pos_y_   { 0.0 };
  double drag_pos_z_   { 0.0 };
  Matrix pmatrix_;
  Matrix matrix_;
  Matrix imatrix_;
  Point  refPoint_     { 0.0, 0.0, 0.0 }; /* Configurable center point for zooming and rotation */
};

class CQGLControlToolBar : public QWidget {
  Q_OBJECT

 public:
  CQGLControlToolBar(CQGLControl *control);

  CQGLControl *control() const { return control_; }

 signals:
  void stateChanged();

 private:
  CQGLControl  *control_;
  CQToolButton *depthButton_;
  CQToolButton *cullButton_;
  CQToolButton *lightButton_;
  CQToolButton *outlineButton_;
  CQToolButton *frontButton_;
  CQToolButton *smoothButton_;
};

#endif
