set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{0 time} {iformat %Y%m%d} {oformat %d/%m/%Y}}}]

set plot [create_charts_plot -type distribution -model $model -columns {{value {1 2 3}}}]

set_charts_property -plot $plot -name bucket.auto  -value 0
set_charts_property -plot $plot -name bucket.delta -value 5

show_charts_create_plot_dlg -model $model
