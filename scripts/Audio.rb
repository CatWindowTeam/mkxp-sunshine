# Legacy shit
module Audio
    @music_group = self.create_group
    @sounds_group = self.create_group

    @bgm_playback = nil
    @bgm_pos = 0
    @bgs_playback = nil
    @me_playback = nil
    @se_playbacks = []

    class << self
        def bgm_play(path, volume = 100, pitch = 100)
            if (@bgm_playback && @bgm_playback.path == path)
                return
            end

            bgm_stop
            @bgm_pos = 0
            @bgm_playback = self.create_sound(path, false, @music_group)
            @bgm_playback.play(-1, volume / 100.0, pitch / 100.0)
        end
        def bgm_fade(time)
            if !@bgm_playback
                return
            end
            @bgm_pos = @bgm_playback.position
            @bgm_playback.fade_out(time / 60.0)
        end
        def bgm_fade_in(time)
            if !@bgm_playback
                return
            end
            @bgm_playback.fade_in(time / 60.0, -1)
            @bgm_playback.position = @bgm_pos
        end
        def bgm_stop
            if @bgm_playback
                @bgm_playback.stop
            end
        end
        def bgm_pos
            if @bgm_playback
                @bgm_playback.position
            end
            0
        end

        def bgs_play(path, volume = 100, pitch = 100)
            bgs_stop
            if (@bgs_playback)
                @bgs_playback.fade_out(0.02)
            end
            @bgs_playback = self.create_sound(path, false, @sounds_group)
            @bgs_playback.play(-1, volume / 100.0, pitch / 100.0)
        end
        def bgs_fade(time)
            @bgs_playback.fade_out(time / 60.0)
        end
        def bgs_stop
            if @bgs_playback
                @bgs_playback.stop
            end
        end
        def bgs_pos
            if @bgs_playback
                @bgs_playback.position
            end
            0
        end

        def me_play(path, volume = 100, pitch = 100)
            if @bgm_playback && @bgm_playback.playing
                @bgm_pos = @bgm_playback.position
            end
            bgm_fade(20);

            @me_playback = self.create_sound(path, true, @sounds_group)
            @me_playback.play(0, volume / 100.0, pitch / 100.0)
        end
        def me_fade(time)
            @me_playback.fade_out(time / 60.0)
            if @bgm_playback
                @bgm_playback.position = @bgm_pos
                @bgm_playback.fade_in(time / 60.0)
            end
        end
        def me_stop
            if @me_playback
                @me_playback.stop
            end
        end

        def se_play(path, volume = 100, pitch = 100)
            sound = self.create_sound(path, true, @sounds_group)
            sound.play(0, volume / 100.0, pitch / 100.0)
            @se_playbacks << sound
        end
        def se_stop
            @se_playbacks.clear
        end

        def update
            if @me_playback && !@me_playback.playing
                @me_playback = nil
                if @bgm_playback && !@bgm_playback.playing
                    @bgm_playback.fade_in(40.0 / 60.0)
                    @bgm_playback.position = @bgm_pos
                end
            end

            @se_playbacks.delete_if { |se| !se.playing }
        end

        def bgm_volume
            self.get_group_volume(@music_group * 100.0)
        end
        def bgm_volume=(val)
            self.set_group_volume(@music_group, val / 100.0)
        end

        def sfx_volume
            self.get_group_volume(@sounds_group)
        end
        def sfx_volume=(val)
            self.set_group_volume(@sounds_group, val / 100.0)
        end

        def music_group
            @music_group
        end
        def sounds_group
            @sounds_group
        end
    end
end

# meow