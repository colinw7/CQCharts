proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -tsv data/bivariate.tsv -comment_header \
 -column_type {{{0 time} {iformat %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y {1 2}}} \
  -properties {{bivariate.visible 1} {yaxis.includeZero 1}} \
  -title "bivariate - single line"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
