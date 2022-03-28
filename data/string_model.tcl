# csv model
set csv_str {
A,B,C
1,2,3
3,4,5
}

set csv_model [load_charts_model -csv @csv_str]
write_charts_model -model $csv_model

# tsv model
set tsv_str {
A	B	C
1	2	3
3	4	5
}

set tsv_model [load_charts_model -tsv @tsv_str]
write_charts_model -model $tsv_model

# json model
set json_str {{'lkey': ['foo', 'bar', 'baz', 'foo'], 'value': [1, 2, 3, 5]}}

set json_model [load_charts_model -json @json_str]
write_charts_model -model $json_model

# xml model ?

exit
