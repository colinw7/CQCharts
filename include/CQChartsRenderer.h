#ifndef CQChartsRenderer_H
#define CQChartsRenderer_H

#include <QPainter>

class CQChartsRenderer {
 public:
  CQChartsRenderer() { }

  virtual void setAntiAlias() = 0;

  virtual const QPen &pen() const = 0;
  virtual void setPen(const QPen &pen) = 0;

  virtual const QBrush &brush() const = 0;
  virtual void setBrush(const QBrush &brush) = 0;

  virtual const QFont &font() const = 0;
  virtual void setFont (const QFont &f) = 0;

  virtual void fillRect(const QRectF &rect, const QBrush &brush) = 0;
  virtual void drawRect(const QRectF &rect) = 0;

  virtual void drawRoundedRect(const QRectF &rect, double xc, double yc) = 0;

  virtual void drawLine(const QPointF &p1, const QPointF &p2) = 0;

  virtual void drawPolygon(const QPolygonF &p) = 0;

  virtual void drawText(const QPointF &p, const QString &text) = 0;

  virtual void drawEllipse(const QRectF &rect) = 0;

  virtual void drawArc(const QRectF &rect, double a, double da) = 0;

  virtual void strokePath(const QPainterPath &path, const QPen   &pen  ) = 0;
  virtual void fillPath  (const QPainterPath &path, const QBrush &brush) = 0;

  virtual void drawPath(const QPainterPath &path) = 0;

  virtual void setTransform(const QTransform &t) = 0;

  virtual void save   () = 0;
  virtual void restore() = 0;

  virtual void setClipRect(const QRectF &rect) = 0;
};

class CQChartsPainter : public CQChartsRenderer {
 public:
  CQChartsPainter(QPainter *painter) :
   painter_(painter) {
  }

  void setAntiAlias() override {
    painter_->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  }

  const QPen &pen() const override { return painter_->pen(); }

  void setPen(const QPen &pen) override {
    painter_->setPen(pen);
  }

  const QBrush &brush() const override { return painter_->brush(); }

  void setBrush(const QBrush &brush) override {
    painter_->setBrush(brush);
  }

  const QFont &font() const override { return painter_->font(); }

  void setFont(const QFont &font) override {
    painter_->setFont(font);
  }

  void fillRect(const QRectF &rect, const QBrush &brush) override {
    painter_->fillRect(rect, brush);
  }

  void drawRect(const QRectF &rect) override {
    painter_->drawRect(rect);
  }

  void drawRoundedRect(const QRectF &rect, double xc, double yc) override {
    painter_->drawRoundedRect(rect, xc, yc);
  }

  void drawLine(const QPointF &p1, const QPointF &p2) override {
    painter_->drawLine(p1, p2);
  }

  void drawPolygon(const QPolygonF &p) override {
    painter_->drawPolygon(p);
  }

  void drawText(const QPointF &p, const QString &text) override {
    painter_->drawText(p, text);
  }

  void drawEllipse(const QRectF &rect) override {
    painter_->drawEllipse(rect);
  }

  void drawArc(const QRectF &rect, double a, double da) override {
    painter_->drawArc(rect, a, da);
  }

  void strokePath(const QPainterPath &path, const QPen &pen) override {
    painter_->strokePath(path, pen);
  }

  void fillPath(const QPainterPath &path, const QBrush &brush) override {
    painter_->fillPath(path, brush);
  }

  void drawPath(const QPainterPath &path) override {
    painter_->drawPath(path);
  }

  void setTransform(const QTransform &t) override {
    painter_->setTransform(t);
  }

  void save() override {
    painter_->save();
  }

  void restore() override {
    painter_->restore();
  }

  void setClipRect(const QRectF &rect) override {
    painter_->setClipRect(rect, Qt::ReplaceClip);
  }

 private:
  QPainter *painter_ { nullptr };
};

#endif
