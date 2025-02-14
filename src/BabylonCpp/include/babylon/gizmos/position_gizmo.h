#ifndef BABYLON_GIZMOS_POSITION_GIZMO_H
#define BABYLON_GIZMOS_POSITION_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class AxisDragGizmo;
class GizmoManager;
class PlaneDragGizmo;
FWD_CLASS_SPTR(UtilityLayerRenderer)

/**
 * @brief Gizmo that enables dragging a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT PositionGizmo : public Gizmo {

public:
  /**
   * @brief Creates a PositionGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param thickness display gizmo axis thickness
   */
  PositionGizmo(const UtilityLayerRendererPtr& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer(),
                float thickness = 1.f, GizmoManager* gizmoManager = nullptr);
  ~PositionGizmo() override; // = default

  /**
   * @brief Builds Gizmo Axis Cache to enable features such as hover state preservation and graying
   * out other axis during manipulation.
   * @param mesh Axis gizmo mesh
   * @param cache Gizmo axis definition used for reactive gizmo UI
   */
  void addToAxisCache(Mesh* mesh, const GizmoAxisCache& cache);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Disposes and replaces the current meshes in the gizmo with the specified mesh.
   * @param mesh The mesh to replace the default mesh of the gizmo
   * @param useGizmoMaterial If the gizmo's default material should be used (default: false)
   */
  void setCustomMesh(const MeshPtr& mesh, bool useGizmoMaterial = false) override;

protected:
  AbstractMeshPtr& get_attachedMesh() override;
  void set_attachedMesh(const AbstractMeshPtr& mesh) override;
  NodePtr& get_attachedNode() override;
  void set_attachedNode(const NodePtr& node) override;

  /**
   * @brief Returns True when the mouse pointer is hovered a gizmo mesh.
   */
  bool get_isHovered() const override;

  /**
   * @brief Gets if the planar drag gizmo is enabled.
   */
  [[nodiscard]] bool get_planarGizmoEnabled() const;

  /**
   * @brief Sets if the planar drag gizmo is enabled.
   * setting this will enable/disable XY, XZ and YZ planes regardless of individual gizmo settings.
   */
  void set_planarGizmoEnabled(bool value);

  void set_updateGizmoRotationToMatchAttachedMesh(bool value) override;
  [[nodiscard]] bool get_updateGizmoRotationToMatchAttachedMesh() const override;
  void set_snapDistance(float value);
  [[nodiscard]] float get_snapDistance() const;
  void set_scaleRatio(float value) override;
  [[nodiscard]] float get_scaleRatio() const override;

public:
  /**
   * Internal gizmo used for interactions on the x axis
   */
  std::unique_ptr<AxisDragGizmo> xGizmo;

  /**
   * Internal gizmo used for interactions on the y axis
   */
  std::unique_ptr<AxisDragGizmo> yGizmo;

  /**
   * Internal gizmo used for interactions on the z axis
   */
  std::unique_ptr<AxisDragGizmo> zGizmo;

  /**
   * Internal gizmo used for interactions on the yz plane
   */
  std::unique_ptr<PlaneDragGizmo> xPlaneGizmo;

  /**
   * Internal gizmo used for interactions on the xz plane
   */
  std::unique_ptr<PlaneDragGizmo> yPlaneGizmo;

  /**
   * Internal gizmo used for interactions on the xy plane
   */
  std::unique_ptr<PlaneDragGizmo> zPlaneGizmo;

  /**
   * Fires an event when any of it's sub gizmos are dragged
   */
  Observable<DragStartOrEndEvent> onDragStartObservable;

  /**
   * Fires an event when any of it's sub gizmos are released from dragging
   */
  Observable<DragStartOrEndEvent> onDragEndObservable;

  /**
   * If the planar drag gizmo is enabled
   * setting this will enable/disable XY, XZ and YZ planes regardless of individual gizmo settings.
   */
  Property<PositionGizmo, bool> planarGizmoEnabled;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged (Default: 0)
   */
  Property<PositionGizmo, float> snapDistance;

private:
  /**
   * private variables
   */
  AbstractMeshPtr _meshAttached;
  NodePtr _nodeAttached;
  float _snapDistance;
  std::vector<Observer<PointerInfo>::Ptr> _observables;

  /** Node Caching for quick lookup */
  std::unordered_map<Mesh*, GizmoAxisCache> _gizmoAxisCache;

  /**
   * If set to true, planar drag is enabled
   */
  bool _planarGizmoEnabled;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_POSITION_GIZMO_H
