#pragma once

#include <memory>
#include <set>
#include <boost/timer/timer.hpp>
#include <crash/engine/camera.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_partition.hpp>
#include <crash/space/collision.hpp>
#include <crash/window/window.hpp>

namespace crash {

namespace space {
   class BoundingPartition;
}

namespace window {
   class Window;
}

namespace engine {

class Driver;
typedef std::shared_ptr< Driver > DriverPtr;

class Driver {
public:
   typedef void (*CollisionCallback)(const space::Collision& collision);

   static render::MeshInstancePtr BoundingCubeMeshInstance;

   Driver(const Driver& driver);
   Driver(space::BoundingPartition* boundingPartition,
    engine::Camera* camera,
    render::LightManager* lightManager,
    window::Window* window);
   virtual ~Driver();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   space::BoundingPartition* getBoundingPartition() const;
   void setBoundingPartition(space::BoundingPartition* boundingPartition);

   engine::Camera* getCamera() const;
   void setCamera(engine::Camera* camera);

   window::Window* getWindow() const;
   void setWindow(window::Window* window);

   const std::set< CollisionCallback >& getCollisionCallbacks() const;
   void addCollisionCallback(CollisionCallback callback);
   void removeCollisionCallback(CollisionCallback callback);
   void clearCollisionCallbacks();

   bool getShouldLoop() const;
   void setShouldLoop(bool shouldLoop);

   bool getRenderBoundingBoxes() const;
   void setRenderBoundingBoxes(bool renderBoundingBoxes);

   bool getRenderBoundingGroups() const;
   void setRenderBoundingGroups(bool renderBoundingGroups);

   bool getRenderBoundingPartition() const;
   void setRenderBoundingPartition(bool renderBoundingPartition);

   /////////////////////////////////////////////////////////////////////////////
   // Driver.
   /////////////////////////////////////////////////////////////////////////////

   void loop(float updateInterval, float renderInterval);
   float getUpdatesPerSecond() const;
   float getRendersPerSecond() const;

private:
   /////////////////////////////////////////////////////////////////////////////
   // Helpers.
   /////////////////////////////////////////////////////////////////////////////

   void update(float delta_t);
   void render(float delta_t) const;

   float getUpdateTimerElapsed() const;
   float getRenderTimerElapsed() const;
   float getTimerElapsed(const boost::timer::cpu_timer& timer) const;

   void resetUpdateTimer();
   void resetRenderTimer();

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   space::BoundingPartition* _boundingPartition;
   engine::Camera* _camera;
   render::LightManager* _lightManager;
   window::Window* _window;
   std::set< CollisionCallback > _collisionCallbacks;
   bool _shouldLoop;
   bool _renderBoundingBoxes;
   bool _renderBoundingGroups;
   bool _renderBoundingPartition;
   boost::timer::cpu_timer _updateTimer;
   boost::timer::cpu_timer _renderTimer;
   float _updatesPerSecond;
   float _rendersPerSecond;
};

} // namespace engine
} // namespace crash
