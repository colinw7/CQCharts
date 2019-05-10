set red  {[31m}
set norm {[0m}

set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set types [get_charts_data -name types]

set view_checked 0

set annotations_checked 0

set title_checked 0
set axis_checked  0
set key_checked   0

foreach type $types {
  set view [create_charts_view]

  if {! $view_checked} {
    set properties [get_charts_data -view $view -name properties]

    foreach property $properties {
      set value [get_charts_property -view $view -name $property]
      set desc  [get_charts_property -view $view -name $property -desc]
      set dtype [get_charts_property -view $view -name $property -type]
      set owner [get_charts_property -view $view -name $property -owner]

      echo "view,,$property,$dtype,$value,$desc"

      if {"$desc" == ""} {
        echo "${red}No desc for : ${property}${norm}"
        exit
      }

      if {"$dtype" == ""} {
        echo "${red}No type for : ${property} ${desc}${norm}"
        exit
      }
    }

    set view_checked 1
  }

  set title_checked1 0
  set axis_checked1  0
  set key_checked1   0

  set plot [create_charts_plot -view $view -type $type]

  set properties [get_charts_data -plot $plot -name properties]

  foreach property $properties {
    set value [get_charts_property -plot $plot -name $property]
    set desc  [get_charts_property -plot $plot -name $property -desc]
    set dtype [get_charts_property -plot $plot -name $property -type]
    set owner [get_charts_property -plot $plot -name $property -owner]

    if {$owner == "title"} {
      if {$title_checked == 1} {
        continue
      }

      set title_checked1 1
    }

    if {$owner == "axis"} {
      if {$axis_checked == 1} {
        continue
      }

      set axis_checked1 1
    }

    if {$owner == "key"} {
      if {$key_checked == 1} {
        continue
      }

      set key_checked1 1
    }

    echo "plot,$type,$property,$dtype,$value,$desc"

    if {"$desc" == ""} {
      echo "${red}No desc for : ${property}${norm}"
      exit
    }

    if {"$dtype" == ""} {
      echo "${red}No type for : ${property} ${desc}${norm}"
      exit
    }
  }

  if {$title_checked1 == 1} {
    set title_checked 1
  }

  if {$axis_checked1 == 1} {
    set axis_checked 1
  }

  if {$key_checked1 == 1} {
    set key_checked 1
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
        set dtype [get_charts_property -annotation $annotation -name $property -type]
        set owner [get_charts_property -annotation $annotation -name $property -owner]

        echo "annotation,$type,$property,$dtype,$value,$desc"

        if {"$desc" == ""} {
          echo "${red}No desc for : ${property}${norm}"
          exit
        }

        if {"$dtype" == ""} {
          echo "${red}No type for : ${property} ${desc}${norm}"
          exit
        }
      }
    }

    set annotations_checked 1
  }

  remove_charts_view -view $view
}

exit
