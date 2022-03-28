if {0} {
set df1 {{'lkey': ['foo', 'bar', 'baz', 'foo'], 'value': [1, 2, 3, 5]}}
set df2 {{'rkey': ['foo', 'bar', 'baz', 'foo'], 'value': [5, 6, 7, 8]}}

set model1 [load_charts_model -json @df1]
write_charts_model -model $model1
set model2 [load_charts_model -json @df2]
write_charts_model -model $model2

set model3 [join_charts_model -models [list $model1 $model2] -lcolumns lkey -rcolumns rkey]
if {$model3 != ""} { write_charts_model -model $model3 }
}

set df1 {{'a': ['foo', 'bar'], 'b': [1, 2]}}
set df2 {{'a': ['foo', 'baz'], 'c': [3, 4]}}

set model4 [load_charts_model -json @df1]
write_charts_model -model $model4
set model5 [load_charts_model -json @df2]
write_charts_model -model $model5

set model6 [join_charts_model -models [list $model4 $model5] -lcolumns a -type inner]
if {$model6 != ""} { write_charts_model -model $model6 }

set model7 [join_charts_model -models [list $model4 $model5] -lcolumns a -type left]
if {$model7 != ""} { write_charts_model -model $model7 }

exit
