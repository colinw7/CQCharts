set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

# create empty plot
set plot [create_charts_plot -type empty]

#---

set viewAnnotationsControl \
  [qt_create_widget -type CQCharts:ViewAnnotationsControl -name viewAnnotationsControl]

set viewAnnotationsControlAnn \
  [create_charts_widget_annotation -plot $plot -id viewAnnotationsControl \
  -rectangle [list 1 1 49 99 V] -widget $viewAnnotationsControl]

set viewAnnotationsControl [get_charts_data -annotation $viewAnnotationsControl -name widget_path]

#---

set plotAnnotationsControl \
  [qt_create_widget -type CQCharts:PlotAnnotationsControl -name plotAnnotationsControl]

set plotAnnotationsControlAnn \
  [create_charts_widget_annotation -plot $plot -id plotAnnotationsControl \
  -rectangle [list 50 1 99 99 V] -widget $plotAnnotationsControl]

set plotAnnotationsControl [get_charts_data -annotation $plotAnnotationsControl -name widget_path]
