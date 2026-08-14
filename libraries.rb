#!/usr/bin/ruby
#encoding: utf-8
puts "copying libraries..."
require 'fileutils'

# These libraries MUST NOT be packaged with OneShot
# or otherwise they will cause cross-distro
# incompatibilities.
BLACKLIST = [
  # These packaged on Manjaro cause issues on openSUSE.
  'libc.so.6',
  'libpthread.so.0',
  'libdl.so.2',
  'libm.so.6',
  # we dont need Windows dlls
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
  "WS2_32.dll"
]

files = []
while line = gets
  if line =~ / => (\/.*) \(/
    path = $1
    filename = File.basename(path)
    next if BLACKLIST.any? { |library| filename.casecmp?(library) }
    path = `cygpath -w "#{path}"`.strip if ENV['MSYSTEM'] && path.start_with?("/")
    files << path
  end
end
FileUtils.cp(files, 'libs')
puts "copying libraries done."
