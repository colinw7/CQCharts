# basic load
set model1 [load_model -csv data/diamonds.csv -first_line_header]

set nr [get_charts_data -model $model1 -name num_rows   ]
set nc [get_charts_data -model $model1 -name num_columns]

assert {$nr == 53940}
assert {$nc == 10}

# max rows
set model2 [load_model -csv data/diamonds.csv -first_line_header -max_rows 100]

set nr [get_charts_data -model $model2 -name num_rows   ]
set nc [get_charts_data -model $model2 -name num_columns]

assert {$nr == 100}
assert {$nc == 10}

# specific columns
set model3 [load_model -csv data/diamonds.csv -first_line_header -columns {price clarity cut}]

set nr [get_charts_data -model $model3 -name num_rows   ]
set nc [get_charts_data -model $model3 -name num_columns]

assert {$nr == 53940}
assert {$nc == 3}

#show_charts_manage_models_dlg
