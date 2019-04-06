set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set types [get_charts_data -name types]

set view_checked 0

set annotations_checked 0

foreach type $types {
  set view [create_charts_view]

  if {! $view_checked} {
    set properties [get_charts_data -view $view -name properties]

    foreach property $properties {
      set value [get_charts_property -view $view -name $property]
      set desc  [get_charts_property -view $view -name $property -desc]

      #echo "$property = $value ($desc)"

      if {"$desc" == ""} {
        echo "No desc for : $property"
      }
    }

    set view_checked 1
  }

  set plot [create_charts_plot -view $view -type $type]

  set properties [get_charts_data -plot $plot -name properties]

  foreach property $properties {
    set value [get_charts_property -plot $plot -name $property]
    set desc  [get_charts_property -plot $plot -name $property -desc]

    #echo "$property = $value ($desc)"

    if {"$desc" == ""} {
      echo "No desc for : $property"
    }
  }

  if {! $annotations_checked} {
    set types [get_charts_data -name annotation_types]

    foreach type $types {
      set cmd "create_charts_${type}_annotation"

      if {$type == "polygon" || $type == "polyline"} {
        set annotation [eval $cmd -plot $plot -points {{0 0} {1 0} {1 1}}]
      } else {
        set annotation [eval $cmd -plot $plot]
      }

      set properties [get_charts_data -annotation $annotation -name properties]

      foreach property $properties {
        set value [get_charts_property -annotation $annotation -name $property]
        set desc  [get_charts_property -annotation $annotation -name $property -desc]

        #echo "$property = $value ($desc)"

        if {"$desc" == ""} {
          echo "No desc for : $property"
        }
      }
    }

    set annotations_checked 1
  }

  remove_charts_view -view $view
}

exit
