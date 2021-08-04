#include <babylon/materials/uniform_buffer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector4.h>

namespace BABYLON {

std::unordered_map<std::string, unsigned int> UniformBuffer::_updatedUbosInFrame = {};
Float32Array UniformBuffer::_tempBuffer = Float32Array(UniformBuffer::_MAX_UNIFORM_SIZE);

UniformBuffer::UniformBuffer(ThinEngine* engine, const Float32Array& data,
                             const std::optional<bool>& dynamic, const std::string& name)
    : _alreadyBound{false}
    , useUbo{this, &UniformBuffer::get_useUbo}
    , isSync{this, &UniformBuffer::get_isSync}
    , _numBuffers{this, &UniformBuffer::get__numBuffers}
    , _indexBuffer{this, &UniformBuffer::get__indexBuffer}
    , name{this, &UniformBuffer::get_name}
    , _uniformLocationPointer{0}
    , _needSync{false}
{
  _engine  = engine;
  _noUBO   = !engine->supportsUniformBuffers();
  _dynamic = dynamic.value_or(false);
  _name    = !name.empty() ? name : "no-name";

  _data = data;

  _uniformLocations       = {};
  _uniformSizes           = {};
  _uniformArraySizes      = {};
  _uniformLocationPointer = 0;
  _needSync               = false;

  if (_engine->_features.trackUbosInFrame) {
    _buffers             = {};
    _bufferIndex         = -1;
    _createBufferOnWrite = false;
    _currentFrameId      = 0;
  }

  if (_noUBO) {
    updateMatrix3x3 = [this](const std::string& name, const Float32Array& matrix) -> void {
      _updateMatrix3x3ForEffect(name, matrix);
    };
    updateMatrix2x2 = [this](const std::string& name, const Float32Array& matrix) -> void {
      _updateMatrix2x2ForEffect(name, matrix);
    };
    updateFloat = [this](const std::string& name, float x) { _updateFloatForEffect(name, x); };
    updateFloat2
      = [this](const std::string& name, float x, float y, const std::string& suffix) -> void {
      _updateFloat2ForEffect(name, x, y, suffix);
    };
    updateFloat3 = [this](const std::string& name, float x, float y, float z,
                          const std::string& suffix = "") -> void {
      _updateFloat3ForEffect(name, x, y, z, suffix);
    };
    updateFloat4 = [this](const std::string& name, float x, float y, float z, float w,
                          const std::string& suffix = "") -> void {
      _updateFloat4ForEffect(name, x, y, z, w, suffix);
    };
    updateFloatArray = [this](const std::string& name, const Float32Array& array) -> void {
      _updateFloatArrayForEffect(name, array);
    };
    updateArray = [this](const std::string& name, const Float32Array& array) -> void {
      _updateArrayForEffect(name, array);
    };
    updateIntArray = [this](const std::string& name, const Int32Array& array) -> void {
      _updateIntArrayForEffect(name, array);
    };
    updateMatrix = [this](const std::string& name, const Matrix& mat) -> void {
      _updateMatrixForEffect(name, mat);
    };
    updateMatrices = [this](const std::string& name, const Float32Array& mat) -> void {
      _updateMatricesForEffect(name, mat);
    };
    updateVector3 = [this](const std::string& name, const Vector3& vector) -> void {
      _updateVector3ForEffect(name, vector);
    };
    updateVector4 = [this](const std::string& name, const Vector4& vector) -> void {
      _updateVector4ForEffect(name, vector);
    };
    updateColor3 = [this](const std::string& name, const Color3& color,
                          const std::string& suffix = "") -> void {
      _updateColor3ForEffect(name, color, suffix);
    };
    updateColor4 = [this](const std::string& name, const Color3& color, float alpha,
                          const std::string& suffix = "") -> void {
      _updateColor4ForEffect(name, color, alpha, suffix);
    };
    updateDirectColor4 = [this](const std::string& name, const Color4& color,
                                const std::string& suffix = "") -> void {
      _updateDirectColor4ForEffect(name, color, suffix);
    };
    updateInt = [this](const std::string& name, int x, const std::string& suffix = "") -> void {
      _updateIntForEffect(name, x, suffix);
    };
    updateInt2
      = [this](const std::string& name, int x, int y, const std::string& suffix = "") -> void {
      _updateInt2ForEffect(name, x, y, suffix);
    };
    updateInt3 = [this](const std::string& name, int x, int y, int z,
                        const std::string& suffix = "") -> void {
      _updateInt3ForEffect(name, x, y, z, suffix);
    };
    updateInt4 = [this](const std::string& name, int x, int y, int z, int w,
                        const std::string& suffix = "") -> void {
      _updateInt4ForEffect(name, x, y, z, w, suffix);
    };
  }
  else {
    _engine->_uniformBuffers.emplace_back(this);

    updateMatrix3x3 = [this](const std::string& name, const Float32Array& matrix) -> void {
      _updateMatrix3x3ForUniform(name, matrix);
    };
    updateMatrix2x2 = [this](const std::string& name, const Float32Array& matrix) -> void {
      _updateMatrix2x2ForUniform(name, matrix);
    };
    updateFloat = [this](const std::string& name, float x) { _updateFloatForUniform(name, x); };
    updateFloat2
      = [this](const std::string& name, float x, float y,
               const std::string& /*suffix*/ = "") -> void { _updateFloat2ForUniform(name, x, y); };
    updateFloat3 = [this](const std::string& name, float x, float y, float z,
                          const std::string& /*suffix*/ = "") -> void {
      _updateFloat3ForUniform(name, x, y, z);
    };
    updateFloat4 = [this](const std::string& name, float x, float y, float z, float w,
                          const std::string& /*suffix*/ = "") -> void {
      _updateFloat4ForUniform(name, x, y, z, w);
    };
    updateFloatArray = [this](const std::string& name, const Float32Array& array) -> void {
      _updateFloatArrayForUniform(name, array);
    };
    updateArray = [this](const std::string& name, const Float32Array& array) -> void {
      _updateArrayForUniform(name, array);
    };
    updateIntArray = [this](const std::string& name, const Int32Array& array) -> void {
      _updateIntArrayForUniform(name, array);
    };
    updateMatrix = [this](const std::string& name, const Matrix& mat) -> void {
      _updateMatrixForUniform(name, mat);
    };
    updateMatrices = [this](const std::string& name, const Float32Array& mat) -> void {
      _updateMatricesForUniform(name, mat);
    };
    updateVector3 = [this](const std::string& name, const Vector3& vector) -> void {
      _updateVector3ForUniform(name, vector);
    };
    updateVector4 = [this](const std::string& name, const Vector4& vector) -> void {
      _updateVector4ForUniform(name, vector);
    };
    updateColor3 = [this](const std::string& name, const Color3& color,
                          const std::string& /*suffix*/ = "") -> void {
      _updateColor3ForUniform(name, color);
    };
    updateColor4 = [this](const std::string& name, const Color3& color, float alpha,
                          const std::string& /*suffix*/ = "") -> void {
      _updateColor4ForUniform(name, color, alpha);
    };
    updateDirectColor4 = [this](const std::string& name, const Color4& color,
                                const std::string& /*suffix*/ = "") -> void {
      _updateDirectColor4ForUniform(name, color);
    };
    updateInt = [this](const std::string& name, int x, const std::string& /*suffix*/ = "") -> void {
      _updateIntForUniform(name, x);
    };
    updateInt2
      = [this](const std::string& name, int x, int y, const std::string& /*suffix*/ = "") -> void {
      _updateInt2ForUniform(name, x, y);
    };
    updateInt3 = [this](const std::string& name, int x, int y, int z,
                        const std::string& /*suffix*/ = "") -> void {
      _updateInt3ForUniform(name, x, y, z);
    };
    updateInt4 = [this](const std::string& name, int x, int y, int z, int w,
                        const std::string& /*suffix*/ = "") -> void {
      _updateInt4ForUniform(name, x, y, z, w);
    };
  }
}

UniformBuffer::~UniformBuffer() = default;

bool UniformBuffer::get_useUbo() const
{
  return !_noUBO;
}

bool UniformBuffer::get_isSync() const
{
  return !_needSync;
}

bool UniformBuffer::isDynamic() const
{
  return _dynamic;
}

Float32Array& UniformBuffer::getData()
{
  return _bufferData;
}

WebGLDataBufferPtr& UniformBuffer::getBuffer()
{
  return _buffer;
}

void UniformBuffer::_fillAlignment(size_t size)
{
  // This code has been simplified because we only use floats, vectors of 1, 2, 3, 4 components
  // and 4x4 matrices
  // TODO : change if other types are used

  size_t alignment = 0;
  if (size <= 2) {
    alignment = size;
  }
  else {
    alignment = 4;
  }

  if ((_uniformLocationPointer % alignment) != 0) {
    size_t oldPointer = _uniformLocationPointer;
    _uniformLocationPointer += alignment - (_uniformLocationPointer % alignment);
    size_t diff = _uniformLocationPointer - oldPointer;

    for (size_t i = 0; i < diff; ++i) {
      _data.emplace_back(0.f);
    }
  }
}

void UniformBuffer::addUniform(const std::string& iName,
                               const std::variant<int, Float32Array>& size, size_t arraySize)
{
  if (_noUBO) {
    return;
  }

  if (stl_util::contains(_uniformLocations, iName)) {
    // Already existing uniform
    return;
  }

  // This function must be called in the order of the shader layout !
  // size can be the size of the uniform, or data directly
  Float32Array data;
  size_t _size = 0;
  // std140 FTW...
  if (arraySize > 0) {
    if (std::holds_alternative<Float32Array>(size)) {
      throw std::runtime_error("addUniform should not be use with Array in UBO: " + iName);
    }

    _fillAlignment(4);

    _size = static_cast<size_t>(std::get<int>(size));

    _uniformArraySizes[iName] = ArraySizes{
      _size,    // strideSize
      arraySize // arraySize
    };
    if (_size == 16) {
      _size = _size * arraySize;
    }
    else {
      const auto perElementPadding = 4 - _size;
      const auto totalPadding      = perElementPadding * arraySize;
      _size                        = _size * arraySize + totalPadding;
    }

    data = {};
    // Fill with zeros
    for (size_t i = 0; i < _size; i++) {
      data.emplace_back(0.f);
    }
  }
  else {
    if (std::holds_alternative<Float32Array>(size)) {
      data  = std::get<Float32Array>(size);
      _size = data.size();
    }
    else {
      _size = static_cast<size_t>(std::get<int>(size));

      // Fill with zeros
      data = Float32Array(_size, 0.f);
    }

    _fillAlignment(_size);
  }

  _uniformSizes[iName]     = _size;
  _uniformLocations[iName] = _uniformLocationPointer;
  _uniformLocationPointer += _size;

  for (size_t i = 0; i < _size; ++i) {
    _data.emplace_back(data[i]);
  }

  _needSync = true;
}

void UniformBuffer::addMatrix(const std::string& iName, const Matrix& mat)
{
  addUniform(iName, mat.toArray());
}

void UniformBuffer::addFloat2(const std::string& iName, float x, float y)
{
  Float32Array temp{x, y};
  addUniform(iName, temp);
}

void UniformBuffer::addFloat3(const std::string& iName, float x, float y, float z)
{
  Float32Array temp{x, y, z};
  addUniform(iName, temp);
}

void UniformBuffer::addColor3(const std::string& iName, const Color3& color)
{
  Float32Array temp{color.r, color.g, color.b};
  addUniform(iName, temp);
}

void UniformBuffer::addColor4(const std::string& iName, const Color3& color, float alpha)
{
  Float32Array temp{color.r, color.g, color.b, alpha};
  addUniform(iName, temp);
}

void UniformBuffer::addVector3(const std::string& iName, const Vector3& vector)
{
  Float32Array temp{vector.x, vector.y, vector.z};
  addUniform(iName, temp);
}

void UniformBuffer::addMatrix3x3(const std::string& iName)
{
  addUniform(iName, 12);
}

void UniformBuffer::addMatrix2x2(const std::string& iName)
{
  addUniform(iName, 8);
}

void UniformBuffer::create()
{
  if (_noUBO) {
    return;
  }
  if (_buffer) {
    return; // nothing to do
  }

  // See spec, alignment must be filled as a vec4
  _fillAlignment(4);
  _bufferData = Float32Array(_data);

  _rebuild();

  _needSync = true;
}

void UniformBuffer::_rebuild()
{
  if (_noUBO || _bufferData.empty()) {
    return;
  }

  if (_dynamic) {
    _buffer = _engine->createDynamicUniformBuffer(_bufferData);
  }
  else {
    _buffer = _engine->createUniformBuffer(_bufferData);
  }

  if (_engine->_features.trackUbosInFrame) {
    _buffers.emplace_back(_buffer);
    _bufferIndex         = static_cast<int>(_buffers.size()) - 1;
    _createBufferOnWrite = false;
  }
}

size_t UniformBuffer::get__numBuffers() const
{
  return _buffers.size();
}

int UniformBuffer::get__indexBuffer() const
{
  return _bufferIndex;
}

std::string UniformBuffer::get_name() const
{
  return _name;
}

void UniformBuffer::update()
{
  if (!_buffer) {
    create();
    return;
  }

  if (!_dynamic && !_needSync) {
    _createBufferOnWrite = _engine->_features.trackUbosInFrame;
    return;
  }

  _engine->updateUniformBuffer(_buffer, _bufferData);

  if (_engine->_features._collectUbosUpdatedInFrame) {
    if (!stl_util::contains(UniformBuffer::_updatedUbosInFrame, _name)) {
      UniformBuffer::_updatedUbosInFrame[_name] = 0;
    }
    UniformBuffer::_updatedUbosInFrame[_name]++;
  }

  _needSync            = false;
  _createBufferOnWrite = _engine->_features.trackUbosInFrame;
}

void UniformBuffer::_createNewBuffer()
{
  if (_bufferIndex + 1 < static_cast<int>(_buffers.size())) {
    _bufferIndex++;
    _buffer              = _buffers[_bufferIndex];
    _createBufferOnWrite = false;
    _needSync            = true;
  }
  else {
    _rebuild();
  }
  if (_currentEffect && _buffer) {
    _currentEffect->bindUniformBuffer(_buffer, _currentEffectName);
  }
}

void UniformBuffer::_checkNewFrame()
{
  if (_engine->_features.trackUbosInFrame && _currentFrameId != _engine->frameId()) {
    _currentFrameId      = _engine->frameId();
    _createBufferOnWrite = false;
    if (_buffers.size() > 0) {
      _needSync    = _bufferIndex != 0;
      _bufferIndex = 0;
      _buffer      = _buffers[_bufferIndex];
    }
    else {
      _bufferIndex = -1;
    }
    if (_currentEffect && _buffer) {
      _currentEffect->bindUniformBuffer(_buffer, _currentEffectName);
    }
  }
}

void UniformBuffer::updateUniform(const std::string& uniformName, const Float32Array& data,
                                  size_t size)
{
  _checkNewFrame();

  size_t location = 0;
  if (!stl_util::contains(_uniformLocations, uniformName)) {
    if (_buffer) {
      // Cannot add an uniform if the buffer is already created
      BABYLON_LOG_ERROR("UniformBuffer", "Cannot add an uniform after UBO has been created.")
      return;
    }
    addUniform(uniformName, static_cast<int>(size));
    location = _uniformLocations[uniformName];
  }
  else {
    location = _uniformLocations[uniformName];
  }

  if (!_buffer) {
    create();
  }

  if (!_dynamic) {
    // Cache for static uniform buffers
    auto changed = false;

    for (size_t i = 0; i < size; ++i) {
      // We are checking the matrix cache before calling updateUniform so we do not need to check it
      // here Hence the test for size === 16 to simply commit the matrix values
      if ((size == 16 && !_engine->_features.uniformBufferHardCheckMatrix)
          || !stl_util::almost_equal(_bufferData[location + i], data[i])) {
        changed = true;
        if (_createBufferOnWrite) {
          _createNewBuffer();
        }
        _bufferData[location + i] = data[i];
      }
    }

    _needSync = _needSync || changed;
  }
  else {
    // No cache for dynamic
    for (size_t i = 0; i < size; ++i) {
      _bufferData[location + i] = data[i];
    }
  }
}

void UniformBuffer::updateUniformArray(const std::string& uniformName, const Float32Array& data,
                                       size_t size)
{
  _checkNewFrame();

  if (!stl_util::contains(_uniformLocations, uniformName)) {
    BABYLON_LOG_ERROR("UniformBuffer",
                      "Cannot add an uniform Array dynamically. Please, add it using addUniform.");
    return;
  }
  const auto& location = _uniformLocations[uniformName];

  if (!_buffer) {
    create();
  }

  const auto& arraySizes = _uniformArraySizes[uniformName];

  if (!_dynamic) {
    // Cache for static uniform buffers
    auto changed     = false;
    auto countToFour = 0ull;
    auto baseStride  = 0;
    for (auto i = 0ull; i < size; ++i) {
      if (_bufferData[location + baseStride * 4 + countToFour] != data[i]) {
        changed = true;
        if (_createBufferOnWrite) {
          _createNewBuffer();
        }
        _bufferData[location + baseStride * 4 + countToFour] = data[i];
      }
      countToFour++;
      if (countToFour == arraySizes.strideSize) {
        for (; countToFour < 4; countToFour++) {
          _bufferData[location + baseStride * 4 + countToFour] = 0;
        }
        countToFour = 0;
        baseStride++;
      }
    }

    _needSync = _needSync || changed;
  }
  else {
    // No cache for dynamic
    for (auto i = 0ull; i < size; i++) {
      _bufferData[location + i] = data[i];
    }
  }
}

bool UniformBuffer::_cacheMatrix(const std::string& iName, const Matrix& matrix)
{
  _checkNewFrame();

  const auto flag = matrix.updateFlag;
  if (stl_util::contains(_valueCache, iName) && _valueCache[iName] == flag) {
    return false;
  }

  return true;
}

void UniformBuffer::_updateMatrix3x3ForUniform(const std::string& iName, const Float32Array& matrix)
{
  // To match std140, matrix must be realigned
  for (unsigned int i = 0; i < 3; ++i) {
    UniformBuffer::_tempBuffer[i * 4]     = matrix[i * 3];
    UniformBuffer::_tempBuffer[i * 4 + 1] = matrix[i * 3 + 1];
    UniformBuffer::_tempBuffer[i * 4 + 2] = matrix[i * 3 + 2];
    UniformBuffer::_tempBuffer[i * 4 + 3] = 0.f;
  }

  updateUniform(iName, UniformBuffer::_tempBuffer, 12);
}

void UniformBuffer::_updateMatrix3x3ForEffect(const std::string& iName, const Float32Array& matrix)
{
  _currentEffect->setMatrix3x3(iName, matrix);
}

void UniformBuffer::_updateMatrix2x2ForEffect(const std::string& iName, const Float32Array& matrix)
{
  _currentEffect->setMatrix2x2(iName, matrix);
}

void UniformBuffer::_updateMatrix2x2ForUniform(const std::string& iName, const Float32Array& matrix)
{
  // To match std140, matrix must be realigned
  for (unsigned int i = 0; i < 2; i++) {
    UniformBuffer::_tempBuffer[i * 4]     = matrix[i * 2];
    UniformBuffer::_tempBuffer[i * 4 + 1] = matrix[i * 2 + 1];
    UniformBuffer::_tempBuffer[i * 4 + 2] = 0.f;
    UniformBuffer::_tempBuffer[i * 4 + 3] = 0.f;
  }

  updateUniform(iName, UniformBuffer::_tempBuffer, 8);
}

void UniformBuffer::_updateFloatForEffect(const std::string& iName, float x)
{
  _currentEffect->setFloat(iName, x);
}

void UniformBuffer::_updateFloatForUniform(const std::string& iName, float x)
{
  UniformBuffer::_tempBuffer[0] = x;
  updateUniform(iName, UniformBuffer::_tempBuffer, 1);
}

void UniformBuffer::_updateFloat2ForEffect(const std::string& iName, float x, float y,
                                           const std::string& suffix)
{
  _currentEffect->setFloat2(iName + suffix, x, y);
}

void UniformBuffer::_updateFloat2ForUniform(const std::string& iName, float x, float y)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  updateUniform(iName, UniformBuffer::_tempBuffer, 2);
}

