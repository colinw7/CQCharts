set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{values 1}} -title "pie value set"]

set_charts_property -plot $plot -name bucket.enabled -value 1
set_charts_property -plot $plot -name bucket.count   -value 20
