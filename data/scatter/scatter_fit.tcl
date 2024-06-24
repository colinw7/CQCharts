set model [load_charts_model -tsv data/cities1.dat -comment_header]

process_charts_model -model $model -add -expr "column(2)/1000.0" -header "pop1"

set plot [create_charts_plot -model $model -type scatter \
 -columns {{x 4} {y 3} {name 0} {fontSize 5}}]

echo "Init : [get_charts_data -plot $plot -name fit_bbox]"

set view [get_charts_data -plot $plot -name view]

write_charts_stats -view $view

set_charts_property -plot $plot -name title.text.string -value "French Cities"
set_charts_property -plot $plot -name key.visible -value 0
set_charts_property -plot $plot -name points.labels.visible -value 1
set_charts_property -plot $plot -name mapping.fontSize.enabled -value 1
set_charts_property -plot $plot -name points.labels.position -value CENTER

connect_charts_signal -plot $plot -from plotObjsAdded -to postUpdateProc

proc postUpdateProc { view plot } {
  echo "postUpdateProc : [get_charts_data -plot $plot -name fit_bbox]"

  write_charts_stats -view $view
}
