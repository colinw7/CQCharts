set modelId [load_charts_model -csv data/xy_100000.csv -first_line_header]

#set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY (100000 Points)" \
# -properties {{lines.visible 0} {points.visible 1}}]

#set_charts_property -plot $plot1 -name lines.visible -value 0

#set plot2 [create_charts_plot -type barchart -columns {{name 0} {value 1}} \
# -title "Barchart (100000 points)" -properties {{stroke.visible 0}}]

set plot3 [create_charts_plot -type scatter -columns {{x 0} {y 1}} -title "Scatter (100000 points)"]

#set plot4 [create_charts_plot -type bubble -columns {{name 0} {value 1}} -title "Bubble (100000 points)"]

#set plot5 [create_charts_plot -type distribution -columns {{value 1}} \
# -title "Distribution (100000 Points)"]

#print_chart -plot $plot1 -file xy_100000.png
#print_chart -plot $plot1 -layer mid_plot -file mid_xy_100000.png

#set view [get_charts_property -plot $plot1 -name viewId]

#set plots [get_charts_data -view $view -name plots]

#place_charts_plots -view $view -rows 2 -columns 3 $plots
