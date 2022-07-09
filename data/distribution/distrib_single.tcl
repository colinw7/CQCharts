set model [load_charts_model -csv data/boxplot.csv -first_line_header \
 -column_type {integer integer integer}]

set plot1 [create_charts_plot -model $model -type distribution \
  -columns {{group 0} {values 2}} -properties {{bucket.enabled 0}} \
  -title "Disrib Single Bucket"]


