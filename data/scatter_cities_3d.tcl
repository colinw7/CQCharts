set model [load_charts_model -tsv data/cities1.dat -comment_header]

process_charts_model -model $model -add -expr "column(2)/1000.0" -header "pop1"

set plot [create_charts_plot -model $model -type scatter3d \
 -columns {{x Longitude} {y Latitude} {z Population} {name 0}}]

set_charts_property -plot $plot -name title.text.string -value "French Cities"
set_charts_property -plot $plot -name key.visible -value 0
set_charts_property -plot $plot -name labels.visible -value 1
#set_charts_property -plot $plot -name dataLabel.position -value CENTER
set_charts_property -plot $plot -name range.zmin -value 0
