set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type distribution \
  -columns {{values 0}} \
  -title "distribution"]

set_charts_property -plot $plot -name filter.minValue -value 100
set_charts_property -plot $plot -name options.skipEmpty -value 1
