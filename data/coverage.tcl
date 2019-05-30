proc plotSlot { viewId  plotId rowId } {
  puts "plotSlot: $viewId, $plotId, $rowId"

  set rc [get_charts_data -plot $plotId -row_id $rowId -column 1 -name value]
  puts $rc
}

set modelId [load_charts_model -csv data/coverage.csv -first_line_header]

set_charts_data -model $modelId -name column_type -column 0 \
  -value {{time} {format %m/%d/%Y} {oformat %F}}

set plotId1 [create_charts_plot -type xy -columns {{id 0} {x 0} {y 1}}]

set viewId [get_charts_property -plot $plotId1 -name viewId]

set plotId2 [create_charts_plot -type xy -columns {{id 0} {x 0} {y 2}}]

set_charts_property -plot $plotId1 -name impulse.visible -value 1
set_charts_property -plot $plotId1 -name impulse.color   -value palette
set_charts_property -plot $plotId1 -name impulse.alpha   -value 0.5
set_charts_property -plot $plotId1 -name impulse.width   -value 20px

set_charts_property -plot $plotId2 -name invert.y        -value 1
set_charts_property -plot $plotId2 -name impulse.visible -value 1
set_charts_property -plot $plotId2 -name impulse.color   -value palette#1
set_charts_property -plot $plotId2 -name impulse.alpha   -value 0.5
set_charts_property -plot $plotId2 -name impulse.width   -value 20px

group_charts_plots -view $viewId -y1y2 $plotId1 $plotId2

place_charts_plots -view $viewId -vertical $plotId1 $plotId2

connect_charts_signal -plot $plotId1 -from objIdPressed -to plotSlot
