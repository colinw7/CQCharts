#ifndef CQChartsModuleData_H
#define CQChartsModuleData_H

class QPainter;

struct CQChartsModuleDrawData {
  int       id;
  QPainter *painter;
  double    pixel_xmin;
  double    pixel_ymin;
  double    pixel_xmax;
  double    pixel_ymax;
  double    plot_xmin;
  double    plot_ymin;
  double    plot_xmax;
  double    plot_ymax;
};

enum CQChartsModuleEventMode {
  CQCHARTS_EVENT_MODE_NONE = 0,

  CQCHARTS_EVENT_MODE_SELECT = 1,
  CQCHARTS_EVENT_MODE_PROBE  = 2,
  CQCHARTS_EVENT_MODE_QUERY  = 3
};

enum CQChartsModuleEventType {
  CQCHARTS_EVENT_TYPE_NONE = 0,

  CQCHARTS_EVENT_TYPE_MOUSE_MASK = (1<<16),
  CQCHARTS_EVENT_TYPE_KEY_MASK   = (1<<17),

  CQCHARTS_EVENT_TYPE_MOUSE_PRESS   = (CQCHARTS_EVENT_TYPE_MOUSE_MASK + 1),
  CQCHARTS_EVENT_TYPE_MOUSE_MOVE    = (CQCHARTS_EVENT_TYPE_MOUSE_MASK + 2),
  CQCHARTS_EVENT_TYPE_MOUSE_RELEASE = (CQCHARTS_EVENT_TYPE_MOUSE_MASK + 3),

  CQCHARTS_EVENT_TYPE_KEY_PRESS   = (CQCHARTS_EVENT_TYPE_KEY_MASK + 1),
  CQCHARTS_EVENT_TYPE_KEY_RELEASE = (CQCHARTS_EVENT_TYPE_KEY_MASK + 2)
};

struct CQChartsModuleEventData {
  int    id;
  int    mode;
  int    type;
  int    button;
  int    key;
  double pixel_x;
  double pixel_y;
  double plot_x;
  double plot_y;
};

struct CQChartsModuleGetData {
  int   id;
  char *buffer;
};

struct CQChartsModuleSetData {
  int   id;
  char *buffer;
};

#endif
