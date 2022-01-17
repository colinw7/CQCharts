#include <CQBuchHeim.h>
#include <CBuchHeim.h>
#include <CFileParse.h>

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>

#include <iostream>
#include <memory>

static int s_drawRadius { 30 };

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto f = app.font();

  f.setPointSizeF(20);

  app.setFont(f);

  auto buckheim = std::make_unique<CQBuchHeim>();

  buckheim->show();

  return app.exec();
}

CQBuchHeim::
CQBuchHeim(QWidget *parent) :
 QFrame(parent)
{
  readTrees("demo_trees.txt");

#if 0
  for (auto *tree : trees_)
    tree->print(std::cerr);
#endif

  n_ = 8;

  place(n_);
}

void
CQBuchHeim::
place(int n)
{
  if (n < 0 || n >= int(trees_.size()))
    return;

  auto *tree = trees_[n].get();

  tree->print(std::cerr);

  dt_ = std::make_unique<CBuchHeim::DrawTree>(tree);

  //dt_->knuthPlace();
  //dt_->wsPlace();
  //dt_->wsPlaceCentered();

  dt_->place();
}

CQBuchHeim::
~CQBuchHeim()
{
}

void
CQBuchHeim::
readTrees(const QString &filename)
{
  CFileParse parse(filename.toStdString());

  parse.setStream(false);

  parse.loadLine();

  CBuchHeim::Tree *parent = nullptr;

  std::vector<int> depths;

  depths.push_back(0);

  while (! parse.eof()) {
    int depth = 0;

    while (parse.isSpace()) {
      parse.skipChar();

      ++depth;
    }

    if (parse.lookChar() == '#') {
      parse.loadLine();
      continue;
    }

    std::string name;

    while (! parse.eol())
      name += parse.readChar();

    if (name == "") {
      parse.loadLine();
      continue;
    }

    auto *child = new CBuchHeim::Tree(name);

    if (depth == 0) {
      if (parent) {
        while (parent->parent())
          parent = parent->parent();

        trees_.push_back(TreeP(parent));
      }

      parent = child;

      depths.clear();
    }
    else {
      int parentDepth = depths.back();

      while (depth <= parentDepth) {
        assert(parent);

        parent = parent->parent();

        depths.pop_back();

        parentDepth = (! depths.empty() ? depths.back() : 0);
      }

      parent->addChild(TreeP(child));

      parent = child;
    }

    depths.push_back(depth);

    parse.loadLine();
  }

  if (parent) {
    while (parent->parent())
      parent = parent->parent();

    trees_.push_back(TreeP(parent));
  }
}

void
CQBuchHeim::
mousePressEvent(QMouseEvent *e)
{
  QPointF p;

  displayRange_.pixelToWindow(e->pos(), p);

  auto *itree = insideTree(dt_.get(), p);
  if (! itree) return;

  if      (mode_ == Mode::SELECT)
    std::cerr << itree->hierName() << "\n";
  else if (mode_ == Mode::ADD) {
    char c[2];

    c[0] = 'A' + itree->numChildren();
    c[1] = '\0';

    auto name = std::string(c);

    itree->tree()->addChild(CBuchHeim::TreeP(new CBuchHeim::Tree(name)));

    place(n_);

    update();
  }
}

void
CQBuchHeim::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_A) {
    mode_ = Mode::ADD;
  }
  else if (e->key() == Qt::Key_F) {
    dt_->fixOverlaps();

    update();
  }
  else if (e->key() == Qt::Key_O) {
    dt_->checkOverlaps();
  }
  else if (e->key() == Qt::Key_Escape) {
    mode_ = Mode::SELECT;
  }
  else if (e->key() == Qt::Key_Up) {
    if (n_ < int(trees_.size()) - 1) {
      ++n_;

      place(n_);

      update();
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if (n_ > 0) {
      --n_;

      place(n_);

      update();
    }
  }
}

void
CQBuchHeim::
resizeEvent(QResizeEvent *)
{
}

void
CQBuchHeim::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter_ = &painter;

  displayRange_.setPixelRange(0, height() - 1, width() - 1, 0);

  xmin_ = 9999.0; ymin_ = 9999.0; xmax_ = -9999.0; ymax_ = -9999.0;

  sizeTree(dt_.get(), 0);

  displayRange_.setWindowRange(xmin_, ymin_, xmax_, ymax_);

  drawConn(dt_.get(), 0);

  drawTree(dt_.get(), 0);
}

