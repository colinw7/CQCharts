set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -type xy -columns {{x 0} {y 1}} -title "XY Plot"]

execute_charts_slot -view $view -name show_table -args [list 1]
