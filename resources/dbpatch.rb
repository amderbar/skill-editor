require "rexml/document"
require 'active_support/core_ext'

file = ARGV[0]

# XMLファイル読み込み
doc = REXML::Document.new(File.new(file))

#Hashに変換
hash = Hash.from_xml(doc.to_s)

hash.each{|k,v|
    puts k, v
}