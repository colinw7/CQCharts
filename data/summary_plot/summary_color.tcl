set model [load_charts_model -csv data/summary_color.csv -first_line_header]

set plot [create_charts_plot -type summary -model $model \
  -columns {{columns {0 1 2}} {color 3} {symbolType 4} {symbolSize 5}}]
