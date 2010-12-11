#!/usr/bin/ruby

require './isearch'

IndexSearch.init("../../dump.dat", "../../dump-index.dat", "../../dump-map.txt")
p IndexSearch.find("ho", 10)

