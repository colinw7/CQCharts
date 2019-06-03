set modelInd [load_charts_model -csv data/split_bar.csv -first_line_header]

set plotInd1 [create_charts_plot -type bar -columns {{group 0} {value 1}}]

set plotInd2 [create_charts_plot -type bar -columns {{group 0} {value 2}}]

set viewInd [get_charts_property -plot $plotInd1 -name viewId]

set_charts_property -view $viewInd -name key.visible -value 1

set_charts_property -plot $plotInd1 -name range.view -value {0 0 50 100}
set_charts_property -plot $plotInd1 -name options.horizontal -value 1
set_charts_property -plot $plotInd1 -name invert.x -value 1
set_charts_property -plot $plotInd1 -name invert.y -value 1
set_charts_property -plot $plotInd1 -name plotBox.fill.visible -value 0
set_charts_property -plot $plotInd1 -name dataBox.fill.visible -value 0
set_charts_property -plot $plotInd1 -name xaxis.side -value TOP_RIGHT
set_charts_property -plot $plotInd1 -name xaxis.grid.lines -value MAJOR
set_charts_property -plot $plotInd1 -name xaxis.ticks.label.text.angle -value 30
set_charts_property -plot $plotInd1 -name yaxis.label.text.visible -value 0
set_charts_property -plot $plotInd1 -name key.visible -value 0
set_charts_property -plot $plotInd1 -name title.text.string -value "Male"
set_charts_property -plot $plotInd1 -name title.location -value BOTTOM
set_charts_property -plot $plotInd1 -name fill.color -value {palette 0.5}
set_charts_property -plot $plotInd1 -name margins.outer.right -value 5%
set_charts_property -plot $plotInd1 -name margins.bar -value 12px

set_charts_property -plot $plotInd2 -name range.view -value {50 0 100 100}
set_charts_property -plot $plotInd2 -name options.horizontal -value 1
set_charts_property -plot $plotInd2 -name invert.y -value 1
set_charts_property -plot $plotInd2 -name plotBox.fill.visible -value 0
set_charts_property -plot $plotInd2 -name dataBox.fill.visible -value 0
set_charts_property -plot $plotInd2 -name xaxis.side -value TOP_RIGHT
set_charts_property -plot $plotInd2 -name xaxis.grid.lines -value MAJOR
set_charts_property -plot $plotInd2 -name xaxis.ticks.label.text.angle -value 30
set_charts_property -plot $plotInd2 -name yaxis.label.text.visible -value 0
set_charts_property -plot $plotInd2 -name yaxis.ticks.label.text.visible -value 0
set_charts_property -plot $plotInd2 -name key.visible -value 0
set_charts_property -plot $plotInd2 -name title.text.string -value "Female"
set_charts_property -plot $plotInd2 -name title.location -value BOTTOM
set_charts_property -plot $plotInd2 -name fill.color -value {palette 0.8}
set_charts_property -plot $plotInd2 -name margins.outer.left -value 5%
set_charts_property -plot $plotInd2 -name margins.bar -value 12px
