#ifndef CQChartsConnectionList_H
#define CQChartsConnectionList_H

#include <CQChartsUtil.h>

/*!
 * \brief connection list
 * \ingroup Charts
 */
class CQChartsConnectionList {
 public:
  struct Connection {
    int node  { 0 };
    int count { 0 };

    Connection(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  using Connections = std::vector<Connection>;

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsConnectionList(const QString &s=QString()) {
   setValue(s);
  }

  CQChartsConnectionList(const CQChartsConnectionList &rhs) :
   connections_(rhs.connections_) {
  }

  CQChartsConnectionList &operator=(const CQChartsConnectionList &rhs) {
    connections_ = rhs.connections_;

    return *this;
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

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsConnectionList)

#endif
