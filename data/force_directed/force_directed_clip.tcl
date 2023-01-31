# force directed link model
set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{link 0} {value 1}} -title "Energy Sources"]

set_charts_property -plot $plot -name animation.initSteps    -value 500
set_charts_property -plot $plot -name animation.animateSteps -value 10
#set_charts_property -plot $plot -name animation.running     -value 0

set_charts_property -plot $plot -name node.scaled    -value 0
set_charts_property -plot $plot -name node.shapeType -value BOX

set_charts_property -plot $plot -name node.text.scaled     -value 1
set_charts_property -plot $plot -name node.text.clipLength -value 5ex
set_charts_property -plot $plot -name node.text.clipElide  -value ElideLeft

set_charts_property -plot $plot -name edge.shapeType -value ARC

set_charts_property -plot $plot -name text.inside.visible -value 1
set_charts_property -plot $plot -name text.inside.noClip  -value 1
set_charts_property -plot $plot -name text.inside.noScale -value 1

set_charts_property -plot $plot -name text.selected.visible -value 1
set_charts_property -plot $plot -name text.selected.noClip  -value 1
set_charts_property -plot $plot -name text.selected.noScale -value 1
