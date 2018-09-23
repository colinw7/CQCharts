set types [get_charts_data -name types]

foreach type $types {
  set desc [get_charts_data -type $type -name desc]
  set html [get_charts_data -type $type -name html_desc]

  echo "$type : $desc"
# echo "$html"

  set parameters [get_charts_data -type $type -name parameters]

  foreach parameter $parameters {
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
