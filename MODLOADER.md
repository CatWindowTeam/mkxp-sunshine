# wtf is hook?
Hook - Ruby script executed in other script, with context(binding) of parent script.

# Hooks dirs
For more information you can read scripts/
* hooks/*/init/ - executed in script after initialization (you can change some variables and other)
'*' - class like Scene_Title
* hooks/Window_NameInput/init2 - Window_NameInput init function
* hooks/Scene_Map/main - main function of Map scene
* hooks/Main/at_exit - on game exit
* hooks/Main/start - at game start

# How add custom hooks?
use RPG::Mod.exec_hooks(path, binding)
path - path in game directory like "hooks/Scene_Title/init", you can't load something from other place, only from game direcory.
binding - its a Ruby binding.

# API
Custom API for mods:

work in progress

# how create mod?
First what you need it 
