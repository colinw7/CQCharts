set env(CQCHARTS_BUFFER_LAYERS) 0
set env(CQCHARTS_PLOT_UPDATE_TIMEOUT) 0
set env(CQCHARTS_OBJ_TREE_WAIT) 1

perf -tracing 1

perf -start_recording

set modelId [load_model -csv data/xy_100000.csv -first_line_header]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "XY (100000 Points)"]
#set_charts_property -plot $plotId -name lines.visible -value 0

#print_chart -plot $plotId -file xy_100000.png

#print_chart -plot $plotId -layer mid_plot -file mid_xy_100000.png

#set plotId [create_plot -type distribution -columns "value=1" -title "Distribution (100000 Points)"]

sync_qt -n 100

perf -end_recording

perf -tracing 0
