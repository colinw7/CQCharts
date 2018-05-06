load -csv data/split_bar.csv -first_line_header
modelInd = _rc

add_plot -type bar -columns "category=0,value=1"
plotInd1 = _rc

add_plot -type bar -columns "category=0,value=2"
plotInd2 = _rc

viewInd = "view1"

set_property -view $viewInd -name key.visible -value 1

set_property -view $viewInd -plot $plotInd1 -name rect -value {0 0 50 100}
set_property -view $viewInd -plot $plotInd1 -name horizontal -value 1
set_property -view $viewInd -plot $plotInd1 -name invertX -value 1
set_property -view $viewInd -plot $plotInd1 -name invertY -value 1
set_property -view $viewInd -plot $plotInd1 -name plotStyle.fill.visible -value 0
set_property -view $viewInd -plot $plotInd1 -name dataStyle.fill.visible -value 0
set_property -view $viewInd -plot $plotInd1 -name xaxis.side -value TOP_RIGHT
set_property -view $viewInd -plot $plotInd1 -name xaxis.grid.line.major.visible -value 1
set_property -view $viewInd -plot $plotInd1 -name xaxis.ticks.label.angle -value 30
set_property -view $viewInd -plot $plotInd1 -name yaxis.side -value TOP_RIGHT
set_property -view $viewInd -plot $plotInd1 -name key.visible -value 0
set_property -view $viewInd -plot $plotInd1 -name title.text.text -value "Male"
set_property -view $viewInd -plot $plotInd1 -name title.location -value BOTTOM
set_property -view $viewInd -plot $plotInd1 -name fill.color -value palette:0.5
set_property -view $viewInd -plot $plotInd1 -name margin.right -value 0

set_property -view $viewInd -plot $plotInd2 -name rect -value {50 0 100 100}
set_property -view $viewInd -plot $plotInd2 -name horizontal -value 1
set_property -view $viewInd -plot $plotInd2 -name invertX -value 0
set_property -view $viewInd -plot $plotInd2 -name invertY -value 1
set_property -view $viewInd -plot $plotInd2 -name plotStyle.fill.visible -value 0
set_property -view $viewInd -plot $plotInd2 -name dataStyle.fill.visible -value 0
set_property -view $viewInd -plot $plotInd2 -name xaxis.side -value TOP_RIGHT
set_property -view $viewInd -plot $plotInd2 -name xaxis.grid.line.major.visible -value 1
set_property -view $viewInd -plot $plotInd2 -name xaxis.ticks.label.angle -value 30
set_property -view $viewInd -plot $plotInd2 -name yaxis.side -value TOP_RIGHT
set_property -view $viewInd -plot $plotInd2 -name key.visible -value 0
set_property -view $viewInd -plot $plotInd2 -name title.text.text -value "Female"
set_property -view $viewInd -plot $plotInd2 -name title.location -value BOTTOM
set_property -view $viewInd -plot $plotInd2 -name fill.color -value palette:0.8
set_property -view $viewInd -plot $plotInd2 -name margin.left -value 0
