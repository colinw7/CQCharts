set model [load_charts_model -csv data/date_dist.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{time} {format %Y%m%d} {oformat %F}}

set plot [create_charts_plot -model $model -type distribution \
  -columns {{values 0} {data 1}}]
