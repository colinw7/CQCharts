set model [load_charts_model -csv data/arrowstyle.csv]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}}]

set view [get_charts_property -plot $plot -name viewId]

set annotation [create_charts_text_annotation -plot $plot -id one -position {0 -60} -text "One"]

get_charts_property -view $view -name "?"

get_charts_property -plot $plot -name "?"

get_charts_property -annotation $annotation -name "?"

set obj [lindex [get_charts_data -plot $plot -name objects 0] 0]

get_charts_property -plot $plot -object $obj -name "?"
