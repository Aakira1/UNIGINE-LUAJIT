component("PhysicsFallSound", {
    description = "Plays a sound when a physics-driven cube impacts any surface, with pitch variation based on impact velocity",

    on_init = "fall_sound_init",
    on_update = "fall_sound_update",

    properties = {
        impact_threshold = {
            type = "float",
            title = "Impact Distance Threshold",
            default = 0.5,
            description = "Distance from surface to detect impact"
        },
        sound_path = {
            type = "file",
            title = "Impact Sound",
            default = "sounds/preview.mp3"
        },
        min_pitch = {
            type = "float",
            title = "Minimum Pitch",
            default = 0.8,
            min = 0.5,
            max = 2.0,
            description = "Pitch for light impacts (OpenAL: 0.5–2.0)"
        },
        max_pitch = {
            type = "float",
            title = "Maximum Pitch",
            default = 1.5,
            min = 0.5,
            max = 2.0,
            description = "Pitch for heavy impacts (OpenAL: 0.5–2.0)"
        },
        cooldown_time = {
            type = "float",
            title = "Impact Cooldown (seconds)",
            default = 0.2,
            description = "Time between impact sounds"
        },
        min_velocity = {
            type = "float",
            title = "Minimum Impact Velocity (m/s)",
            default = 0.5,
            description = "Minimum velocity to trigger impact sound"
        }
    }
})

function fall_sound_init(self)
    -- Track cooldown timer and previous position
    self.cooldown_timer = 0.0
    self.previous_position = nil  -- Will be set on first update
    self.first_frame = true

    -- Get the node (passed from C++ wrapper)
    if not self.node then
        Log.error("[PhysicsFallSound] No node available\n")
        return
    end

    -- Validate sound path
    local sound_path = self.sound_path or "sounds/preview.mp3"

    -- Check if path is empty or nil
    if sound_path == "" or sound_path == nil then
        Log.warning("[PhysicsFallSound] No valid sound path specified, disabling sound\n")
        self.sound = nil
        return
    end

    Log.message(string.format("[PhysicsFallSound] Creating SoundSource and loading sound: '%s'\n", sound_path))
    self.sound = SoundSource.create(sound_path, 0)

    if not self.sound then
        Log.error(string.format("[PhysicsFallSound] Failed to create SoundSource\n"))
        return
    end

    -- Set the sample path (this actually loads the audio file)
    self.sound:setSampleName(sound_path)
    Log.message(string.format("[PhysicsFallSound] Sample set to: '%s'\n", self.sound:getSampleName()))

    -- Check if sound system is enabled
    if Sound.isEnabled() then
        Log.message(string.format("[PhysicsFallSound] Sound system is ENABLED (Volume: %.2f)\n", Sound.getVolume()))
    else
        Log.warning("[PhysicsFallSound] Sound system is DISABLED!\n")
    end

    -- Configure sound properties (OpenAL-safe: gain >= 0, pitch 0.5–2.0, distances > 0)
    self.sound:setLoop(0)  -- Don't loop
    self.sound:setPlayOnEnable(false)  -- Don't auto-play when enabled
    self.sound:setGain(1.0)  -- Full volume
    self.sound:setMinDistance(1.0)   -- OpenAL ref distance > 0
    self.sound:setMaxDistance(50.0) -- OpenAL max distance > 0

    Log.message(string.format("[PhysicsFallSound] Sound gain: %.2f, Sample: %s\n",
        self.sound:getGain(), self.sound:getSampleName()))

    -- Add sound as child first, then set local position to (0,0,0)
    -- This keeps the sound at the same location as the parent node
    self.node:addChild(self.sound)
    self.sound:setPosition(Vec3.new(0, 0, 0))  -- Local position relative to parent
    self.sound:setEnabled(true)  -- Ensure sound is enabled

    Log.message("[PhysicsFallSound] Initialized successfully\n")
end

