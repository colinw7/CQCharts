set model [create_charts_data_model -rows 6 -columns 2]

set xcol [list @TCL [list 2 3 4 5 6 7]]
set ycol [list [list @TCL [list 1 3 5 5 3 1]]]

set plot [create_charts_plot -type xy -columns [list [list x $xcol] [list y $ycol]] \
  -title "XY Plot"]
