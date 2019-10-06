CQChartsTest -csv data/color_data.csv -first_line_header \
  -type barchart -columns "name=0,values=1,color=2" \
  -properties "color.map.enabled=1"
