#include <CQChartsObjRefPos.h>
#include <CQTclUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsObjRefPos, toString, fromString)

int CQChartsObjRefPos::metaTypeId;

void
CQChartsObjRefPos::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsObjRefPos);

  CQPropertyViewMgrInst->setUserName("CQChartsObjRefPos", "objRefPos");
}

QString
CQChartsObjRefPos::
toString() const
{
  QString str;

  if (objRef_.isValid()) {
    str += "@(";

    auto strs = objRef_.toStrings();

    str += strs[0] + " " + strs[1];

    str += ")";
  }

  if (position_.isValid()) {
    if (str != "")
      str += " ";

    str += position_.toString();
  }

  return str;
}

bool
CQChartsObjRefPos::
decodeString(const QString &str, ObjRef &objRef, Position &position, const Units &defUnits)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isString("@(")) {
    parse.skipChars(2);

    auto pos = parse.getPos();

    while (! parse.isChar(')'))
      parse.skipChar();

    auto str1 = parse.getAt(pos, parse.getPos() - pos);

    parse.skipChar();

    CQStrParse parse1(str1);

    parse1.skipSpace();

    QStringList objStrs;

    QString word1, word2;

    if (parse1.readNonSpace(word1))
      objStrs << word1;

    parse1.skipSpace();

    if (parse1.readNonSpace(word2))
      objStrs << word2;

    if (! objRef.fromStrings(objStrs))
      return false;

    parse.skipSpace();
  }

  auto rhs = parse.getAt();

  if (! position.fromString(rhs, defUnits))
    return false;

  return true;
}
