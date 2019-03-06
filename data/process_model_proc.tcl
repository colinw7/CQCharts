set modelId [load_model -csv data/chord-cities.csv]
puts $modelId

define_charts_proc fromProc { arg } {
  return [lindex [split $arg / ] 0]
}

define_charts_proc toProc { arg } {
  return [lindex [split $arg / ] 1]
}

process_model -model $modelId -add -expr "fromProc(column(0))" -header "From"
process_model -model $modelId -add -expr "toProc(column(0))" -header "To"

manage_models_dlg
