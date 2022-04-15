set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

# create empty plot
set plot [create_charts_plot -type empty]

#---

set annotationsControl \
  [qt_create_widget -type CQChartsAnnotationsControl -name annotationsControl]

set annotationsControlAnn \
  [create_charts_widget_annotation -plot $plot -id annotationsControl \
  -rectangle [list 1 1 99 99 V] -widget $annotationsControl]

set annotationsControl [get_charts_data -annotation $annotationsControl -name widget_path]
