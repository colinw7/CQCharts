#ifndef CQChartsViewGLWidget_H
#define CQChartsViewGLWidget_H

#include <QGLWidget>

class CQChartsView;

class CQGLControl;
class CQGLControlToolBar;

class CQChartsViewGLWidget : public QGLWidget {
 public:
  CQChartsViewGLWidget(CQChartsView *view);

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
  CQChartsView*       view_           { nullptr };
  CQGLControl*        control_        { nullptr };
  CQGLControlToolBar* controlToolbar_ { nullptr };
};

#endif
