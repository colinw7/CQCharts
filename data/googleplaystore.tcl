set model [load_model -csv data/googleplaystore.csv -first_line_header]

manage_model_dlg

add_process_model_proc fixSize { value } {
  if {"$value" == "Varies with device"} { return "NaN" }
  if {[regexp {M$} $value]} { return [regsub {M$} $value {}] }
  if {[regexp {k$} $value]} { return [expr {[regsub {(.*)k$} $value {\1}] / 1000.0}] }
  return $value
}

echo "Size"
process_model -model $model -column "Size" -modify -expr "fixSize(@v)" -force
set_charts_data -model $model -column "Size" -name column_type -value real

add_process_model_proc fixInstalls { value } {
  regsub {\+$} $value {} value
  regsub -all {,} $value {} value
  return $value
}

echo "Installs"
process_model -model $model -column "Installs" -modify -expr "fixInstalls(@v)" -force

add_process_model_proc fixPrice { value } {
  regsub {^\$} $value {} value
  return $value
}

echo "Price"
process_model -model $model -column "Price" -modify -expr "fixPrice(@v)" -force

add_process_model_proc fixAndroidVer { value } {
  if {"$value" == "Varies with device"} { return "NaN" }
  regsub { and up$} $value {} value
  if {[string length $value] > 3} { return [string range $value 0 2] }
  return $value
}

#echo "Current Ver"
#process_model -model $model -column "Current Ver" -modify -expr "fixAndroidVer(@v)" -force
echo "Android Ver"
process_model -model $model -column "Android Ver" -modify -expr "fixAndroidVer(@v)" -force

set_charts_data -model $model -column "Rating"       -name column_type -value real
set_charts_data -model $model -column "Installs"     -name column_type -value real
set_charts_data -model $model -column "Price"        -name column_type -value real
set_charts_data -model $model -column "Last Updated" -name column_type -value "time:format=%B %d\\, %Y"
set_charts_data -model $model -column "Android Ver"  -name column_type -value real

#set view [create_view]
#set plot [create_plot -model $model -type distribution -columns "value=Category"]

#set view1 [create_view]
#set plot1 [create_plot -view $view1 -model $model -type barchart -columns "group=Category,value=Installs"]

#set view2 [create_view]
#set plot2 [create_plot -view $view2 -model $model -type distribution -columns "value=Size"]

set view3 [create_view]
set plot3 [create_plot -view $view3 -model $model -type boxplot -columns "group=Type,value=Size"]
