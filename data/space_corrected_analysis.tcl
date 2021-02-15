set model [load_charts_model -csv data/Space_Corrected.csv -first_line_header]

# Fri Jun 08, 2001 15:08 UTC
set_charts_data -model $model -column 4 -name column_type -value {{time} {format {%a %b %d, %Y %H:%M UTC}}}

#set day_col [process_charts_model -model $model -add -expr {timeval(4,"%Y")} -header Year]

set nr [get_charts_data -model $model -name num_rows]
set nc [get_charts_data -model $model -name num_columns]

set countries_map {
 "Russia" "Russian Federation"
 "New Mexico" "USA"
 "Yellow Sea" "China"
 "Shahrud Missile Test Site" "Iran"
 "Pacific Missile Range Facility" "USA"
 "Barents Sea" "Russian Federation"
 "Gran Canaria" "USA"
}

set column {}

for {set r 0} {$r < $nr} {incr r} {
  set loc [get_charts_data -model $model -row $r -column 3 -name value]

  set locs [split $loc ","]
  set loc1 [string trim [lindex $locs end]]

  set loc2 [string map $countries_map $loc1]

  lappend column $loc2
}

#echo $column

process_charts_model -model $model -add -tcl $column -header country

show_charts_manage_models_dlg

if {0} {
set plot [create_charts_plot -model $model -type sunburst \
  -columns {{name {9 2 8}}} -title "space missions"]

set_charts_property -plot $plot -name options.innerRadius -value 0
set_charts_property -plot $plot -name options.sortType    -value SIZE
}

if {0} {
set plot [create_charts_plot -model $model -type barchart \
  -columns {{group 2} {value 7}} -title "space missions"]

#set_charts_property -plot $plot -name options.plotType -value STACKED
set_charts_property -plot $plot -name options.valueType -value SUM
set_charts_property -plot $plot -name options.skipEmpty -value 1
}

set plot [create_charts_plot -model $model -type distribution \
  -columns {{values Datum[%Y]}} -title "space missions"]
