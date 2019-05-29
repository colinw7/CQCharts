proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/ToothGrowth.csv -first_line_header]

set view [create_charts_view]

set plot1 [create_charts_plot -view $view -model $model -type boxplot \
  -columns {{group dose} {value len}} \
  -where {$supp=="OJ"} -title "dose: OJ"]

set plot2 [create_charts_plot -view $view -model $model -type boxplot \
  -columns {{group dose} {value len}} \
  -where {$supp=="VC"} -title "dose: VC"]

place_charts_plots -horizontal $plot1 $plot2

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed
connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed
