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

require 'fileutils'

class GenericWrapper
    def initialize(entry_point_host, entry_point_port, self_port, binary_name,
		   wrapped_prompt_string)
        @entry_point_ip = nil
        @entry_point_port = entry_point_port.to_i
        @self_port = self_port.to_i
	@wrapped_in = nil
	@wrapped_out = nil
	@wrapped_prompt_string = wrapped_prompt_string

        if `which #{binary_name}`.empty?
            raise "Can't find #{binary_name}. Is your $PATH set?"
        end

	my_dir_name = "#{binary_name}.#{@self_port}"
	FileUtils.mkdir_p my_dir_name
	Dir.chdir my_dir_name
    end

    def start_wrapped(cmd_str, stdout_name)
        puts "# cmd_str: #{cmd_str}"

        # Write into wrapped using popen's pipe, but read its output from the
        # "stdout_name" file, because we can't let it block its printfs while we
        # don't read its output.
        puts "# starting wrapped command"
        @wrapped_in = IO.popen(cmd_str, "w")
        puts "# waiting creation of output file"
        begin
            sleep 0.1
        end until File.exists?(stdout_name)
        puts "# created!"
        @wrapped_out = File.open(stdout_name, "r")
    end

    def wrapped_getline
	ret = ""
	loop do
	    line = @wrapped_out.gets
	    if line.nil?
		sleep 0
	    elsif line[-1].chr == "\n"
		ret = ret + line
		return ret
	    else
		ret = ret + line
	    end
	end
    end

    def get_output
        loop do
            line = wrapped_getline
            if line.chomp == @wrapped_prompt_string
                break
            end
        end
    end

    def run
        loop do
            puts "w:$"
            STDOUT.flush
            command = gets
            if command.nil?
                break
            elsif command == "stop\n"
                break
            else
                process_command command.chomp
            end
        end
    end

    def process_command(command)
        case command
            when /^put ([^ ]+) ([^ ]+)/
                key = $1
                value = $2
                put(key, value)
            when /^get (.+)/
                key = $1
                value = get(key)
                if value.nil?
                    # not found
                    puts "Value not found"
                else
                    #found
                    puts "Value found:"
                    puts "#{value}"
                end
            else
                puts "# Unrecognized command: #{command}"
        end
    end

    def my_finalize
        puts "# Closing subprocess..."
        #@wrapped_in.close

        # "man 2 kill" will help you understand this genocide:
        Signal.trap("TERM", "IGNORE");
        Process.kill("TERM", 0);
        Signal.trap("TERM", "DEFAULT");

        puts "# Closed!"
    end
end

def parse_arguments(argv)

    entry_point_host = nil
    entry_point_port = nil
    self_port = nil

    while argv.any?
        curr_arg = argv.shift
        if curr_arg == "--eph" # entry point host
            entry_point_host = argv.shift
        elsif curr_arg == "--epp" # entry point port
            entry_point_port = argv.shift
        elsif curr_arg == "--sp" # self port
            self_port = argv.shift
        elsif curr_arg == "-h" or curr_arg == "--help"
            puts "Arguments:"
            puts "  --eph host: entry point host or IP"
            puts "  --epp port: entry point port"
            puts "  --sp:       self port"
            puts "  --help,-h:  prints this help"
            puts "Use only --sp if you want to create a new DHT"
	    exit 0
        end
    end

    if self_port.nil?
        puts "Self port not specified!"
        exit 1
    end

    if (!entry_point_port.nil?) and entry_point_host.nil?
        puts "--epp specificied, but missing --eph"
        exit 1
    end

    if (!entry_point_host.nil?) and entry_point_port.nil?
        puts "--eph specified, but missing --epp"
	exit 1
    end

    return entry_point_host, entry_point_port, self_port
end
