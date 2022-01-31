#ifndef CQChartsConnectionList_H
#define CQChartsConnectionList_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

/*!
 * \brief connection list
 * \ingroup Charts
 */
class CQChartsConnectionList {
 public:
  //! connecton node and value
  struct Connection {
    int    node  { -1 };
    double value { -1 };

    Connection() = default;

    Connection(int node, double value) :
     node(node), value(value) {
    }
  };

  using Connections = std::vector<Connection>;

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsConnectionList, metaTypeId)

 public:
  CQChartsConnectionList(const QString &s=QString()) {
   setValue(s);
  }

  const Connections &connections() const { return connections_; }

  bool setValue(const QString &str) {
    return stringToConnections(str, connections_);
  }

  //---

  QString toString() const {
    return connectionsToString(connections_);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  static bool stringToConnections(const QString &str, Connections &connections);
  static QString connectionsToString(const Connections &connections);

 private:
  static bool decodeConnection(const QString &str, Connection &connection);

 private:
  Connections connections_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsConnectionList)

#endif
