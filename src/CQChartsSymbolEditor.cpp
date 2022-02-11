#include <CQChartsSymbolEditor.h>
#include <CQChartsPlotSymbol.h>

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>

CQChartsSymbolEditor::
CQChartsSymbolEditor(CQChartsViewSettings *viewSettings) :
 viewSettings_(viewSettings)
{
  setObjectName("symbolEditor");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);
}

void
CQChartsSymbolEditor::
setSymbol(const CQChartsSymbol &symbol)
{
  symbol_ = symbol;

  pointsArray_.clear();

  Points *points = nullptr;

  if (CQChartsPlotSymbolMgr::isSymbol(symbol_)) {
    const auto &plotSymbol = CQChartsPlotSymbolMgr::getSymbol(symbol_);

    if (symbol_.isFilled()) {
      pointsArray_.emplace_back();

      points = &pointsArray_[pointsArray_.size() - 1];

      bool skip = false;

      for (auto &line : plotSymbol.fillLines) {
        if      (points->empty())
          points->emplace_back(line.x1, line.y1);
        else if (skip) {
          Point p(line.x1, line.y1);

          p.skip = true;

          points->push_back(p);

          skip = false;
        }

        points->emplace_back(line.x2, line.y2);

        skip = (line.connect == CQChartsPlotSymbol::Connect::BREAK);
      }
    }
    else {
      bool connect = false;

      for (auto &line : plotSymbol.lines) {
        if (! connect) {
          pointsArray_.emplace_back();

          points = &pointsArray_[pointsArray_.size() - 1];

          points->emplace_back(line.x1, line.y1);
        }

        points->emplace_back(line.x2, line.y2);

        if      (line.connect == CQChartsPlotSymbol::Connect::STROKE)
          connect = false;
        else if (line.connect == CQChartsPlotSymbol::Connect::CLOSE)
          connect = false;
        else
          connect = true;
      }
    }
  }

  update();
}

//---

void
CQChartsSymbolEditor::
resizeEvent(QResizeEvent *)
{
  range_.setPixelRange(0, height() - 1, width() - 1, 0);

  range_.setWindowRange(-1, -1, 1, 1);

  range_.setEqualScale(true);
}

void
CQChartsSymbolEditor::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  drawGrid(&painter);

  drawSymbol(&painter);

  drawGuides(&painter);
}

void
CQChartsSymbolEditor::
drawGrid(QPainter *painter)
{
  int n = 64;

  double d = 2.0/n;

  for (int ix = 0; ix <= n; ++ix) {
    QColor c(ix % 8 == 0 ? "#666666" : "#cccccc");

    painter->setPen(c);

    double x = ix*d - 1.0;

    double x1, y1, x2, y2;

    range_.windowToPixel(x, -1, &x1, &y1);
    range_.windowToPixel(x,  1, &x2, &y2);

    painter->drawLine(int(x1), int(y1), int(x2), int(y2));
  }

  for (int iy = 0; iy <= n; ++iy) {
    QColor c(iy % 8 == 0 ? "#666666" : "#cccccc");

    painter->setPen(c);

    double y = iy*d - 1.0;

    double x1, y1, x2, y2;

    range_.windowToPixel(-1, y, &x1, &y1);
    range_.windowToPixel( 1, y, &x2, &y2);

    painter->drawLine(int(x1), int(y1), int(x2), int(y2));
  }
}

void
CQChartsSymbolEditor::
drawSymbol(QPainter *painter)
{
  for (const auto &points : pointsArray_) {
    QPainterPath path;

    for (const auto &p : points) {
      double x, y;

      range_.windowToPixel(p.x(), p.y(), &x, &y);

      if (path.elementCount() == 0 || p.skip)
        path.moveTo(x, y);
      else
        path.lineTo(x, y);
    }

    path.closeSubpath();

    auto pen = QPen(Qt::black);

    if (! symbol_.isFilled())
      pen.setWidth(3);

    painter->setPen(pen);

    if (symbol_.isFilled())
      painter->setBrush(QColor("#55dd55")); // TODO: config
    else
      painter->setBrush(Qt::NoBrush);

    painter->drawPath(path);
  }

  //---

  if (mouseArrayInd_ >= 0 && mouseArrayInd_ < int(pointsArray_.size())) {
    const auto &points = pointsArray_[size_t(mouseArrayInd_)];

    if (mouseInd_ >= 0 && mouseInd_ < int(points.size())) {
      auto p = points[size_t(mouseInd_)];

      int r = 6;

      double x, y;

      range_.windowToPixel(p.x(), p.y(), &x, &y);

      painter->setPen(Qt::black);
      painter->setBrush(Qt::red);

      painter->drawEllipse(QRectF(x - r, y - r, 2*r, 2*r));
    }
  }
}