function fall_sound_update(self, dt)
    if not self.node then return end

    -- Update cooldown timer
    self.cooldown_timer = self.cooldown_timer + dt

    -- Get cube world position
    local world_pos = self.node:getWorldPosition()

    -- Handle first frame - just store position and return
    if self.first_frame or not self.previous_position then
        self.previous_position = world_pos
        self.first_frame = false
        return
    end

    -- Calculate current velocity (difference in position over time)
    local velocity_x = (world_pos.x - self.previous_position.x) / dt
    local velocity_y = (world_pos.y - self.previous_position.y) / dt
    local velocity_z = (world_pos.z - self.previous_position.z) / dt

    -- Store current position for next frame
    self.previous_position = world_pos

    -- Safety: defaults and OpenAL-safe pitch range (0.5–2.0)
    local min_velocity = self.min_velocity or 0.5
    local impact_threshold = self.impact_threshold or 0.5
    local cooldown_time = self.cooldown_time or 0.2
    local min_pitch = math.max(0.5, math.min(2.0, self.min_pitch or 0.8))
    local max_pitch = math.max(0.5, math.min(2.0, self.max_pitch or 1.5))

    -- Define 6 face directions (normalized vectors from cube center)
    local faces = {
        {name = "Down",    dir = Vec3.new(0, 0, -1)},
        {name = "Up",      dir = Vec3.new(0, 0, 1)},
        {name = "Forward", dir = Vec3.new(0, 1, 0)},
        {name = "Back",    dir = Vec3.new(0, -1, 0)},
        {name = "Right",   dir = Vec3.new(1, 0, 0)},
        {name = "Left",    dir = Vec3.new(-1, 0, 0)}
    }

    -- Early exit if cooldown hasn't passed (prevents unnecessary raycasts)
    if self.cooldown_timer < cooldown_time then
        return
    end

    -- Calculate velocity magnitude once (used for all faces)
    local velocity_mag = math.sqrt(
        velocity_x * velocity_x +
        velocity_y * velocity_y +
        velocity_z * velocity_z
    )

    -- Early exit if velocity is too low
    if velocity_mag < min_velocity then
        return
    end

    -- Check each face for nearby surfaces
    for _, face in ipairs(faces) do
        -- Calculate raycast target (1 unit in face direction)
        local target = Vec3.new(
            world_pos.x + face.dir.x * 1.5,
            world_pos.y + face.dir.y * 1.5,
            world_pos.z + face.dir.z * 1.5
        )

        -- Create intersection object
        local hit = PhysicsIntersectionNormal.create()

        -- Perform raycast (mask 1 = all physics objects)
        local result = Physics.getIntersection(world_pos, target, 1, hit)

        if result then
            local hit_point = hit:getPoint()

            -- Calculate distance
            local dx = world_pos.x - hit_point.x
            local dy = world_pos.y - hit_point.y
            local dz = world_pos.z - hit_point.z
            local distance = math.sqrt(dx*dx + dy*dy + dz*dz)

            -- Check if close enough for impact AND velocity is directed TOWARD the surface
            local velocity_vec = Vec3.new(velocity_x, velocity_y, velocity_z)
            local dot_product = velocity_vec.x * face.dir.x + velocity_vec.y * face.dir.y + velocity_vec.z * face.dir.z

            if distance < impact_threshold and
               dot_product < -0.1 then  -- Moving toward surface (negative dot product)

                -- Map velocity to pitch (0–10 m/s -> min_pitch to max_pitch), OpenAL range 0.5–2.0
                local pitch = min_pitch + (velocity_mag / 10.0) * (max_pitch - min_pitch)
                pitch = math.max(0.5, math.min(2.0, pitch))

                if self.sound and pitch == pitch then
                    -- Stop any currently playing sound first
                    if self.sound:isPlaying() then
                        self.sound:stop()
                    end
                    self.sound:setPitch(pitch)
                    if not self.sound:isEnabled() then
                        self.sound:setEnabled(true)
                    end
                    self.sound:play()
                end

                -- Reset cooldown timer
                self.cooldown_timer = 0.0

                -- Break after first impact to avoid multiple sounds per frame
                break
            end
        end
    end
end