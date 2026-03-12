-- PlayerController.lua
-- Simple first-person controller: WASD movement, Space jump, Ctrl crouch.
--
-- Bindings match FirstPersonController (source/FirstPersonController.h):
--   forward_key  = Input.KEY_W   (move_forward)
--   backward_key = Input.KEY_S   (move_backward)
--   left_key     = Input.KEY_A   (move_left)
--   right_key    = Input.KEY_D   (move_right)
--   jump_key     = Input.KEY_SPACE
--   crouch_key   = Input.KEY_ANY_CTRL
-- Uses UNIGINE Input API: isKeyPressed (held), isKeyDown (held), getMouseDX/DY, isMouseGrab.

component("PlayerController", {
    description = "Simple first-person controller (movement, jump, crouch)",

    on_init = "player_init",
    on_update = "player_update",

    properties = {
        move_speed = { type = "float", default = 5.0, min = 0, max = 20, title = "Move Speed" },
        jump_power = { type = "float", default = 6.0, min = 0, max = 20, title = "Jump Power" },
        crouch_speed = { type = "float", default = 2.5, min = 0, max = 10, title = "Crouch Speed" },
        mouse_sensitivity = { type = "float", default = 0.25, min = 0.01, max = 2.0, title = "Mouse Sensitivity" },
        camera_height = { type = "float", default = 1.65, min = 0.5, max = 3.0, title = "Camera Height" },
        crouch_height = { type = "float", default = 1.0, min = 0.3, max = 1.5, title = "Crouch Height" },
    },
})

local function n(v) return (type(v) == "number") and v or 0 end

function player_init(self)
    if not self or not self.node then return end

    local node = self.node
    local obj = Object.cast(node)
    if not obj then
        if Log and Log.message then Log.message("[PlayerController] Node is not an Object\n") end
        return
    end

    -- Align player: Y = forward, Z = up
    local wt = obj:getWorldTransform()

    if wt then
        local axis_y = wt:getAxisY()
        axis_y.z = 0
        local len2 = axis_y:length2()
        if len2 and len2 > 0 and Math and Math.EPSILON and len2 > Math.EPSILON then
            axis_y = axis_y:normalized()
        else
            axis_y = Vec3.new(0, 1, 0)
        end
        local pos = wt:getTranslate()
        obj:setWorldTransform(Mat4.setTo(pos, pos + axis_y, Vec3.UP, Mat4.AXIS_Y))
    end

    -- Physics body (dummy, we move it manually)
    local body = BodyDummy.create(obj)
    if not body then return end
    body:setTransform(obj:getWorldTransform())
    if body.setEnabled then body:setEnabled(true) end

    -- Capsule shape (radius, height; center at origin, we offset so bottom sits on ground)
    local radius = 0.35
    local height = 1.8
    local shape = ShapeCapsule.create(body, radius, height)
    if not shape then return end
    if shape.setEnabled then shape:setEnabled(true) end
    local num_shapes = body:getNumShapes()
    if num_shapes > 0 and body.setShapeTransform then
        local up_off = Vec3.new(0, 0, radius + 0.5 * height)
        body:setShapeTransform(num_shapes - 1, Mat4.translate(up_off))
    end
    shape:setCollisionMask(0xFFFFFFFF)
    shape:setPhysicsIntersectionMask(0xFFFFFFFF)

    -- First-person camera
    local camera = PlayerDummy.create()
    if camera then
        camera:setParent(obj)
        camera:setFov(60.0)
        camera:setZNear(0.01)
        camera:setZFar(20000.0)
        local cwt = obj:getWorldTransform()
        local cam_offset = Vec3.new(0, 0, n(self.camera_height) > 0 and self.camera_height or 1.65)
        camera:setWorldPosition(cwt * cam_offset)
        camera:setWorldDirection(cwt:getAxisY(), Vec3.UP)
        camera:setMainPlayer(true)
    end

    -- State
    self.obj = obj
    self.node = node
    self.body = body
    self.shape = shape
    self.camera = camera
    self.radius = radius
    self.stand_height = height
    self.crouch_h = math.max(0.5, math.min(n(self.crouch_height), height - 0.2))

    self.yaw = 0.0
    self.pitch = 0.0
    self.velocity_x = 0.0
    self.velocity_y = 0.0
    self.velocity_z = 0.0
    self.on_ground = false
    self.is_crouching = false
    self.current_height = height

    -- Register InputActions to match FirstPersonController keys (optional; raw Input used as fallback)
    if not _G._player_controller_input_registered and InputActions then
        InputActions.register("move_forward",  { Input.KEY_W }, {}, {})
        InputActions.register("move_backward", { Input.KEY_S }, {}, {})
        InputActions.register("move_left",     { Input.KEY_A }, {}, {})
        InputActions.register("move_right",    { Input.KEY_D }, {}, {})
        InputActions.register("jump",          { Input.KEY_SPACE }, {}, {})
        InputActions.register("crouch",        { Input.KEY_ANY_CTRL }, {}, {})
        _G._player_controller_input_registered = true
    end

    self.is_initialized = true
    if Input and Input.setMouseGrab then Input.setMouseGrab(true) end
    if Log and Log.message then Log.message("[PlayerController] Initialized (simple)\n") end
