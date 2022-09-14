set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set_charts_data -model $model -column 1 -name column_type -value {{integer} {sum 10000000}}

set plot [create_charts_plot -model $model -type barchart -columns {{name 0} {values 1}}]

set_charts_property -plot $plot -name title.text.string -value "Population by State"
set_charts_property -plot $plot -name title.subtitle.string -value "Under 5 years"

set_charts_property -plot $plot -name options.percent -value 1

set objs [get_charts_data -plot $plot -name objects -sync]

assert {[llength $objs] == 6}

set sum [get_charts_data -model $model -name details.parameter -column 1 -data sum]
echo $sum

set names [get_charts_data -model $model -name details.parameter -column 1 -data ?]
echo $names

foreach name $names {
  set value [get_charts_data -model $model -name details.parameter -column 1 -data $name]
  echo "$name='$value'"
}

set_charts_data -model $model -name details.parameter -column 1 -data sum -value 2704659

set sum [get_charts_data -model $model -name details.parameter -column 1 -data sum]
echo $sum
