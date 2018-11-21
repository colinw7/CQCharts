proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/multi_series.tsv -comment_header \
 -column_type "time:format=%Y%m%d"]

set plot [create_plot -model $model -type xy \
  -columns "x=0,y=1 2 3" \
  -properties "lines.visible=1,points.visible=1" \
  -title "multiple xy plot"]

connect_charts -plot $plot -from objIdPressed -to objPressed
