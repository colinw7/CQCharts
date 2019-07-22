set model [load_charts_model -csv data/googleplaystore.csv -first_line_header]

show_charts_manage_models_dlg

define_charts_proc fixSize { value } {
  if {"$value" == "Varies with device"} { return "NaN" }
  if {[regexp {M$} $value]} { return [regsub {M$} $value {}] }
  if {[regexp {k$} $value]} { return [expr {[regsub {(.*)k$} $value {\1}] / 1000.0}] }
  return $value
}

echo "Size"
process_charts_model -model $model -column "Size" -modify -expr "fixSize(@v)" -force
set_charts_data -model $model -column "Size" -name column_type -value {{real}}

define_charts_proc fixInstalls { value } {
  regsub {\+$} $value {} value
  regsub -all {,} $value {} value
  return $value
}

echo "Installs"
process_charts_model -model $model -column "Installs" -modify -expr "fixInstalls(@v)" -force

define_charts_proc fixPrice { value } {
  regsub {^\$} $value {} value
  return $value
}

echo "Price"
process_charts_model -model $model -column "Price" -modify -expr "fixPrice(@v)" -force

define_charts_proc fixAndroidVer { value } {
  if {"$value" == "Varies with device"} { return "NaN" }
  regsub { and up$} $value {} value
  if {[string length $value] > 3} { return [string range $value 0 2] }
  return $value
}

#echo "Current Ver"
#process_charts_model -model $model -column "Current Ver" -modify -expr "fixAndroidVer(@v)" -force
echo "Android Ver"
process_charts_model -model $model -column "Android Ver" -modify -expr "fixAndroidVer(@v)" -force

set_charts_data -model $model -column "Rating"       -name column_type -value {real}
set_charts_data -model $model -column "Installs"     -name column_type -value {real}
set_charts_data -model $model -column "Price"        -name column_type -value {real}
set_charts_data -model $model -column "Last Updated" -name column_type \
  -value {time {format "%B %d, %Y"}}
set_charts_data -model $model -column "Android Ver"  -name column_type -value {real}

proc cat_dist { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type distribution \
    -columns {{value Category}}]
  return $plot
}

proc install_dist { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type distribution \
    -columns {{value Installs}}]
  return $plot
}

proc bar_installs { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type barchart \
    -columns {{group Category} {value Installs}} \
    -properties {{options.plotType STACKED}}]
  return $plot
}

proc dist_installs { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type distribution \
    -columns {{value Category} {data Installs}} \
    -properties {{options.valueType SUM}}]
  return $plot
}

proc size_dist { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type distribution \
    -columns {{value Size}}]
  return $plot
}

proc category_type_distribution { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type distribution \
    -columns {{group Type} {value Category}}]
  return $plot
}

proc size_boxplot { model } {
  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model -type boxplot \
    -columns {{group Type} {value Size}}]
  return $plot
}

proc nan_plot { model } {
  filter_charts_model -model $model -expr {isnan($Size)}
  set n1 [get_charts_data -model $model -name num_rows]

  filter_charts_model -model $model -expr {isnan($Rating)}
  set n2 [get_charts_data -model $model -name num_rows]

  filter_charts_model -model $model -expr {isnan(column(12))}
  set n3 [get_charts_data -model $model -name num_rows]

  set ::names {{Column} {Size} {Rating} {Android Ver}}
  set ::values [list {Count} $n1 $n2 $n3]

  set model1 [load_charts_model -var ::names -var ::values -first_line_header]

  set view [create_charts_view]
  set plot [create_charts_plot -view $view -model $model1 -type barchart \
    -columns {{name Column} {value Count}}]

  return $plot
}

proc add_visible { model } {
  set vis [process_charts_model -model $model -add -expr "1" -header "Visible" -type boolean]

  return $vis
}

proc mark_duplicates { model } {
  set vis [get_charts_data -model $model -name column_index -data Visible]

  if {$vis == -1} {
    set vis [add_visible $model]
  }

  sort_model -model $model -column 0

# set inds [get_charts_data -model $model -name duplicates]
  set inds [get_charts_data -model $model -name duplicates -column 0]

  foreach ind $inds {
    set_charts_data -model $model -column $vis -row $ind -name value -value 0
  }
}
