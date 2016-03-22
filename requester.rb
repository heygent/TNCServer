#!/usr/bin/env ruby

require 'rest-client'

numrequests = ARGV[0].to_i
numthreads = ARGV[1].to_i

threads = []

numthreads.times do |threadnum|
  threads << Thread.new do 
    numrequests.times do |requestnum|
      begin
        RestClient.get 'http://localhost:8080/img.jpg'
        puts "Thread #{threadnum}, request #{requestnum}"
      rescue
        puts "Quitting. Thread #{threadnum}, connection error."
      end
    end
  end
end

threads.each do |th|
  th.join
end
