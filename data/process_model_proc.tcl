set modelId [load_charts_model -csv data/chord-cities.csv]
puts $modelId

define_charts_proc fromProc { arg } {
  return [lindex [split $arg / ] 0]
}

define_charts_proc toProc { arg } {
  return [lindex [split $arg / ] 1]
}

process_charts_model -model $modelId -add -expr "fromProc(column(0))" -header "From"
process_charts_model -model $modelId -add -expr "toProc(column(0))" -header "To"

show_charts_manage_models_dlg
