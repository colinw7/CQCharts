set model [load_charts_model -csv data/temp_bar_range.csv -first_line_header]

set plot [create_charts_plot -type distribution -model $model -columns {{values {2 3}}} \
  -parameter {bucketed 0} -parameter {autoRange 1} \
  -properties {{dataGrouping.group 0} {dataGrouping.rowGroups 1}}]