void
CQBuchHeim::
sizeTree(CBuchHeim::DrawTree *root, int depth)
{
  double x = mapX(root->x());
  double y = mapY(depth);

  double x1 = x - s_drawRadius*1.5;
  double y1 = y - s_drawRadius*1.5;
  double x2 = x + s_drawRadius*1.5;
  double y2 = y + s_drawRadius*1.5;

  xmin_ = std::min(xmin_, x1);
  ymin_ = std::min(ymin_, y1);
  xmax_ = std::max(xmax_, x2);
  ymax_ = std::max(ymax_, y2);

  for (const auto &child : root->children())
    sizeTree(child.get(), depth + 1);
}

void
CQBuchHeim::
drawTree(CBuchHeim::DrawTree *root, int depth)
{
  auto prect = treePixelRect(root);

  painter_->setPen(Qt::black);

  painter_->drawEllipse(prect);

  QFontMetricsF fm(font());

  double xm = prect.center().x();
  double ym = prect.center().y();

  auto text = QString::fromStdString(root->name());

  painter_->setPen(Qt::black);

  painter_->drawText(xm - fm.width(text)/2.0, ym + (fm.ascent() - fm.descent())/2.0, text);

  for (const auto &child : root->children())
    drawTree(child.get(), depth + 1);
}

void
CQBuchHeim::
drawConn(CBuchHeim::DrawTree *parent, int depth)
{
  if (parent->thread()) {
    painter_->setPen(Qt::blue);

    drawLine(parent, parent->thread());
  }

#if 0
  if (parent->ancestor() && parent->ancestor() != parent) {
    painter_->setPen(Qt::blue);

    drawLine(parent, parent->ancestor());
  }
#endif

  for (const auto &child : parent->children()) {
    painter_->setPen(Qt::black);

    drawLine(parent, child.get());

    drawConn(child.get(), depth + 1);
  }
}

void
CQBuchHeim::
drawLine(CBuchHeim::DrawTree *tree, CBuchHeim::DrawTree *child)
{
  auto prect1 = treeRect(tree );
  auto prect2 = treeRect(child);

  double x1 = prect1.center().x();
  double y1 = prect1.center().y();
  double x2 = prect2.center().x();
  double y2 = prect2.center().y();

  //double x1 = mapX(root ->x());
  //double y1 = mapY(depth     );
  //double x2 = mapX(child->x());
  //double y2 = mapY(depth + 1 );

  double a = atan2(y2 - y1, x2 - x1);

  x1 += s_drawRadius*cos(a);
  y1 += s_drawRadius*sin(a);

  x2 -= s_drawRadius*cos(a);
  y2 -= s_drawRadius*sin(a);

  QPointF p1, p2;

  displayRange_.windowToPixel(QPointF(x1, y1), p1);
  displayRange_.windowToPixel(QPointF(x2, y2), p2);

  painter_->drawLine(p1.x(), p1.y(), p2.x(), p2.y());
}

CBuchHeim::DrawTree *
CQBuchHeim::
insideTree(CBuchHeim::DrawTree *tree, const QPointF &p) const
{
  auto rect = treeRect(tree);

  if (rect.contains(p))
    return tree;

  for (const auto &child : tree->children()) {
    auto *itree = insideTree(child.get(), p);

    if (itree)
      return itree;
  }

  return nullptr;
}

QRectF
CQBuchHeim::
treeRect(CBuchHeim::DrawTree *tree) const
{
  double x = mapX(tree->x());
  double y = mapY(tree->y());

  return QRectF(x - s_drawRadius, y - s_drawRadius, 2*s_drawRadius, 2*s_drawRadius);
}

QRectF
CQBuchHeim::
treePixelRect(CBuchHeim::DrawTree *tree) const
{
  double x = mapX(tree->x());
  double y = mapY(tree->y());

  QPointF p1, p2;

  displayRange_.windowToPixel(QPointF(x - s_drawRadius, y - s_drawRadius), p1);
  displayRange_.windowToPixel(QPointF(x + s_drawRadius, y + s_drawRadius), p2);

  return QRectF(p1.x(), p1.y(), p2.x() - p1.x(), p2.y() - p1.y());
}

double
CQBuchHeim::
mapX(int x) const
{
  return x*s_drawRadius*2.5;
}

double
CQBuchHeim::
mapY(int depth) const
{
  return depth*s_drawRadius*2.5;
}

QSize
CQBuchHeim::
sizeHint() const
{
  return QSize(800, 800);
}
