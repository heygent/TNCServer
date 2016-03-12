#!/usr/bin/env ruby

require 'rest-client'

numrequests_arg, numthreads_arg = ARGV

numrequests = numrequests_arg.to_i
numthreads = numthreads_arg.to_i

threads = []

numthreads.times do |threadnum|
  threads << Thread.new do 
    numrequests.times do |requestnum|
      begin
        RestClient.get 'http://localhost:8080'
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
