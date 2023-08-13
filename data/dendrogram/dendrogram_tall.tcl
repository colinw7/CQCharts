set model [load_charts_model -csv data/dendrogram_tall.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{name 0} {value 1}} -title "dendrogram"]

set_charts_property -plot $plot -name root.visible -value 0
set_charts_property -plot $plot -name hier.shape -value box
set_charts_property -plot $plot -name hier.size  -value 0.1P
set_charts_property -plot $plot -name leaf.shape -value box
set_charts_property -plot $plot -name leaf.size  -value 0.1P

set_charts_property -plot $plot -name options.fitMode -value SCROLL

#set_charts_property -plot $plot -name options.spreadNodeOverlaps -value 1
#set_charts_property -plot $plot -name plotBox.fill.color -value {interface 0.12}
#set_charts_property -plot $plot -name plotBox.clip -value 0

proc expandAll { view plot } {
  disconnect_charts_signal -plot $plot -from plotObjsAdded -to expandAll

  execute_charts_slot -plot $plot -name expand_all
}

connect_charts_signal -plot $plot -from plotObjsAdded -to expandAll
