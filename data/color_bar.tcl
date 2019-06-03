set model [load_charts_model -csv data/color_data.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{name 0} {value 1} {color 2}} \
  -properties {{mapping.color.enabled 1}}]
