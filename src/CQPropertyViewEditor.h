#ifndef CQPropertyViewEditor_H
#define CQPropertyViewEditor_H

#include <QString>
#include <QVariant>
#include <map>

class QWidget;
class QObject;

//------

class CQPropertyViewEditorFactory {
 public:
  CQPropertyViewEditorFactory() { }

  virtual ~CQPropertyViewEditorFactory() { }

  virtual QWidget *createEdit(QWidget *parent=0) = 0;

  virtual void connect(QWidget *w, QObject *obj, const char *method) = 0;

  virtual QVariant getValue(QWidget *w) = 0;

  virtual void setValue(QWidget *w, const QVariant &var) = 0;
};

//------

class CQPropertyViewIntegerEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewIntegerEditor(int min=INT_MIN, int max=INT_MAX, int step=1);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  int min_, max_;
  int step_;
};

//------

class CQPropertyViewRealEditor : public CQPropertyViewEditorFactory {
 public:
  enum class Type {
    DoubleSpin,
    RealSlider,
    ComboSlider
  };

 public:
  CQPropertyViewRealEditor(double min=-1E50, double max=1E50, double step=1.0);

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

class CQPropertyViewPointEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewPointEditor(double min=-1E50, double max=1E50, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double min_, max_;
  double step_;
};

//------

class CQPropertyViewSizeFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewSizeFEditor(double max=1000, double step=1.0);

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double max_;
  double step_;
};

//------

class CQPropertyViewRectFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewRectFEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

class CQPropertyViewColorEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewColorEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

class CQPropertyViewFontEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewFontEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#if 0
class CQPropertyViewPaletteEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewPaletteEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#if 0
class CQPropertyViewLineDashEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewLineDashEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#if 0
class CQPropertyViewAngleEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewAngleEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};
#endif

//------

#define CQPropertyViewEditorMgrInst CQPropertyViewEditorMgr::instance()

class CQPropertyViewEditorMgr {
 public:
  static CQPropertyViewEditorMgr *instance();

  CQPropertyViewEditorMgr();

  void setEditor(const QString &typeName, CQPropertyViewEditorFactory *editor);

  CQPropertyViewEditorFactory *getEditor(const QString &typeName) const;

 private:
  typedef std::map<QString, CQPropertyViewEditorFactory *> Editors;

  Editors editors_;
};

//------

class CQPropertyViewAlignEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewAlignEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
