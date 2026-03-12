-- RotatingCubeDebug.lua
component("RotatingCube", {
    description = "Rotating cube with debug logging",
    
    on_init = "rotate_init",
    on_update = "rotate_update",
    
    properties = {
        rotation_speed = { type = "float", default = 45.0 },
        use_world_transform = { type = "bool", default = false }
    }
})

function rotate_init(self)
    self.total_rotation = 0.0
    self.frame_count = 0
    
    local pos = self.node:getWorldPosition()
    Log.message("[Debug] Node initialized at position: %.2f, %.2f, %.2f\n", 
        pos.x, pos.y, pos.z)
end

function rotate_update(self, dt)
    self.frame_count = self.frame_count + 1
    
    -- Increment rotation
    self.total_rotation = self.total_rotation + (self.rotation_speed * dt)
    
    -- Log every 60 frames (roughly once per second at 60fps)
    if self.frame_count % 60 == 0 then
        Log.message("[Debug] Rotation: %.2f degrees, dt: %.4f\n", 
            self.total_rotation, dt)
    end
    
    -- Create rotation quaternion around Z axis
    local rotation_quat = Quat.new(vec3.UP, self.total_rotation)
    
    if self.use_world_transform then
        self.node:setWorldRotation(rotation_quat)
    else
        self.node:setRotation(rotation_quat)
    end
end