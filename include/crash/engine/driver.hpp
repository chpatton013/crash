#pragma once

#include <memory>
#include <set>
#include <boost/timer/timer.hpp>
#include <crash/render/camera.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/space/bounding_partition.hpp>
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
   static render::MeshInstancePtr BoundingCubeMeshInstance;

   Driver(const Driver& driver);
   Driver(const space::BoundingPartitionPtr& boundingPartition,
    const render::CameraPtr& camera,
    const render::LightManagerPtr& lightManager,
    const window::WindowPtr& window);
   virtual ~Driver();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   space::BoundingPartitionPtr getBoundingPartition();
   void setBoundingPartition(
    const space::BoundingPartitionPtr& boundingPartition);

   render::CameraPtr getCamera();
   void setCamera(const render::CameraPtr& camera);

   window::WindowPtr getWindow();
   void setWindow(const window::WindowPtr& window);

   bool getShouldLoop() const;
   void setShouldLoop(bool shouldLoop);

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

   space::BoundingPartitionPtr _boundingPartition;
   render::CameraPtr _camera;
   render::LightManagerPtr _lightManager;
   window::WindowPtr _window;
   bool _shouldLoop;
   bool _renderBoundingGroups;
   bool _renderBoundingPartition;
   boost::timer::cpu_timer _updateTimer;
   boost::timer::cpu_timer _renderTimer;
   float _updatesPerSecond;
   float _rendersPerSecond;
};

} // namespace engine
} // namespace crash
