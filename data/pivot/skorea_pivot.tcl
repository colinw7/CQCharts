cd skorea_covid

set model1 [load_charts_model -csv TimeAge.csv -first_line_header]
set model2 [load_charts_model -csv TimeGender.csv -first_line_header]
set model3 [load_charts_model -csv TimeProvince.csv -first_line_header]

proc print_headers { model } {
  set nc [get_charts_data -model $model -name num_columns]

  set names {}

  for {set i 0} {$i < $nc} {incr i} {
    set name [get_charts_data -model $model -header -name value -column $i]

    lappend names $name
  }

  echo $names
}

print_headers $model1
print_headers $model2
print_headers $model3

set model1p [create_charts_pivot_model -model $model1 \
  -hcolumns {age} -vcolumns {date} -dcolumns {confirmed deceased}]
set model2p [create_charts_pivot_model -model $model2 \
  -hcolumns {sex} -vcolumns {date} -dcolumns {confirmed deceased}]
set model3p [create_charts_pivot_model -model $model3 \
  -hcolumns {province} -vcolumns {date} -dcolumns {confirmed deceased}]

write_charts_model -model $model1p
write_charts_model -model $model2p
write_charts_model -model $model3p
