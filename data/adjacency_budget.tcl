proc objPressed { view plot id } {
  #echo "$view $plot $id"

  set model [get_charts_data -plot $plot -name model]

  set objs [get_charts_data -plot $plot -object $id -name connected]

  echo "Connected"

  foreach obj $objs {
    echo "$obj"
  }
}

set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type adjacency -columns {{link 0} {value 1}}]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
