#!/usr/bin/ruby
require 'zlib'

def usage
  STDERR.puts "Tool for building and extracting xScripts.rxdata"
  STDERR.puts "\nUsage examples:"
  STDERR.puts "Create xScripts: rpgscript.rb scripts_dir game_dir"
  STDERR.puts "Extract xScripts: rpgscript.rb output_dir game_dir x"
  exit 1
end

usage if ARGV.length < 2
scripts_dir = ARGV[0]
game_dir = ARGV[1]
if ARGV.length >= 3
  usage if ARGV[2] != 'x'
  extract = true
else
  extract = false
end

# Determine version of game engine
game_data_dir = File.join(game_dir, 'Data')
unless Dir.exist? game_data_dir
  STDERR.puts "error: #{game_dir} does not have a Data subdirectory"
  exit 1
end

target_path = nil
Dir.entries(game_data_dir).each do |e|
  ext = File.extname(e)
  if ext =~ /\.r[xv]data2?/
    target_path = File.join(game_data_dir, 'xScripts' + ext)
    break
  end
end

unless target_path
  STDERR.puts "warning: could not determine game engine version, assuming XP"
  target_path = File.join(game_data_dir, 'xScripts.rxdata')
end

if extract
  # Make sure the script directory exists
  Dir.mkdir(scripts_dir) unless Dir.exist? scripts_dir

  # Keep track of names of scripts extracted so we can warn about duplicates
  names = Hash.new(0)

  # Read scripts
  File.open(target_path, 'rb') do |fin|
    Marshal.load(fin).each_with_index do |script, index|
      name = script[1].strip
      data = Zlib::Inflate.inflate(script[2]).rstrip
        .gsub(/[ \t]*(?:$|\r\n?)/, "\n")

      # Make sure this file doesn't already exist
      if name.empty?
        if data.empty? || data == "\n"
          next
        else
          name = 'UNTITLED'
        end
      end

      names[name] += 1
      if names[name] > 1
        name << " (#{names[name]})"
      end

      file_path = File.join(scripts_dir, name + ".rb")

      # Make subdirs if its doesn't exists
      dir_path = File.dirname(file_path)
      FileUtils.mkdir_p(dir_path)

      # Write script file
      File.open(file_path, 'wb') do |fout|
        fout.write(data)
      end
    end
  end
  puts "#{target_path} extracted."
else
  # Write scripts
  script_files = Dir.glob("#{scripts_dir}/**/*.rb", base: "scripts").select { |f| File.file?(f) }

  priorities = script_files.map do |file_path|
    priority = 0
    first_line = File.open(file_path, &:gets)
    if first_line && first_line.rstrip.gsub(/[ \t]*(?:$|\r\n?)/, "\n") =~ /#\s*PRIORITY\s+(-?\d+)$/
      priority = $1.to_i
    end

    [priority, file_path]
  end
  sorted = priorities.sort_by { |priority, path| [-priority, path] }
  final = sorted.map { |priority, path| path }

  #puts final

  scripts = []
  final.each do |path|
    path.strip!
    next if path.empty?

    data = File.read(path).rstrip.gsub("\n", "\r\n")

    script = Array.new(3)
    script[0] = 0
    script[1] = path.delete_suffix(".rb")
    script[2] = Zlib.deflate(data)
    scripts << script
  end

  File.open(target_path, 'wb') { |f| f.write(Marshal.dump(scripts)) }
  puts "#{target_path} written."
end

