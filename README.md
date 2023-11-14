# CQCharts

## Introduction

CQCharts is a Qt based charting library to support the display of an interactive
chart from a data model (QAbstractItemModel) with support for cross selection using
the data model's selection model (QItemSelectionModel).

Charts are live in that changes to the data model automatically update
the chart. This allows the programmer to build interactive charts where
the user can select items from the model using a table or tree view and/or
the displayed chart data.

The library comes with a test program which supports scripting of charts using the
'tcl' programming language. A number of example plot scripts are available in the
data directory.

See [doc/README.md](doc/README.md) for more details