void
CQChartsSymbolEditor::
drawGuides(QPainter *painter)
{
  painter->setBrush(Qt::NoBrush);

  auto drawEllipse = [&](double xc, double yc, double r) {
    double tlx, tly, brx, bry;

    range_.windowToPixel(xc - r, yc - r, &tlx, &tly);
    range_.windowToPixel(xc + r, yc + r, &brx, &bry);

    painter->drawEllipse(QRectF(tlx, tly, brx - tlx, bry - tly));
  };

  painter->setPen(QColor("#666666")); // TODO: config

  drawEllipse(0.0, 0.0, 1.000);
  drawEllipse(0.0, 0.0, 0.875);

  auto drawRect = [&](double xc, double yc, double r) {
    double tlx, tly, brx, bry;

    range_.windowToPixel(xc - r, yc - r, &tlx, &tly);
    range_.windowToPixel(xc + r, yc + r, &brx, &bry);

    painter->drawRect(QRectF(tlx, tly, brx - tlx, bry - tly));
  };

  double d = 1.0/8.0;

  painter->setPen(QColor("#666666")); // TODO: config

  drawRect(0.0, 0.0, 1.000 - d);
}

void
CQChartsSymbolEditor::
mousePressEvent(QMouseEvent *me)
{
  auto pos = me->pos();

  updateMousePos(pos);

  pressed_ = true;
  button_  = me->button();
  escape_  = false;

  update();
}

void
CQChartsSymbolEditor::
mouseMoveEvent(QMouseEvent *me)
{
  if (escape_)
    return;

  auto pos = me->pos();

  updateMousePos(pos);

  double dx = std::abs(mousePos_.x() - pressPos_.x());
  double dy = std::abs(mousePos_.y() - pressPos_.y());

  if (pressed_) {
    double x, y;

    pixelToWindow(pos.x(), pos.y(), x, y);

    QPointF p(x, y);

    bool isShift = (me->modifiers() & Qt::ShiftModifier);

    if      (button_ == Qt::LeftButton) {
      if (isShift) {
        if (dx > dy)
          p.setY(pointPos_.y());
        else
          p.setX(pointPos_.x());
      }

      if (mouseArrayInd_ >= 0 && mouseArrayInd_ < int(pointsArray_.size())) {
        auto &points = pointsArray_[size_t(mouseArrayInd_)];

        if (mouseInd_ >= 0 && mouseInd_ < int(points.size()))
          points[size_t(mouseInd_)] = p;
      }
    }
  }

  update();
}

void
CQChartsSymbolEditor::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
  escape_  = false;
}

