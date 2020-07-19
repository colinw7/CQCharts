set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type bubble \
  -columns {{group 0} {value @GROUP}} \
  -title "distribution"]

set_charts_property -plot $plot -name dataGrouping.bucket.auto -value 1
set_charts_property -plot $plot -name filter.minSize           -value 100
