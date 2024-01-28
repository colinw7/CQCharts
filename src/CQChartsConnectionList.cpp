#include <CQChartsConnectionList.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsConnectionList, toString, fromString)

int CQChartsConnectionList::metaTypeId;

void
CQChartsConnectionList::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsConnectionList);

  CQPropertyViewMgrInst->setUserName("CQChartsConnectionList", "connection_list");
}

bool
CQChartsConnectionList::
setValue(const QString &str)
{
  return stringToConnections(str, connections_);
}

QString
CQChartsConnectionList::
toString() const
{
  return connectionsToString(connections_);
}

bool
CQChartsConnectionList::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsConnectionList();
    return true;
  }

  return setValue(s);
}

bool
CQChartsConnectionList::
stringToConnections(const QString &str, Connections &connections)
{
  // connections are { <connection> <connection> ... }
  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    Connection connection;

    if (! decodeConnection(str1, connection))
      return false;

    connections.push_back(connection);

    parse.skipSpace();
  }

  if (parse.isChar('}'))
    parse.skipChar();

  return true;
}

bool
CQChartsConnectionList::
decodeConnection(const QString &str, Connection &connection)
{
  // connection is { <node> <value> }
  CQStrParse parse(str);

  parse.skipSpace();

  QString str1;

  if (! parse.readNonSpace(str1))
    return false;

  parse.skipSpace();

  QString str2;

  if (! parse.readNonSpace(str2))
    return false;

  long node;

  if (! CQChartsUtil::toInt(str1, node))
    return false;

  double value;

  if (! CQChartsUtil::toReal(str2, value))
    return false;

  connection = Connection(int(node), value);

  return true;
}

QString
CQChartsConnectionList::
connectionsToString(const Connections &connections)
{
  QStringList strs;

  for (const auto &connection : connections) {
    strs.push_back(QString("{%1 %2}").arg(connection.node).arg(connection.value));
  }

  return "{" + strs.join(" ") + "}";
}
