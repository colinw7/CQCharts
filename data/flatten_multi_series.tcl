set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

echo "date,city,temperature"

for {set r 0} {$r < $nr} {incr r} {
  set date  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set ny    [get_charts_data -model $model -column 1 -row $r -name value -role display]
  set sf    [get_charts_data -model $model -column 2 -row $r -name value -role display]
  set au    [get_charts_data -model $model -column 3 -row $r -name value -role display]

  echo "$date,New York,$ny"
  echo "$date,San Francisco,$sf"
  echo "$date,Austin,$au"
}

exit
