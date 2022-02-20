set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -type xy -columns {{x 0} {y {{@REF x}}}} -title "XY Plot"]
