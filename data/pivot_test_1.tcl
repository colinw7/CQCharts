set df {{
"foo": ["one", "one", "one", "two", "two", "two"],
"bar": ["A", "B", "C", "A", "B", "C"],
"baz": [1, 2, 3, 4, 5, 6],
"zoo": ["x", "y", "z", "q", "w", "t"]}}
#echo "df=$df"

set model [load_charts_model -json @df]
write_charts_model -model $model

set modelp [create_charts_pivot_model -model $model \
  -vcolumns {foo} -hcolumns {bar} -dcolumns {baz}]
write_charts_model -model $modelp

set modelp [create_charts_pivot_model -model $model \
  -vcolumns {foo} -hcolumns {bar} -dcolumns {baz zoo}]
write_charts_model -model $modelp
