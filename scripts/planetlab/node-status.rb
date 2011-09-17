#!/usr/bin/ruby
#
# dhtperf: DHT performance evaluation tool
# Copyright (C) 2011 Paulo Zanoni <przanoni@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

SLICE_NAME="ufpr_dht"

def check(node)

    #ping_cmd = "sudo ping -q -c 20 -w 3 -f #{node} > /dev/null"
    ssh_cmd = "ssh -o ConnectTimeout=5 #{SLICE_NAME}@#{node} rpm -q dhtperf"

#    if system(ping_cmd)
	version = `#{ssh_cmd}`.chomp
	if version =~ /dhtperf.*fc8/
	    $nodes[node] = "y:#{node}:#{version}"
	else
	    $nodes[node] = " :#{node}:wrong-version"
	end
#    else
#	$nodes[node] = " :#{node}:ping-fail"
#    end

    puts $nodes[node]
end

def main(argv)
    if ENV["NODES_FILE"].nil?
	file_path = "nodes"
    else
	file_path = ENV["NODES_FILE"]
    end

    file = File.open(file_path, "r")

    $nodes = {}

    file.each_line { |line|
	if line =~ /^y:/
	    host = line.chomp.split(":")[1]
	    Thread.start do
		check(host)
	    end
	    sleep 1.5
	end
    }

    # ctrl+c _once_ will save the file and exit
    begin
	loop do
	    threads = Thread.list
	    puts "#{threads.size} threads remaining"
	    break if threads.size == 1
	    sleep 1
	end
    rescue Interrupt
	puts "Interrupt! Saving..."
    end

    File.open("today.nodes", "w") do |f|
	$nodes.each_value { |value|
	    f.puts value
	}
    end
    return 0
end

Thread.abort_on_exception = true
# Make it stop asking us the password
#`sudo ls > /dev/null`
exit main(ARGV)
