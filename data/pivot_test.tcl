set df {{
"A": ["foo", "foo", "foo", "foo", "foo", "bar", "bar", "bar", "bar"],
"B": ["one", "one", "one", "two", "two", "one", "one", "two", "two"],
"C": ["small", "large", "large", "small", "small", "large", "small", "small", "large"],
"D": [1, 2, 2, 3, 3, 4, 5, 6, 7],
"E": [2, 4, 5, 5, 6, 6, 8, 9, 9]}}

set model [load_charts_model -json @df]
write_charts_model -model $model

if {0} {
set modelp [create_charts_pivot_model -model $model \
  -hcolumns {C} -vcolumns {A B} -dcolumns {D}]
write_charts_model -model $modelp

set modelp [create_charts_pivot_model -model $model \
  -hcolumns {C} -vcolumns {A B} -dcolumns {D} -fill_value 0]
write_charts_model -model $modelp

set modelp [create_charts_pivot_model -model $model \
  -vcolumns {A C} -dcolumns {D E} -fill_value 0 -value_types {{D mean} {E mean}}]
write_charts_model -model $modelp

set modelp [create_charts_pivot_model -model $model \
  -vcolumns {A C} -dcolumns {D E} -fill_value 0 -value_types {{D mean} {E {min max mean}}}]
write_charts_model -model $modelp
}

set modelp [create_charts_pivot_model -model $model \
  -hcolumns B -vcolumns A -dcolumns {D E}]
write_charts_model -model $modelp

#set plot [create_charts_plot -type empty]
