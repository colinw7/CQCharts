# force directed link model
set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{link 0} {value 1}} -title "Energy Sources"]

set_charts_property -plot $plot -name animation.initSteps    -value 500
set_charts_property -plot $plot -name animation.animateSteps -value 10
#set_charts_property -plot $plot -name animation.running     -value 0

set_charts_property -plot $plot -name animation.autoHideBusyButton -value 0

set_charts_property -plot $plot -name node.scaled      -value 0
set_charts_property -plot $plot -name node.text.scaled -value 1
set_charts_property -plot $plot -name node.shapeType   -value BOX

set_charts_property -plot $plot -name edge.shapeType -value ARC

set_charts_property -plot $plot -name edge.mouseValue -value 1

set_charts_property -view current -name inside.insideColor -value red
set_charts_property -view current -name inside.insideAlpha -value 1

set_charts_property -view current -name inside.highlight.mode -value FILL
set_charts_property -view current -name inside.highlight.fill.color -value red
set_charts_property -view current -name inside.highlight.stroke.color -value red
set_charts_property -view current -name inside.highlight.stroke.width -value 4px

set_charts_property -view current -name select.highlight.mode -value FILL
set_charts_property -view current -name select.highlight.fill.color -value red
set_charts_property -view current -name select.highlight.stroke.color -value red
set_charts_property -view current -name select.highlight.stroke.width -value 4px
