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
    str += "@";

    auto strs = objRef_.toStrings();

    str += strs[0] + " " + strs[1];
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
fromString(const QString &str)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('@')) {
    parse.skipChar();

    QString word1, word2;

    if (! parse.readNonSpace(word1))
      return false;

    parse.skipSpace();

    if (! parse.readNonSpace(word2))
      return false;

    auto strs = QStringList() << word1 << word2;

    if (! objRef_.fromStrings(strs))
      return false;

    parse.skipSpace();
  }

  auto rhs = parse.getAt();

  if (! position_.fromString(rhs))
    return false;

  return true;
}
