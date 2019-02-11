set model [load_model -csv data/empty.csv -first_line_header]

set types [get_charts_data -name types]

set plots {}

foreach type $types {
  echo "$type"

  set plot [create_plot -type $type -model $model]

  if {$type == "forcedirected"} {
    set_charts_property -plot $plot -name options.running -value 0
  }

  if {[get_charts_data -type $type -name title]} {
    set_charts_property -plot $plot -name title.text.text -value $type
  }

  lappend plots $plot
}

###---

#set plot1 [lindex $plots 0]

#set view [get_charts_property -plot $plot1 -name viewId]

#set plots [get_charts_data -view $view -name plots]

place_plots -rows 5 -columns 5 $plots
