set modelId [load_model -tsv data/cities1.dat -comment_header]

puts $modelId

export_model -model $modelId -to "csv"
export_model -model $modelId -to "tsv"

set modelId [load_model -tsv data/comma.tsv]

puts $modelId

export_model -model $modelId -to "csv"
export_model -model $modelId -to "tsv"
