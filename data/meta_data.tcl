set model [load_charts_model -csv data/connections3.csv -first_line_header]

set names [get_charts_data -model $model -name meta_names]

foreach name $names {
  set keys [get_charts_data -model $model -name meta_keys -data $name]

  foreach key $keys {
    set value [get_charts_data -model $model -name meta_data -data [list $name $key]]

    echo "name=$name key=$key value=$value"
  }
}

exit
