set modelId [load_charts_model -csv data/p2.csv]

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

set viewId [get_charts_property -plot $plot -name state.viewId]

set ann [create_charts_tk_widget_annotation -plot $plot -id tkw1 -position [list 0 0]]

button .tkw1.button1 -text One
button .tkw1.button2 -text Two
button .tkw1.button3 -text Three

pack .tkw1.button1 .tkw1.button2 .tkw1.button3
