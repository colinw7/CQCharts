proc animateStateChanged { view plot b } {
  echo "animateStateChanged $view $plot $b"

  set objs [get_charts_data -plot $plot -name objects]
  echo $objs

  set node1 {}
  set node2 {}
  set edge1 {}

  foreach obj $objs {
    set type [get_charts_property -plot $plot -object $obj -name typeName]

    if       {$type == "node"} {
      if       {$node1 == ""} {
        set node1 $obj
      } elseif {$node2 == ""} {
        set node2 $obj
      }
    } elseif {$type == "edge"} {
      if {$edge1 == ""} {
        set edge1 $obj
      }
    }

    if {$node1 != "" && $edge1 != ""} {
      break
    }
  }

  echo "Node: $node1, Edge $edge1"
  echo "Node: $node2"

  if {$node1 == "" || $node2 == ""} {
    return
  }

  #set_charts_data -plot $plot -object $node1 -name select -value 1
  #set_charts_data -plot $plot -object $edge1 -name select -value 1

  #set startPos "@($node1 intersect) 0 0"
  #set endPos   "@($node2 intersect) 0 0"
  set startPos "@($node1) 0 0"
  set endPos   "@($node2) 0 0"
  echo "StartPos: $startPos, EndPos: $endPos"

  #set startPos [list "@(${node1})" 0 0]
  #set endPos   [list "@(${node2})" 0 0]
  #echo "StartPos: $startPos, EndPos: $endPos"

  set edge [create_charts_arrow_annotation -plot $plot -id edge -start $startPos -end $endPos]

  set_charts_property -annotation $edge -name midHead.type   -value ARROW
  set_charts_property -annotation $edge -name midHead.length -value 0.1P
  set_charts_property -annotation $edge -name line.width     -value 0.05P
}

proc objPressed { view plot id } {
  echo "$view $plot $id"

  set model [get_charts_data -plot $plot -name model]

  set objs [get_charts_data -plot $plot -object $id -name connected]

  echo "Connected"

  foreach obj $objs {
    echo "$obj"
  }
}

proc plotAnnotationSlot { viewId plotId id } {
  puts "plotAnnotationSlot: $viewId, $plotId, $id"

  set text [get_charts_property -annotation $id -name text.string]
  puts "text: $text"
}

set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{link 0} {value 1}} -title "Force Directed Budget"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed

set button [create_charts_button_annotation -plot $plot -id button \
              -position {5 5 V} -text "Button"]

connect_charts_signal -plot $plot -from animateStateChanged -to animateStateChanged
connect_charts_signal -plot $plot -from annotationIdPressed -to plotAnnotationSlot
