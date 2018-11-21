set model [load_model -csv data/color_data.csv -first_line_header]

set plot [create_plot -model $model -type barchart \
  -columns "name=0,value=1,color=2" \
  -properties "color.map.enabled=1"]
