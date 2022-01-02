#ifndef BABYLON_MATERIALS_IMATERIAL_DEFINES_H
#define BABYLON_MATERIALS_IMATERIAL_DEFINES_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct MaterialDefines;

struct BABYLON_SHARED_EXPORT IMaterialDefines {
  virtual ~IMaterialDefines()                                            = default;
  [[nodiscard]] virtual bool isDirty() const                             = 0;
  virtual void markAsProcessed()                                         = 0;
  virtual void markAsUnprocessed()                                       = 0;
  virtual void markAllAsDirty()                                          = 0;
  virtual void markAsImageProcessingDirty()                              = 0;
  virtual void markAsLightDirty(bool disposed = false)                   = 0;
  virtual void markAsAttributesDirty()                                   = 0;
  virtual void markAsTexturesDirty()                                     = 0;
  virtual void markAsFresnelDirty()                                      = 0;
  virtual void markAsMiscDirty()                                         = 0;
  virtual void markAsPrePassDirty()                                      = 0;
  virtual void rebuild()                                                 = 0;
  [[nodiscard]] virtual bool isEqual(const MaterialDefines& other) const = 0;
  virtual void cloneTo(MaterialDefines& other)                           = 0;
  virtual void reset()                                                   = 0;
  [[nodiscard]] virtual std::string toString() const                     = 0;
}; // end of struct IMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_IMATERIALS_MATERIAL_DEFINES_H
