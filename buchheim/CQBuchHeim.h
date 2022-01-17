#ifndef CQBuchHeim_H
#define CQBuchHeim_H

#include <QFrame>
#include <CQDisplayRange2D.h>
#include <memory>

namespace CBuchHeim {
class Tree;
class DrawTree;
}

class CQBuchHeim : public QFrame {
  Q_OBJECT

 public:
  enum Mode {
    SELECT,
    ADD
  };

 public:
  CQBuchHeim(QWidget *parent=nullptr);

 ~CQBuchHeim();

  void mousePressEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *) override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  void readTrees(const QString &filename);

  void place(int n);

  void sizeTree(CBuchHeim::DrawTree *tree, int depth);
  void drawTree(CBuchHeim::DrawTree *tree, int depth);
  void drawConn(CBuchHeim::DrawTree *tree, int depth);
  void drawLine(CBuchHeim::DrawTree *tree, CBuchHeim::DrawTree *child);

  CBuchHeim::DrawTree *insideTree(CBuchHeim::DrawTree *tree, const QPointF &p) const;

  QRectF treeRect(CBuchHeim::DrawTree *tree) const;
  QRectF treePixelRect(CBuchHeim::DrawTree *tree) const;

  double mapX(int x) const;
  double mapY(int depth) const;

 private:
  using TreeP     = std::unique_ptr<CBuchHeim::Tree>;
  using Trees     = std::vector<TreeP>;
  using DrawTreeP = std::unique_ptr<CBuchHeim::DrawTree>;

  Trees            trees_;
  DrawTreeP        dt_      { nullptr };
  QPainter*        painter_ { nullptr };
  double           xmin_    { 0.0 };
  double           ymin_    { 0.0 };
  double           xmax_    { 0.0 };
  double           ymax_    { 0.0 };
  int              n_       { 0 };
  Mode             mode_    { Mode::SELECT };
  CQDisplayRange2D displayRange_;
};

#endif
