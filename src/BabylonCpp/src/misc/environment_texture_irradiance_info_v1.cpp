#include <babylon/misc/environment_texture_irradiance_info_v1.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1() = default;

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1(
  const EnvironmentTextureIrradianceInfoV1& other)
  = default;

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1(
  EnvironmentTextureIrradianceInfoV1&& other)
  = default;

EnvironmentTextureIrradianceInfoV1&
EnvironmentTextureIrradianceInfoV1::operator=(const EnvironmentTextureIrradianceInfoV1& other)
  = default;

EnvironmentTextureIrradianceInfoV1&
EnvironmentTextureIrradianceInfoV1::operator=(EnvironmentTextureIrradianceInfoV1&& other)
  = default;

EnvironmentTextureIrradianceInfoV1::~EnvironmentTextureIrradianceInfoV1() = default;

EnvironmentTextureIrradianceInfoV1Ptr
EnvironmentTextureIrradianceInfoV1::Parse(const json& parsedIrradiance)
{
  EnvironmentTextureIrradianceInfoV1 irradiance;
  irradiance.x = json_util::get_array<float>(parsedIrradiance, "x");
  irradiance.y = json_util::get_array<float>(parsedIrradiance, "y");
  irradiance.z = json_util::get_array<float>(parsedIrradiance, "z");

  irradiance.xx = json_util::get_array<float>(parsedIrradiance, "xx");
  irradiance.yy = json_util::get_array<float>(parsedIrradiance, "yy");
  irradiance.zz = json_util::get_array<float>(parsedIrradiance, "zz");

  irradiance.yz = json_util::get_array<float>(parsedIrradiance, "yz");
  irradiance.zx = json_util::get_array<float>(parsedIrradiance, "zx");
  irradiance.xy = json_util::get_array<float>(parsedIrradiance, "xy");

  return std::make_shared<EnvironmentTextureIrradianceInfoV1>(irradiance);
}

} // end of namespace BABYLON
