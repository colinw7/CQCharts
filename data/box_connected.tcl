proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/digits.tsv -first_line_header]

set plot [create_plot -model $model -type boxplot \
  -columns "set=0,value=2,group=1" \
  -properties "outlier.visible=0,options.connected=1" \
  -title "connected boxplot"]

connect_chart -plot $plot -from objIdPressed -to objPressed
