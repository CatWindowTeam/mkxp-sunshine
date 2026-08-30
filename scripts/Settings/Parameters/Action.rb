class Window_Settings
  class ActionParameter < BaseParameter
    TYPE = :action

    def initialize(settings_content, screen_id, position, name, icon, value_text, func, arg1, arg2, arg3, arg4)
      @func = func
      @arg1 = arg1
      @arg2 = arg2
      @arg3 = arg3
      @arg4 = arg4

      super(settings_content, screen_id, position, name, icon, nil, value_text)
    end
    
    def action()
      return if @disabled
      if @arg1 == nil && @arg2 == nil && @arg3 == nil && @arg4 == nil
        @func.call
      elsif @arg2 == nil && @arg3 == nil && @arg4 == nil
        @func.call(@arg1)
      elsif @arg3 == nil && @arg4 == nil
        @func.call(@arg1, @arg2)
      elsif @arg4 == nil
        @func.call(@arg1, @arg2, @arg3)
      else
        @func.call(@arg1, @arg2, @arg3, @arg4)
      end
      @settings_content.redraw_all
      $game_system.se_play($data_system.buzzer_se)
    end
  end
end