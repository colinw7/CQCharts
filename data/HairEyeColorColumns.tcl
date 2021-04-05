set model [load_charts_model -csv data/HairEyeColor.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{values Freq} {controls {Hair Eye Sex}}}]
