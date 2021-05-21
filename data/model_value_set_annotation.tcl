set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set ann [create_charts_value_set_annotation -plot $plot -rectangle {25 25 75 75}]

set_charts_property -annotation $ann -name drawType -value BARCHART
set_charts_property -annotation $ann -name modelColumn -value 1:1
