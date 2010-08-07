#!/usr/bin/ruby

require 'isearch'

IndexSearch.init("../../dump.dat", "../../dump-map.txt")
p IndexSearch.find("ho")

