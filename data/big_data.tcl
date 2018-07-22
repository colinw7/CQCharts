set modelId [load_model -csv data/xy_100000.csv -first_line_header]

#set plotId [create_plot -type xy -columns "x=0,y=1" -title "XY (100000 Points)"]
#set_property -plot $plotId -name lines.visible -value 0

#print_chart -plot $plotId -file xy_100000.png

#print_chart -plot $plotId -layer mid_plot -file mid_xy_100000.png

set plotId [create_plot -type distribution -columns "value=1" -title "Distribution (100000 Points)"]
