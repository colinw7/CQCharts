proc plotSlot(viewId, plotId, rowId)
  print "plotSlot", viewId, plotId, rowId

  get_data -view $viewId -plot $plotId -id $rowId -column 1

  print _rc
endproc

load_model -csv data/coverage.csv -first_line_header
modelId = _rc

set_model -ind $modelId -column_type "0#time:format=%m/%d/%Y,oformat=%F"

create_plot -type xy -columns "id=0,x=0,y=1"
plotId1 = _rc

get_property -plot $plotId1 -name viewId
viewId = _rc

create_plot -type xy -columns "id=0,x=0,y=2"
plotId2 = _rc

set_property -view $viewId -plot $plotId1 -name impulse.visible -value 1
set_property -view $viewId -plot $plotId1 -name impulse.color   -value palette
set_property -view $viewId -plot $plotId1 -name impulse.alpha   -value 0.5
set_property -view $viewId -plot $plotId1 -name impulse.width   -value 20px

set_property -view $viewId -plot $plotId2 -name invert.y        -value 1
set_property -view $viewId -plot $plotId2 -name impulse.visible -value 1
set_property -view $viewId -plot $plotId2 -name impulse.color   -value palette#1
set_property -view $viewId -plot $plotId2 -name impulse.alpha   -value 0.5
set_property -view $viewId -plot $plotId2 -name impulse.width   -value 20px

group_plots -y1y2 $plotId1 $plotId2

place_plots -vertical $plotId1 $plotId2

connect -view $viewId -plot $plotId1 -from objIdPressed -to plotSlot
