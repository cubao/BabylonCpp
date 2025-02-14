#include <babylon/postprocesses/pass_cube_post_process.h>

namespace BABYLON {

PassCubePostProcess::PassCubePostProcess(const std::string& iName,
                                         const std::variant<float, PostProcessOptions>& options,
                                         const CameraPtr& camera,
                                         const std::optional<unsigned int>& samplingMode,
                                         Engine* engine, bool reusable, unsigned int textureType,
                                         bool blockCompilation)
    : PostProcess{iName,        "passCube",
                  {},           {},
                  options,      camera,
                  samplingMode, engine,
                  reusable,     "#define POSITIVEX",
                  textureType,  "",
                  {},           blockCompilation}
    , face{this, &PassCubePostProcess::get_face, &PassCubePostProcess::set_face}
    , _face{}
{
}

PassCubePostProcess::~PassCubePostProcess() = default;

unsigned int PassCubePostProcess::get_face() const
{
  return _face;
}

void PassCubePostProcess::set_face(unsigned int value)
{
  if (value > 5) {
    return;
  }

  _face = value;
  switch (_face) {
    case 0:
      updateEffect("#define POSITIVEX");
      break;
    case 1:
      updateEffect("#define NEGATIVEX");
      break;
    case 2:
      updateEffect("#define POSITIVEY");
      break;
    case 3:
      updateEffect("#define NEGATIVEY");
      break;
    case 4:
      updateEffect("#define POSITIVEZ");
      break;
    case 5:
      updateEffect("#define NEGATIVEZ");
      break;
  }
}

std::string PassCubePostProcess::getClassName() const
{
  return "PassCubePostProcess";
}

PassCubePostProcessPtr PassCubePostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                   const CameraPtr& /*targetCamera*/,
                                                   Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
