proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/multi_series.tsv -comment_header -column_type "time:format=%Y%m%d,oformat=%F"]

set plot [create_plot -type scatter -columns "x=0,y=1" -title "Scatter Plot"]

connect_chart -plot $plot -from objIdPressed -to objPressed
