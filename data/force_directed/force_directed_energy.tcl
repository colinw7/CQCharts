proc animateStateChanged { view plot b } {
  echo "animateStateChanged $view $plot $b"
}

proc selectionChanged { view plot } {
  echo "selectionChanged $view $plot"

  set objs [get_charts_data -plot $plot -name selected_objects]

  foreach obj $objs {
    #set names [get_charts_property -plot $plot -object $obj -name ?]
    #echo $names

    set type [get_charts_property -plot $plot -object $obj -name typeName]

    if       {$type == "node"} {
      set inds [get_charts_data -plot $plot -object $obj -name inds]
      echo "Inds $inds"

      set id [get_charts_property -plot $plot -object $obj -name id]
      set label [get_charts_property -plot $plot -object $obj -name label]
      echo "Node : $id $label"
    } elseif {$type == "edge"} {
      set inds [get_charts_data -plot $plot -object $obj -name inds]
      echo "Inds $inds"

      set id [get_charts_property -plot $plot -object $obj -name id]
      set label [get_charts_property -plot $plot -object $obj -name label]
      echo "Edge : $id $label"
    }
  }
}

# force directed link model
set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{link 0} {value 1}} -title "Energy Sources"]

set_charts_property -plot $plot -name animation.initSteps    -value 500
set_charts_property -plot $plot -name animation.animateSteps -value 10
#set_charts_property -plot $plot -name animation.running     -value 0

set_charts_property -plot $plot -name node.scaled      -value 0
set_charts_property -plot $plot -name node.text.scaled -value 1
set_charts_property -plot $plot -name node.shapeType   -value BOX

set_charts_property -plot $plot -name edge.shapeType -value ARC

connect_charts_signal -plot $plot -from animateStateChanged -to animateStateChanged
connect_charts_signal -plot $plot -from selectionChanged -to selectionChanged
