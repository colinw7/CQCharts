proc writeStats { view plot } {
  write_charts_stats -view $view -bbox {{5 3} {7 4}}
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1}} -title "Scatter Group"]

set view [get_charts_data -plot $plot -name view]

connect_charts_signal -plot $plot -from plotObjsAdded -to writeStats
