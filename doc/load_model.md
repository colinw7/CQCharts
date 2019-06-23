# Loading Data #

To load data use the charts_\_load\_model\_ tcl command.

This command supports loading files in the following formats:
 + csv
    + Comma Separated Value data
 + tsv
     + Tab Separated Value Data
 + json
     + JSON Data
 + data
    + GNUPlot like data (space separated)

 and also allows the data to be generated from a tcl expression or read from a tcl variable (list of lists).

For csv, csv and data formats the reader recognises lines starting with # as comments. Headers for the columns can be take from the first non-comment line or first comment line.

csv defaults to using a comma for the value separator. This can be changed to any character to support other separator types.

csv format also supports specially formated comments to support meta data for specifying additional column data e.g. column types and formats.

The tcl expression is run on a specified number of rows (default 100).

The data processed can be limited by specifying a maximum number of rows, a tcl filter expression or specific columns.

The types of the columns can be specified.

The command returns a unique identifier the for model which can be used in other commands e.g. as the input model for a plot.

