set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

# create empty plot
set plot [create_charts_plot -type empty]

#---

set plotTable [qt_create_widget -type CQChartsPlotTableControl -name plotTable]

set plotTableAnn [create_charts_widget_annotation -plot $plot -id plotTable \
  -rectangle [list 1 1 99 99 V] -widget $plotTable]

set plotTable [get_charts_data -annotation $plotTable -name widget_path]
