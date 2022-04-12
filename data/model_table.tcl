set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

# create empty plot
set plot [create_charts_plot -type empty]

#---

set modelTable [qt_create_widget -type CQChartsModelTableControl -name modelTable]

set modelTableAnn [create_charts_widget_annotation -plot $plot -id modelTable \
  -rectangle [list 1 1 99 99 V] -widget $modelTable]

set modelTable [get_charts_data -annotation $modelTable -name widget_path]
