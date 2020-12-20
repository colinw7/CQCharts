#ifndef CQModelNameValues_H
#define CQModelNameValues_H

#include <QVariant>
#include <QFont>

/*!
 * \brief Name Values class
 */
class CQModelNameValues {
 public:
  using NameValues = std::map<QString,QVariant>;

 public:
  CQModelNameValues() { }

  CQModelNameValues(const QString &str) { (void) fromString(str); }

  virtual ~CQModelNameValues() { }

  const NameValues &nameValues() const { return nameValues_; }
  void setNameValues(const NameValues &v) { nameValues_ = v; }

  bool empty() const { return nameValues_.empty(); }

  QString toString() const { return toString(nameValues_); }

  static QString toString(const NameValues &nameValues);

  bool fromString(const QString &str);

  static bool fromString(const QString &str, NameValues &nameValues);

  bool nameValue(const QString &name, QVariant &value) const;
  void setNameValue(const QString &name, const QVariant &value);

  void removeName(const QString &name);

  template<typename T>
  void setNameValueType(const QString &name, const T &t) {
    setNameValue(name, t.toString());
  }

  //---

  // get named value. Returns true if name found, ok is false if found and value invalid
  bool nameValueString (const QString &name, QString &value, bool &ok) const;

  virtual bool nameValueInteger(const QString &name, int &value, bool &ok) const;
  virtual bool nameValueReal   (const QString &name, double &value, bool &ok) const;
  virtual bool nameValueBool   (const QString &name, bool &value, bool &ok) const;
  virtual bool nameValueColor  (const QString &name, QColor &color, bool &ok) const;
  virtual bool nameValueFont   (const QString &name, QFont &font, bool &ok) const;
  virtual bool nameValueAlign  (const QString &name, Qt::Alignment &align, bool &ok) const;

  template<typename T>
  bool nameValueType(const QString &name, T &t, bool &ok) const {
    QVariant var;

    if (! nameValue(name, var))
      return false;

    //t = T(var.toString());
    //ok = t.isValid();

    ok = t.fromString(var.toString());

    return true;
  }

 private:
  static QStringList splitNameValues(const QString &str);

 private:
  NameValues nameValues_;
};

#endif
