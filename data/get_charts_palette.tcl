echo "--- Themes ---"

set themes [get_charts_palette -name themes]

echo $themes

foreach theme $themes {
  set queries [get_charts_palette -theme $theme -name "?"]

  foreach query $queries {
    set value [get_charts_palette -theme $theme -name $query]

    echo "theme : $theme : $query = $value"
  }
}

echo "--- Palettes ---"

set palettes [get_charts_palette -name palettes]

echo $palettes

foreach palette $palettes {
  set color_type [get_charts_palette -palette $palette -name "color_type"]

  set queries [get_charts_palette -palette $palette -name "?"]
  
  foreach query $queries {
    if       {$query == "color"} {
      if {$color_type == "defined"} {
        set value [get_charts_palette -palette $palette -name $query -data 0]
      }
    } elseif {$query == "interp_color"} {
      set value [get_charts_palette -palette $palette -name $query -data 0.0]
    } else {
      set value [get_charts_palette -palette $palette -name $query]
    }
       
    echo "palette : $palette : $query = $value"
  }
}

echo "--- Interfacce ---"

set queries [get_charts_palette -interface -name "?"]
  
foreach query $queries {
  if {$query == "interp_color"} {
    set value [get_charts_palette -interface -name $query -data 0.0]
  } else {
    set value [get_charts_palette -interface -name $query]
  }
       
  echo "interface : $query = $value"
}

echo "--- Color Models ---"

set color_models [get_charts_palette -name color_models]

echo $color_models
