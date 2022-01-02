#ifndef BABYLON_MESHES_BUFFER_H
#define BABYLON_MESHES_BUFFER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Mesh;
class ThinEngine;
class VertexBuffer;
FWD_CLASS_SPTR(WebGLDataBuffer)

/**
 * @brief Class used to store data that will be store in GPU memory.
 */
class BABYLON_SHARED_EXPORT Buffer : public std::enable_shared_from_this<Buffer> {

public:
  /**
   * @brief Constructor
   * @param engine the engine
   * @param data the data to use for this buffer
   * @param updatable whether the data is updatable
   * @param stride the stride (optional)
   * @param postponeInternalCreation whether to postpone creating the internal WebGL buffer
   * (optional)
   * @param instanced whether the buffer is instanced (optional)
   * @param useBytes set to true if the stride in in bytes (optional)
   * @param divisor sets an optional divisor for instances (1 by default)
   */
  Buffer(ThinEngine* engine, const Float32Array& data, bool updatable,
         std::optional<size_t> stride = std::nullopt, bool postponeInternalCreation = false,
         bool instanced = false, bool useBytes = false,
         const std::optional<unsigned int>& divisor = std::nullopt);

  /**
   * @brief Constructor
   * @param engine the engine
   * @param data the data to use for this buffer
   * @param updatable whether the data is updatable
   * @param stride the stride (optional)
   * @param postponeInternalCreation whether to postpone creating the internal WebGL buffer
   * (optional)
   * @param instanced whether the buffer is instanced (optional)
   * @param useBytes set to true if the stride in in bytes (optional)
   * @param divisor sets an optional divisor for instances (1 by default)
   */
  Buffer(Mesh* mesh, const Float32Array& data, bool updatable,
         std::optional<size_t> stride = std::nullopt, bool postponeInternalCreation = false,
         bool instanced = false, bool useBytes = false,
         const std::optional<unsigned int>& divisor = std::nullopt);

  virtual ~Buffer(); // = default

  /**
   * @brief Create a new VertexBuffer based on the current buffer.
   * @param kind defines the vertex buffer kind (position, normal, etc.)
   * @param offset defines offset in the buffer (0 by default)
   * @param size defines the size in floats of attributes (position is 3 for instance)
   * @param stride defines the stride size in floats in the buffer (the offset to apply to reach
   * next value when data is interleaved)
   * @param instanced defines if the vertex buffer contains indexed data
   * @param useBytes defines if the offset and stride are in bytes     *
   * @param divisor sets an optional divisor for instances (1 by default)
   * @returns the new vertex buffer
   */
  std::unique_ptr<VertexBuffer>
  createVertexBuffer(const std::string& kind, size_t offset, size_t size,
                     std::optional<size_t> stride  = std::nullopt,
                     std::optional<bool> instanced = std::nullopt, bool useBytes = false,
                     const std::optional<unsigned int>& divisor = std::nullopt);

  // Properties

  /**
   * @brief Gets a boolean indicating if the Buffer is updatable?
   * @returns true if the buffer is updatable
   */
  bool isUpdatable() const;

  /**
   * @brief Gets current buffer's data
   * @returns a DataArray or null
   */
  Float32Array& getData();

  /**
   * @brief Gets underlying native buffer
   * @returns underlying native buffer
   */
  WebGLDataBufferPtr& getBuffer();

  /**
   * @brief Gets the stride in float32 units (i.e. byte stride / 4).
   * May not be an integer if the byte stride is not divisible by 4.
   * @returns the stride in float32 units
   * @deprecated Please use byteStride instead.
   */
  size_t getStrideSize() const;

  // Methods

  /**
   * @brief Store data into the buffer. If the buffer was already used it will be either recreated
   * or updated depending on isUpdatable property
   * @param data defines the data to store
   */
  WebGLDataBufferPtr create(Float32Array data = {});

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Update current buffer data
   * @param data defines the data to store
   */
  WebGLDataBufferPtr update(const Float32Array& data);

  /**
   * @brief Updates the data directly.
   * @param data the new data
   * @param offset the new offset
   * @param vertexCount the vertex count (optional)
   * @param useBytes set to true if the offset is in bytes
   */
  WebGLDataBufferPtr updateDirectly(const Float32Array& data, size_t offset,
                                    const std::optional<size_t>& vertexCount = std::nullopt,
                                    bool useBytes                            = false);

  /**
   * @brief Hidden
   */
  void _increaseReferences();

  /**
   * @brief Release all resources
   */
  void dispose();

public:
  /**
   * Hidden
   */
  Float32Array _data;

  /**
   * Gets the byte stride
   */
  size_t byteStride;

private:
  ThinEngine* _engine;
  WebGLDataBufferPtr _buffer;
  bool _updatable;
  bool _instanced;
  unsigned int _divisor;
  bool _isAlreadyOwned;

}; // end of class Buffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUFFER_H
