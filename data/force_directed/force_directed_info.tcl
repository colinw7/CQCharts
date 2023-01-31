set objs [get_charts_data -plot current -name selected_objects]

foreach obj $objs {
  set names [get_charts_property -plot current -object $obj -name ?]
  echo $names

  set type [get_charts_property -plot current -object $obj -name typeName]

  if       {$type == "node"} {
    set label [get_charts_property -plot current -object $obj -name label]
    echo "Node : $label"
  } elseif {$type == "edge"} {
    set label [get_charts_property -plot current -object $obj -name label]
    echo "Edge : $label"
  }
}