end

function player_update(self, dt)
    if not self or not self.is_initialized or not self.body or not self.shape then return end

    local ifps = (type(dt) == "number" and dt > 0) and dt or (Game and Game.getIFps and Game.getIFps())
    if type(ifps) ~= "number" or ifps <= 0 then ifps = 1.0 / 60.0 end
    local scale = (Physics and Physics.getScale and Physics.getScale()) or 1.0
    ifps = ifps * scale

    local wt = self.node:getWorldTransform()
    if not wt then return end

    -- Grab mouse so WASD and mouse look work (engine may not grab by default in editor/game)
    if self.camera and Input and Input.setMouseGrab then
        Input.setMouseGrab(true)
    end
    local mouse_active = Input and Input.isMouseGrab and Input.isMouseGrab()

    -- Keyboard: read movement regardless of mouse grab (so WASD works even before click)
    local move_x, move_y = 0.0, 0.0
    local use_actions = InputActions and InputActions.isPressed
    if use_actions then
        if InputActions.isPressed("move_forward") then move_y = move_y + 1 end
        if InputActions.isPressed("move_backward") then move_y = move_y - 1 end
        if InputActions.isPressed("move_left") then move_x = move_x - 1 end
        if InputActions.isPressed("move_right") then move_x = move_x + 1 end
    end
    if move_x == 0 and move_y == 0 and Input and Input.isKeyPressed then
        local W, S, A, D = Input.KEY_W, Input.KEY_S, Input.KEY_A, Input.KEY_D
        if W and Input.isKeyPressed(W) then move_y = move_y + 1 end
        if S and Input.isKeyPressed(S) then move_y = move_y - 1 end
        if A and Input.isKeyPressed(A) then move_x = move_x - 1 end
        if D and Input.isKeyPressed(D) then move_x = move_x + 1 end
    end

    -- Normalize horizontal input
    local len = math.sqrt(move_x * move_x + move_y * move_y)
    if len > 0 then
        move_x = move_x / len
        move_y = move_y / len
    end

    -- Mouse look (only when grab is active so we don't spin when clicking UI)
    if mouse_active and Input.getMouseDX and Input.getMouseDY then
        local dx = Input.getMouseDX()
        local dy = Input.getMouseDY()
        self.yaw = self.yaw - dx * (self.mouse_sensitivity or 0.25)
        self.pitch = self.pitch - dy * (self.mouse_sensitivity or 0.25)
        if self.pitch > 89 then self.pitch = 89 end
        if self.pitch < -89 then self.pitch = -89 end
    end

    -- Crouch: crouch_key = KEY_ANY_CTRL (FirstPersonController.h)
    local want_crouch = (InputActions and InputActions.isPressed and InputActions.isPressed("crouch"))
        or (Input and Input.KEY_ANY_CTRL and Input.isKeyPressed and Input.isKeyPressed(Input.KEY_ANY_CTRL))
    if want_crouch then
        self.is_crouching = true
        self.current_height = self.crouch_h
    else
        -- Try to stand (check head clearance with a slightly taller shape would be ideal; for simplicity we just stand)
        self.is_crouching = false
        self.current_height = self.stand_height
    end
    self.shape:setHeight(self.current_height)
    if self.body and self.body.setShapeTransform then
        local num_shapes = self.body:getNumShapes()
        if num_shapes > 0 then
            local up_off = Vec3.new(0, 0, self.radius + 0.5 * self.current_height)
            self.body:setShapeTransform(num_shapes - 1, Mat4.translate(up_off))
        end
    end

    -- Jump: jump_key = KEY_SPACE (one-shot per press when grounded)
    local jump_trigger = false
    if self.on_ground then
        if InputActions and InputActions.isDown and InputActions.isDown("jump") then
            jump_trigger = true
        elseif Input and Input.KEY_SPACE and Input.isKeyDown and Input.isKeyDown(Input.KEY_SPACE) then
            if not self._jump_key_held then jump_trigger = true end
            self._jump_key_held = true
        else
            self._jump_key_held = false
        end
    end
    if jump_trigger then
        self.velocity_z = self.jump_power or 6.0
        self.on_ground = false
    end

    -- Gravity
    if not self.on_ground then
        local g = (Physics and Physics.getGravity and Physics.getGravity()) or Vec3.new(0, 0, -9.8)
        self.velocity_z = self.velocity_z + (g.z or -9.8) * ifps
    else
        self.velocity_z = 0.0
    end

    -- Movement speed (crouch vs walk)
    local speed = self.is_crouching and (self.crouch_speed or 2.5) or (self.move_speed or 5.0)
    local yaw_rad = self.yaw * (Math and Math.DEG2RAD or 0.0174533)
    local cos_y = math.cos(yaw_rad)
    local sin_y = math.sin(yaw_rad)
    -- World-space horizontal direction from input
    local world_dx = -sin_y * move_x + cos_y * move_y
    local world_dy =  cos_y * move_x + sin_y * move_y
    self.velocity_x = world_dx * speed
    self.velocity_y = world_dy * speed

    -- Apply movement
    local pos = wt:getTranslate()
    pos.x = pos.x + self.velocity_x * ifps
    pos.y = pos.y + self.velocity_y * ifps
    pos.z = pos.z + self.velocity_z * ifps

    -- Rotation matrix from yaw (around Z)
    wt:setRotate(Vec3.UP, self.yaw)
    wt:setColumn3(3, pos)
    self.node:setWorldTransform(wt)
    self.body:setTransform(wt)

    -- Resolve collisions and detect ground
    self.on_ground = false
    local eps = Math and Math.EPSILON or 1e-6
    local cos_ground = math.cos(60 * 0.0174533)
    for _ = 1, 3 do
        self.body:setTransform(wt)
        if self.is_crouching and self.shape.setTransform then
            local up_off = Vec3.new(0, 0, self.radius + 0.5 * self.shape:getHeight())
            self.shape:setTransform(wt * Mat4.translate(up_off))
        end
        local contacts = self.shape:getCollision()
        if not contacts or #contacts == 0 then break end
        local push = Vec3.new(0, 0, 0)
        for i = 1, math.min(8, #contacts) do
            local c = contacts[i]
            if c then
                local norm = c:getNormal()
                local depth = c:getDepth()
                push.x = push.x + norm.x * (depth - eps)
                push.y = push.y + norm.y * (depth - eps)
                push.z = push.z + norm.z * (depth - eps)
                if norm.z > cos_ground then
                    local pt = c:getPoint()
                    if pt.z <= pos.z + self.radius + 0.5 * self.current_height + 0.1 then
                        self.on_ground = true
                        if self.velocity_z < 0 then self.velocity_z = 0 end
                    end
                end
                if norm.z < -0.5 then
                    if self.velocity_z > 0 then self.velocity_z = 0 end
                end
            end
        end
        pos.x = pos.x + push.x
        pos.y = pos.y + push.y
        pos.z = pos.z + push.z
        wt:setColumn3(3, pos)
        wt:setRotate(Vec3.UP, self.yaw)
        self.node:setWorldTransform(wt)
        self.body:setTransform(wt)
    end

    -- Camera
    if self.camera and self.camera.setWorldPosition and self.camera.setWorldDirection then
        local cam_z = (self.camera_height or 1.65)
        if self.is_crouching then cam_z = self.crouch_h * 0.7 end
        local cam_pos = wt * Vec3.new(0, 0, cam_z)
        self.camera:setWorldPosition(cam_pos)
        local pitch_rad = self.pitch * (Math and Math.DEG2RAD or 0.0174533)
        local dir = Vec3.new(
            -math.sin(self.yaw * 0.0174533) * math.cos(pitch_rad),
            math.cos(self.yaw * 0.0174533) * math.cos(pitch_rad),
            math.sin(pitch_rad)
        )
        self.camera:setWorldDirection(dir:normalized(), Vec3.UP)
    end
end
