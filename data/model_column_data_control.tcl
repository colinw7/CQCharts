proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

# create chord plot
set model [load_charts_model -csv data/chord-cities.csv -comment_header]

set plot1 [create_charts_plot -model $model -type chord \
  -columns {{link 0} {value 1} {group 2} {color 3} {controls 2}} -title "chord"]

# create empty plot
set plot2 [create_charts_plot -type empty -xmin 0 -xmax 1 -ymin 0 -ymax 1]

#---

set modelDataControl [qt_create_widget -type CQChartsModelColumnDataControl -name modelDataControl]

set ann [create_charts_widget_annotation -plot $plot2 -id modelDataControl \
  -position [list -1 -1] -widget $modelDataControl]

#set_charts_property -annotation $ann -name interactive -value 1
set_charts_property -annotation $ann -name rectangle -value {0 0 1 1}

set view [get_charts_data -plot $plot1 -name view]

set_charts_property -view $view -name options.plotSeparators -value 1

connect_charts_signal -plot $plot2 -from annotationPressed -to annotationSlot

#---

place_charts_plots -vertical $plot1 $plot2
