#ifndef CQChartsViewGLWidget_H
#define CQChartsViewGLWidget_H

#include <QGLWidget>
#include <QPointer>

class CQChartsView;

class CQGLControl;
class CQGLControlToolBar;

class CQChartsViewGLWidget : public QGLWidget {
 public:
  using View = CQChartsView;

 public:
  CQChartsViewGLWidget(View *view);

  CQGLControl *control() const { return control_; }

  void initializeGL() override;

  void resizeGL(int width, int height) override;

  void paintGL() override;

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void keyPressEvent  (QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
  using ViewP = QPointer<View>;

  ViewP               view_;
  CQGLControl*        control_        { nullptr };
  CQGLControlToolBar* controlToolbar_ { nullptr };
};

#endif