void UniformBuffer::_updateFloat3ForEffect(const std::string& iName, float x, float y, float z,
                                           const std::string& suffix)
{
  _currentEffect->setFloat3(iName + suffix, x, y, z);
}

void UniformBuffer::_updateFloat3ForUniform(const std::string& iName, float x, float y, float z)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  UniformBuffer::_tempBuffer[2] = z;
  updateUniform(iName, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateFloat4ForEffect(const std::string& iName, float x, float y, float z,
                                           float w, const std::string& suffix)
{
  _currentEffect->setFloat4(iName + suffix, x, y, z, w);
}

void UniformBuffer::_updateFloat4ForUniform(const std::string& iName, float x, float y, float z,
                                            float w)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  UniformBuffer::_tempBuffer[2] = z;
  UniformBuffer::_tempBuffer[3] = w;
  updateUniform(iName, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateFloatArrayForEffect(const std::string& iName, const Float32Array& array)
{
  _currentEffect->setFloatArray(iName, array);
}

void UniformBuffer::_updateFloatArrayForUniform(const std::string& iName, const Float32Array& array)
{
  updateUniformArray(iName, array, array.size());
}

void UniformBuffer::_updateArrayForEffect(const std::string& iName, const Float32Array& array)
{
  _currentEffect->setArray(iName, array);
}

void UniformBuffer::_updateArrayForUniform(const std::string& iName, const Float32Array& array)
{
  updateUniformArray(iName, array, array.size());
}

void UniformBuffer::_updateIntArrayForEffect(const std::string& iName, const Int32Array& array)
{
  _currentEffect->setIntArray(iName, array);
}

void UniformBuffer::_updateIntArrayForUniform(const std::string& iName, const Int32Array& array)
{
  for (size_t i = 0; i < array.size(); ++i) {
    UniformBuffer::_tempBuffer[i] = static_cast<float>(array[i]);
  }
  updateUniformArray(iName, UniformBuffer::_tempBuffer, array.size());
}

void UniformBuffer::_updateMatrixForEffect(const std::string& iName, const Matrix& mat)
{
  _currentEffect->setMatrix(iName, mat);
}

void UniformBuffer::_updateMatrixForUniform(const std::string& iName, const Matrix& mat)
{
  if (_cacheMatrix(iName, mat)) {
    updateUniform(iName, mat.toArray(), 16);
  }
}

void UniformBuffer::_updateMatricesForEffect(const std::string& iName, const Float32Array& mat)
{
  _currentEffect->setMatrices(iName, mat);
}

void UniformBuffer::_updateMatricesForUniform(const std::string& iName, const Float32Array& mat)
{
  updateUniform(iName, mat, mat.size());
}

void UniformBuffer::_updateVector3ForEffect(const std::string& iName, const Vector3& vector)
{
  _currentEffect->setVector3(iName, vector);
}

void UniformBuffer::_updateVector3ForUniform(const std::string& iName, const Vector3& vector)
{
  UniformBuffer::_tempBuffer[0] = vector.x;
  UniformBuffer::_tempBuffer[1] = vector.y;
  UniformBuffer::_tempBuffer[2] = vector.z;
  updateUniform(iName, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateVector4ForEffect(const std::string& iName, const Vector4& vector)
{
  _currentEffect->setVector4(iName, vector);
}

void UniformBuffer::_updateVector4ForUniform(const std::string& iName, const Vector4& vector)
{
  UniformBuffer::_tempBuffer[0] = vector.x;
  UniformBuffer::_tempBuffer[1] = vector.y;
  UniformBuffer::_tempBuffer[2] = vector.z;
  UniformBuffer::_tempBuffer[3] = vector.w;
  updateUniform(iName, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateColor3ForEffect(const std::string& iName, const Color3& color,
                                           const std::string& suffix)
{
  _currentEffect->setColor3(iName + suffix, color);
}

void UniformBuffer::_updateColor3ForUniform(const std::string& iName, const Color3& color)
{
  UniformBuffer::_tempBuffer[0] = color.r;
  UniformBuffer::_tempBuffer[1] = color.g;
  UniformBuffer::_tempBuffer[2] = color.b;
  updateUniform(iName, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateColor4ForEffect(const std::string& iName, const Color3& color,
                                           float alpha, const std::string& suffix)
{
  _currentEffect->setColor4(iName + suffix, color, alpha);
}

void UniformBuffer::_updateDirectColor4ForEffect(const std::string& name, const Color4& color,
                                                 const std::string& suffix)
{
  _currentEffect->setDirectColor4(name + suffix, color);
}

void UniformBuffer::_updateColor4ForUniform(const std::string& iName, const Color3& color,
                                            float alpha)
{
  UniformBuffer::_tempBuffer[0] = color.r;
  UniformBuffer::_tempBuffer[1] = color.g;
  UniformBuffer::_tempBuffer[2] = color.b;
  UniformBuffer::_tempBuffer[3] = alpha;
  updateUniform(iName, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateDirectColor4ForUniform(const std::string& name, const Color4& color)
{
  UniformBuffer::_tempBuffer[0] = color.r;
  UniformBuffer::_tempBuffer[1] = color.g;
  UniformBuffer::_tempBuffer[2] = color.b;
  UniformBuffer::_tempBuffer[3] = color.a;
  updateUniform(name, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateIntForEffect(const std::string& iName, int x, const std::string& suffix)
{
  _currentEffect->setInt(iName + suffix, x);
}

void UniformBuffer::_updateIntForUniform(const std::string& iName, int x)
{
  UniformBuffer::_tempBuffer[0] = static_cast<float>(x);
  updateUniform(iName, UniformBuffer::_tempBuffer, 1);
}

void UniformBuffer::_updateInt2ForEffect(const std::string& iName, int x, int y,
                                         const std::string& suffix)
{
  _currentEffect->setInt2(iName + suffix, x, y);
}

void UniformBuffer::_updateInt2ForUniform(const std::string& iName, int x, int y)
{
  UniformBuffer::_tempBuffer[0] = static_cast<float>(x);
  UniformBuffer::_tempBuffer[1] = static_cast<float>(y);
  updateUniform(iName, UniformBuffer::_tempBuffer, 2);
}

void UniformBuffer::_updateInt3ForEffect(const std::string& iName, int x, int y, int z,
                                         const std::string& suffix)
{
  _currentEffect->setInt3(iName + suffix, x, y, z);
}

void UniformBuffer::_updateInt3ForUniform(const std::string& iName, int x, int y, int z)
{

  UniformBuffer::_tempBuffer[0] = static_cast<float>(x);
  UniformBuffer::_tempBuffer[1] = static_cast<float>(y);
  UniformBuffer::_tempBuffer[2] = static_cast<float>(z);
  updateUniform(iName, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateInt4ForEffect(const std::string& iName, int x, int y, int z, int w,
                                         const std::string& suffix)
{
  _currentEffect->setInt4(iName + suffix, x, y, z, w);
}

void UniformBuffer::_updateInt4ForUniform(const std::string& iName, int x, int y, int z, int w)
{
  UniformBuffer::_tempBuffer[0] = static_cast<float>(x);
  UniformBuffer::_tempBuffer[1] = static_cast<float>(y);
  UniformBuffer::_tempBuffer[2] = static_cast<float>(z);
  UniformBuffer::_tempBuffer[3] = static_cast<float>(w);
  updateUniform(iName, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::setTexture(const std::string& iName, const ThinTexturePtr& texture)
{
  _currentEffect->setTexture(iName, texture);
}

void UniformBuffer::updateUniformDirectly(const std::string& uniformName, const Float32Array& data)
{
  updateUniform(uniformName, data, data.size());

  update();
}

void UniformBuffer::bindToEffect(Effect* effect, const std::string& iName)
{
  _currentEffect     = effect;
  _currentEffectName = iName;

  if (_noUBO || !_buffer) {
    return;
  }

  _alreadyBound = true;
  effect->bindUniformBuffer(_buffer, iName);
}

void UniformBuffer::dispose()
{
  if (_noUBO) {
    return;
  }

  stl_util::remove_vector_elements_equal_ptr(_engine->_uniformBuffers, this);

  if (_engine->_features.trackUbosInFrame && !_buffers.empty()) {
    for (const auto& buffer : _buffers) {
      if (buffer) {
        _engine->_releaseBuffer(buffer);
      }
    }
  }
  else if (_buffer && _engine->_releaseBuffer(_buffer)) {
    _buffer = nullptr;
  }
}

} // end of namespace BABYLON
