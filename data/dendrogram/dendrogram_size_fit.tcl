set model [load_charts_model -csv data/dendrogram_tall.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{name 0} {value 1}} -title "dendrogram"]

set_charts_property -plot $plot -name title.visible -value 0

set_charts_property -plot $plot -name root.visible -value 0
set_charts_property -plot $plot -name hier.shape -value box
set_charts_property -plot $plot -name hier.size  -value 0.1P
set_charts_property -plot $plot -name leaf.shape -value box
set_charts_property -plot $plot -name leaf.size  -value 0.1P

set_charts_property -plot $plot -name root.label.position -value CENTER
set_charts_property -plot $plot -name root.label.scaled -value 1
set_charts_property -plot $plot -name hier.label.position -value CENTER
set_charts_property -plot $plot -name hier.label.scaled -value 1
set_charts_property -plot $plot -name leaf.label.position -value CENTER
set_charts_property -plot $plot -name leaf.label.scaled -value 1

set_charts_property -plot $plot -name options.fitMode -value SIZE

set_charts_property -plot $plot -name options.spreadNodeOverlaps -value 1

set_charts_property -plot $plot -name header.visible -value 1

if {0} {
set_charts_property -plot $plot -name header.values -value [ \
  list \
    {} \
    {<table><tr><td style="text-align: center">A</td></tr><tr><td style="text-align: center">start</td></tr></table>} \
    {<table><tr><td style="text-align: center">B</td></tr><tr><td style="text-align: center">middle</td></tr></table>} \
    {<table><tr><td style="text-align: center">C</td></tr><tr><td style="text-align: center">end</td></tr></table>} \
]

set_charts_property -plot $plot -name header.tips -value [ \
  list \
  {} \
  {Column A} \
  {Column B} \
  {Column C} \
  \
]
}

set_charts_property -plot $plot -name header.margin  -value 4px
set_charts_property -plot $plot -name header.spacing -value 4px

set_charts_property -plot $plot -name header.text.scaled -value 1
set_charts_property -plot $plot -name header.text.align -value AlignCenter
set_charts_property -plot $plot -name header.text.html -value 1

#set_charts_property -plot $plot -name plotBox.fill.color -value {interface 0.12}
#set_charts_property -plot $plot -name plotBox.clip -value 0

set_charts_property -plot $::plot -name options.pixelScaled -value 1

set_charts_property -plot $::plot -name edge.width -value 3px
set_charts_property -plot $::plot -name edge.stroke.alpha -value 0.3

proc expandAll { view plot } {
  disconnect_charts_signal -plot $plot -from plotObjsAdded -to expandAll

  execute_charts_slot -plot $plot -name expand_all
}

connect_charts_signal -plot $plot -from plotObjsAdded -to expandAll
