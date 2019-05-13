#include <CQChartsStyle.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQPropertyView.h>

#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsStyle, toString, fromString)

int CQChartsStyle::metaTypeId;

void
CQChartsStyle::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsStyle);

  CQPropertyViewMgrInst->setUserName("CQChartsStyle", "style");
}

QString
CQChartsStyle::
toString() const
{
  if (! pen_)
    return "";

  QString str;

  str += QString("stroke: %1;"      ).arg(pen_  ->color().name());
  str += " ";
  str += QString("stroke-width: %1;").arg(pen_  ->widthF());
  str += " ";
  str += QString("fill: %1;"        ).arg(brush_->color().name());
  str += " ";
  str += QString("fill-opacity: %1;").arg(brush_->color().alphaF());

  return str;
}

bool
CQChartsStyle::
fromString(const QString &str)
{
  bool valid = true;

  delete pen_;
  delete brush_;

  pen_   = new QPen  (Qt::NoPen);
  brush_ = new QBrush(Qt::NoBrush);

  //---

  CQStrParse parse(str);

  while (! parse.eof()) {
    parse.skipSpace();

    QString name;

    while (! parse.eof()) {
      if (parse.isSpace() || parse.isChar(':'))
        break;

      name += parse.getChar();
    }

    parse.skipSpace();

    if (parse.isChar(':')) {
      parse.skipChar();

      parse.skipSpace();
    }

    QString value;

    while (! parse.eof()) {
      if (parse.isSpace() || parse.isChar(';'))
        break;

      value += parse.getChar();
    }

    parse.skipSpace();

    if (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();
    }

    if      (name == "fill") {
      QColor c(value);

      brush_->setColor(c);
      brush_->setStyle(Qt::SolidPattern);
    }
    else if (name == "fill-opacity") {
      bool ok;
      double a = CQChartsVariant::toReal(value, ok);
      if (! ok) continue;

      QColor c = brush_->color();
      c.setAlphaF(a);
      brush_->setColor(c);
    }
    else if (name == "stroke") {
      QColor c(value);

      pen_->setColor(c);
    }
    else if (name == "stroke-width") {
      bool ok;
      double w = CQChartsVariant::toReal(value, ok);
      if (! ok) continue;

      pen_->setWidthF(w);
    }
    else {
      valid = false;

#if 0
      std::cerr << "Unsupported style: " <<
                   name.toStdString() << "=" << value.toStdString() << "\n";
#endif
    }
  }

  return valid;
}
