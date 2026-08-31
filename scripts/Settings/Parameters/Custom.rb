class Window_Settings
  class CustomParameter < BaseParameter
    TYPE = :custom
    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: nil, callbacks: [])
      @callbacks = callbacks

      callback(:init, settings_content, screen_id, position, name, additional_icons, icons, parameter, init_value, callbacks) do
        super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value)
      end
    end
    def callback(name, *args)
      original = proc { yield if block_given? }

      if proc = @callbacks[name]
        instance_exec(original, *args, &proc)
      else
        original.call
      end
    end
    
    def opacity
      callback(:opacity_get) do super end
    end
    def opacity=(value)
      callback(:opacity_set, value) do super(value) end
    end

    def value
      callback(:value_get) do super end
    end
    def value=(value)
      callback(:value_set, value) do super(value) end
    end

    def name
      callback(:name_get) do super end
    end
    def name=(name)
      callback(:name_set, name) do super(name) end
    end

    def get_display_value
      callback(:get_display_value) do super end
    end

    def redraw_icon
      callback(:redraw_icon) do super end
    end

    def offset
      callback(:offset) do super end
    end

    def redraw_title
      callback(:redraw_title) do super end
    end

    def redraw_value
      callback(:redraw_value) do super end
    end

    def redraw_separator
      callback(:redraw_separator) do super end
    end

    def redraw
      callback(:redraw) do super end
    end

    def update
      callback(:update) do super end
    end

    def dispose
      callback(:dispose) do super end
    end

    def value_left
      callback(:value_left) do super end
    end
    def value_right
      callback(:value_right) do super end
    end
    def action
      callback(:action) do super end
    end
    # only for visual updating, its not selecting for real
    def select(previous)
      callback(:select, previous) do super(previous) end
    end
    def deselect
      callback(:deselect) do super end
    end
  end
end
