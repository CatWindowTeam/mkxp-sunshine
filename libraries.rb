#!/usr/bin/ruby
#encoding: utf-8
puts "copying libraries..."
require 'fileutils'

# These libraries MUST NOT be packaged with OneShot
# or otherwise they will cause incompatibilities.
BLACKLIST = [
  # Linux/*NIX
  'libc.so',
  'libpthread.so',
  'libdl.so',
  'libm.so',
  'linux-vdso.so',
  'libstdc++.so',
  # Windows system dlss
  "ntdll.dll",
  "KERNEL32.DLL",
  "KERNELBASE.dll",
  "ADVAPI32.dll",
  "msvcrt.dll",
  "sechost.dll",
  "RPCRT4.dll",
  "ucrtbase.dll",
  "SHLWAPI.dll",
  "USER32.dll",
  "win32u.dll",
  "GDI32.dll",
  "gdi32full.dll",
  "WINMM.dll",
  "DWrite.dll",
  "VERSION.dll",
  "Secur32.dll",
  "IMM32.DLL",
  "IPHLPAPI.DLL",
  "ole32.dll",
  "OLEAUT32.dll",
  "SETUPAPI.dll",
  "SHELL32.DLL",
  "SSPICLI.DLL",
  "USP10.DLL",
  "WS2_32.dll",
  "bcrypt.dll",
  "combase.dll",
  "imagehlp.dll",
  "msvcp_win.dll"
]

outDir = "libs"
if ARGV.length >= 1
  outDir = ARGV[0]
end

files = []
while line = STDIN.gets
  if line =~ / => (\/.*) \(/
    path = $1
    filename = File.basename(path)
    next if BLACKLIST.any? { |library| filename.downcase.start_with?(library.downcase) }
    path = `cygpath -w "#{path}"`.strip if ENV['MSYSTEM'] && path.start_with?("/")
    files << path
  end
end
FileUtils.cp(files, outDir)
puts "copying libraries done."
