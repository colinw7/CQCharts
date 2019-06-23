set types [get_charts_data -name plot_types]

foreach type $types {
# set names [get_charts_data -type $type -name properties]

# foreach name $names {
#   set value [get_charts_data -type $type -name $name]

#   echo "$name : $value"
# }

  set desc [get_charts_data -type $type -name desc]
  set html [get_charts_data -type $type -name html_desc]

  echo "$type : $desc"
# echo "$html"

  set parameters [get_charts_data -type $type -name parameters]

  foreach parameter $parameters {
#   set names [get_charts_data -type $type -name parameter.properties -data $parameter]

#   foreach name $names {
#     set value [get_charts_data -type $type -name parameter.$name -data $parameter]

#     echo "  $name : $value"
#   }

    set desc [get_charts_data -type $type -name parameter.desc -data $parameter]

    echo "  $parameter : $desc"

    set required  [get_charts_data -type $type -name parameter.required    -data $parameter]
    set monotonic [get_charts_data -type $type -name parameter.monotonic   -data $parameter]
    set ptype     [get_charts_data -type $type -name parameter.type        -data $parameter]
    set ptyped    [get_charts_data -type $type -name parameter.type_detail -data $parameter]

    if {$ptype == "column"} {
      set pstr "${ptype}(${ptyped})"
    } else {
      set pstr "$ptype"
    }

    if {$required } { append pstr " required"  }
    if {$monotonic} { append pstr " monotonic" }

    echo "    $pstr"
  }
}

exit
