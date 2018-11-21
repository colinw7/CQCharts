proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/ToothGrowth.csv -first_line_header]

set view [create_view]

set plot1 [create_plot -view $view -model $model -type boxplot -columns "group=dose,set=supp,value=len"]

connect_charts -plot $plot1 -from objIdPressed -to objPressed
