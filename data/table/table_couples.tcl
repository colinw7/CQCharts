set model [load_charts_model -csv data/couples.csv -first_line_header -first_column_header]

set draw_colors [list #9f2b68 #aa4444 #008080 #4444dd #bf40bf #aa8888 #70d3cf #8888dd #dddd44]

for {set i 0} {$i < 9} {incr i} {
  set c [lindex $draw_colors $i]

  set_charts_data -model $model -column $i -name column_type \
    -value [list [list integer] [list draw_type barchart] [list draw_color $c] [list min 0] [list preferred_width 150]]
}

set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4 5 6 7 8}}}]

set_charts_property -plot $plot -name vheader.visible -value 1
#set_charts_property -plot $plot -name options.rowNumber -value 1
