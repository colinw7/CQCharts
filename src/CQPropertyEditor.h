#ifndef CQPropertyEditor_H
#define CQPropertyEditor_H

#include <QString>
#include <QVariant>
#include <map>

class QWidget;
class QObject;

//------

class CQPropertyEditorFactory {
 public:
  CQPropertyEditorFactory() { }

  virtual ~CQPropertyEditorFactory() { }

  virtual QWidget *createEdit(QWidget *parent=0) = 0;

  virtual void connect(QWidget *w, QObject *obj, const char *method) = 0;

  virtual QVariant getValue(QWidget *w) = 0;

  virtual void setValue(QWidget *w, const QVariant &var) = 0;
};

//------

class CQPropertyIntegerEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyIntegerEditor(int min=INT_MIN, int max=INT_MAX, int step=1);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  int min_, max_;
  int step_;
};

//------

class CQPropertyRealEditor : public CQPropertyEditorFactory {
 public:
  enum class Type {
    DoubleSpin,
    RealSlider,
    ComboSlider
  };

 public:
  CQPropertyRealEditor(double min=-1E50, double max=1E50, double step=1.0);

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  Type   type_;
  double min_, max_;
  double step_;
};

//------

class CQPropertyPointEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyPointEditor(double min=-1E50, double max=1E50, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double min_, max_;
  double step_;
};

//------

class CQPropertySizeFEditor : public CQPropertyEditorFactory {
 public:
  CQPropertySizeFEditor(double max=1000, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double max_;
  double step_;
};

//------

class CQPropertyRectFEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyRectFEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

class CQPropertyColorEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyColorEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

class CQPropertyFontEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyFontEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#if 0
class CQPropertyPaletteEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyPaletteEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#if 0
class CQPropertyLineDashEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyLineDashEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#if 0
class CQPropertyAngleEditor : public CQPropertyEditorFactory {
 public:
  CQPropertyAngleEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#define CQPropertyEditorMgrInst CQPropertyEditorMgr::instance()

class CQPropertyEditorMgr {
 public:
  static CQPropertyEditorMgr *instance();

  CQPropertyEditorMgr();

  void setEditor(const QString &typeName, CQPropertyEditorFactory *editor);

  CQPropertyEditorFactory *getEditor(const QString &typeName) const;

 private:
  typedef std::map<QString, CQPropertyEditorFactory *> Editors;

  Editors editors_;
};

#endif