void
CQChartsSymbolEditor::
keyPressEvent(QKeyEvent *ke)
{
  if      (ke->key() == Qt::Key_Escape) {
    if (! escape_) {
      escape_ = true;

      if (mouseArrayInd_ >= 0 && mouseArrayInd_ < int(pointsArray_.size())) {
        auto &points = pointsArray_[size_t(mouseArrayInd_)];

        if (mouseInd_ >= 0 && mouseInd_ < int(points.size()))
          points[size_t(mouseInd_)] = pointPos_;
      }

      update();
    }
  }
  else if (ke->key() == Qt::Key_P) {
    if (! pointsArray_.empty()) {
      if (symbol_.isFilled()) {
        auto *points = &pointsArray_[0];

        auto np = points->size();

        CQChartsPlotSymbol::Lines lines;

        for (size_t i = 0; i < np; ++i) {
          size_t i1 = (i < np - 1 ? i + 1 : 0);

          auto connectStr = (i < np - 1 ? "PlotSymbol::Connect::STROKE" :
                                          "PlotSymbol::Connect::FILL");

          std::cout << "{" << (*points)[i ].x() << ", " << (*points)[i ].y() << ", " <<
                              (*points)[i1].x() << ", " << (*points)[i1].y() << ", " <<
                              connectStr << "},\n";
        }
      }
      else {
        for (const auto &points : pointsArray_) {
          auto np = points.size();

          if (np == 2) {
            std::cout << "{" << points[0].x() << ", " << points[0].y() << ", " <<
                                points[1].x() << ", " << points[1].y() << ", " <<
                                "PlotSymbol::Connect::STROKE},\n";
          }
          else {
            for (size_t i = 0; i < np; ++i) {
              size_t i1 = (i < np - 1 ? i + 1 : 0);

              auto connectStr = (i < np - 1 ? "PlotSymbol::Connect::LINE" :
                                              "PlotSymbol::Connect::CLOSE");

              std::cout << "{" << points[i ].x() << ", " << points[i ].y() << ", " <<
                                  points[i1].x() << ", " << points[i1].y() << ", " <<
                                  connectStr << "},\n";
            }
          }
        }
      }
    }
  }
  else if (ke->key() == Qt::Key_A) {
    if (! pointsArray_.empty()) {
      if (symbol_.isFilled()) {
        auto *points = &pointsArray_[0];

        auto np = points->size();

        CQChartsPlotSymbol::Lines lines;

        for (size_t i = 0; i < np; ++i) {
          size_t i1 = (i < np - 1 ? i + 1 : 0);

          auto connect = (i < np - 1 ? CQChartsPlotSymbol::Connect::STROKE :
                                       CQChartsPlotSymbol::Connect::FILL);

          lines.emplace_back((*points)[i ].x(), (*points)[i ].y(),
                             (*points)[i1].x(), (*points)[i1].y(), connect);
        }

        CQChartsPlotSymbolMgr::setSymbolFillLines(symbol_, lines);
      }
      else {
        CQChartsPlotSymbol::Lines lines;

        for (const auto &points : pointsArray_) {
          auto np = points.size();

          if (np == 2) {
            lines.emplace_back(points[0].x(), points[0].y(),
                               points[1].x(), points[1].y(),
                               CQChartsPlotSymbol::Connect::STROKE);
          }
          else {
            for (size_t i = 0; i < np; ++i) {
              size_t i1 = (i < np - 1 ? i + 1 : 0);

              auto connect = (i < np - 1 ? CQChartsPlotSymbol::Connect::LINE :
                                           CQChartsPlotSymbol::Connect::CLOSE);

              lines.emplace_back(points[i ].x(), points[i ].y(),
                                 points[i1].x(), points[i1].y(), connect);
            }
          }
        }

        CQChartsPlotSymbolMgr::setSymbolLines(symbol_, lines);
      }
    }
  }
}

void
CQChartsSymbolEditor::
updateMousePos(const QPoint &pos)
{
  double x, y;

  pixelToWindow(pos.x(), pos.y(), x, y);

  mousePos_ = QPointF(x, y);

  if (! pressed_) {
    double d        = 9999;
    int    arrayInd = 0;

    for (const auto &points : pointsArray_) {
      int ind = 0;

      for (const auto &p : points) {
        double d1 = std::hypot(p.x() - x, p.y() - y);

        if (d1 < d) {
          mouseArrayInd_ = arrayInd;
          mouseInd_      = ind;
          d              = d1;
        }

        ++ind;
      }

      ++arrayInd;
    }

    pressPos_ = mousePos_;

    if (mouseArrayInd_ >= 0 && mouseArrayInd_ < int(pointsArray_.size())) {
      const auto &points = pointsArray_[size_t(mouseArrayInd_)];

      if (mouseInd_ >= 0 && mouseInd_ < int(points.size()))
        pointPos_ = points[size_t(mouseInd_)].p;
    }
  }
}

void
CQChartsSymbolEditor::
pixelToWindow(double px, double py, double &wx, double &wy)
{
  range_.pixelToWindow(px, py, &wx, &wy);

  wx = int(wx*64.0)/64.0;
  wy = int(wy*64.0)/64.0;

  wx = std::min(std::max(wx, -1.0), 1.0);
  wy = std::min(std::max(wy, -1.0), 1.0);
}

QSize
CQChartsSymbolEditor::
sizeHint() const
{
  return QSize(1000, 1000);
}
