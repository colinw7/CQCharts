set color_data [list \
 [list 1 10 red   fred] \
 [list 2 20 green bill] \
 [list 3 30 blue  harry] \
]

set model [load_charts_model -var color_data -transpose]

# map color directly in model
#set_charts_data -model $model -column 3 -name column_type \
#  -value {{color:mapped 1} {palette plasma}}

set plot [create_charts_plot -model $model -type barchart \
  -columns {{name 0} {value 1} {color 3}}]

set_charts_property -plot $plot -name mapping.color.enabled -value 1
set_charts_property -plot $plot -name mapping.color.palette -value plasma
