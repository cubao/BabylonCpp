#ifdef _MSC_VER
#include <filesystem>
#endif
#include <map>
#include <babylon/core/string.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>
#include <babylon/imgui_babylon/sample_list_page.h>
#include <babylon/interfaces/irenderable_scene.h>

#include <imgui.h>

#include <babylon/inspector/inspector.h>
#include <babylon/samples/samples_index.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

namespace
{
std::string to_snake_case(const std::string &sPascalCase)
{
  std::stringstream ss;
  bool first = true;
  for (auto c : sPascalCase)
  {
    if (std::tolower(c) != c)
    {
      if (!first)
        ss << "_";
      ss << static_cast<unsigned char>(std::tolower(c));
    }
    else
      ss << c;
    first = false;
  }
  return ss.str();
}

const std::string screenshotsFolderCurrent = "../../../assets/screenshots/samples_current/";
const std::string screenshotsFolderOriginal = "../../../assets/screenshots/samples/";

} // end anonymous namespace


namespace BABYLON {

namespace
{
  void openBrowser(const std::string &url)
  {
#ifndef _WIN32
    std::string cmd = std::string("open ") + url;
    system(cmd.c_str());
#else
    ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#endif
  }
  void openFile(const std::string &filename)
  {
#ifndef _WIN32
    std::string cmd = std::string("open ") + filename;
    system(cmd.c_str());
#else
    std::string canonical_path = std::filesystem::canonical(std::filesystem::path(filename)).string();
    ShellExecute(0, 0, canonical_path.c_str(), 0, 0, SW_SHOW);
#endif
  }
}



using namespace BABYLON::Samples;

class SampleListPageImpl
{
public:
  using CategoryName = std::string;
  using SampleName = std::string;

  SampleListPageImpl()
  {
    for (const std::string & sample : _samplesIndex.getSampleNames())
      _samplesInfos[sample] = _samplesIndex.getSampleInfo(sample);
    fillMatchingSamples();
  }

  void render()
  {
    render_filter();
    ImGui::Checkbox("Show original screenshots", &_showOriginalScreenshots);
    ImGui::SameLine();
    ImGui::Checkbox("Show current screenshots", &_showCurrentScreenshots);
    ImGui::Separator();
    render_list();
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();
  }

  SampleListPage::CallbackNewRenderableScene OnNewRenderableScene;

private:
  void render_filter()
  {
    bool changed = false;
    ImGui::Text("Filter");
    if (ImGui::InputText_String("", _query.query))
      changed = true;

    if (ImGui::Checkbox("Include disabled", &_query.includeDisabled))
      changed = true;
    ImGui::SameLine();
    if (ImGui::Checkbox("Include failing", &_query.includeFailures))
      changed = true;
    ImGui::SameLine();
    if (ImGui::Checkbox("Only failing", &_query.onlyFailures))
      changed = true;

    if (changed)
      fillMatchingSamples();
  }

  void render_list()
  {
    enum class CollapseMode
    {
      None,
      CollapseAll,
      ExpandAll
    };
    CollapseMode collapseMode = CollapseMode::None;
    if (ImGui::Button("Collapse All"))
      collapseMode = CollapseMode::CollapseAll;
    ImGui::SameLine();
    if (ImGui::Button("Expand All"))
      collapseMode = CollapseMode::ExpandAll;

    ImGui::BeginChild("Child1");
    for (const auto & kv : _matchingSamples)
    {
      CategoryName category = kv.first;
      auto samples = kv.second;
      if (!samples.empty())
      {
        std::string header = category + " (" + std::to_string(samples.size()) + ")";
        if (collapseMode == CollapseMode::CollapseAll)
          ImGui::SetNextItemOpen(false);
        if (collapseMode == CollapseMode::ExpandAll)
          ImGui::SetNextItemOpen(true);
        if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
          for (const std::string & sample : samples)
          {
            //ImGui::Text("%s", sample.c_str());
            guiOneSample(sample);
            ImGui::Separator();
          }
        }
      }
    }
    ImGui::EndChild();

  }

  void guiOneSampleInfos(const std::string & sampleName)
  {
    const auto & sampleInfo = _samplesInfos.at(sampleName);

    std::string runLabel = std::string(ICON_FA_PLAY_CIRCLE " Run##") + sampleName;
    if (ImGui::Button(runLabel.c_str()))
    {
      if (OnNewRenderableScene)
      {
        BABYLON::ICanvas *dummyCanvas = nullptr;
        auto scene = _samplesIndex.createRenderableScene(sampleName, dummyCanvas);
        OnNewRenderableScene(std::move(scene));
      }
      if (Inspector::OnSampleChanged)
        Inspector::OnSampleChanged(sampleName);
    }

    std::string btnHeaderString = std::string(ICON_FA_EYE "##") + sampleInfo.HeaderFile;
    if (ImGui::Button(btnHeaderString.c_str()))
      openFile(sampleInfo.HeaderFile);
    ImGui::SameLine();
    ImGui::TextDisabled(".h  : %s", sampleInfo.HeaderFile.c_str());

    std::string btnSourceString = std::string(ICON_FA_EYE "##") + sampleInfo.SourceFile;
    if (ImGui::Button(btnSourceString.c_str()))
      openFile(sampleInfo.SourceFile);
    ImGui::SameLine();
    ImGui::TextDisabled(".cpp: %s", sampleInfo.SourceFile.c_str());

    if (!sampleInfo.Links.empty()) {
      for (auto link : sampleInfo.Links) {
        std::string btnUrlString = std::string(ICON_FA_EXTERNAL_LINK_ALT "##") + link;
        if (ImGui::Button(btnUrlString.c_str()))
          openBrowser(link);
        ImGui::SameLine();
        ImVec4 linkColor(0.5f, 0.5f, 0.95f, 1.f);
        ImGui::TextColored(linkColor, "%s", link.c_str());
      }
    }
  }

  void guiOneSample(const std::string &sampleName)
  {
    const auto & sampleInfo = _samplesInfos[sampleName];
    std::string currentScreenshotFile = screenshotsFolderCurrent + sampleName + ".jpg";
    std::string sample_snake = to_snake_case(sampleName);
    std::string originalScreenshotFile = screenshotsFolderOriginal + sample_snake + ".png";

    if (_showCurrentScreenshots)
    {
      ImGui::BeginGroup();
      ImGuiUtils::ImageFromFile(currentScreenshotFile);
      if (_showOriginalScreenshots && _showCurrentScreenshots)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.7f), "Current(c++)");
      ImGui::EndGroup();
      ImGui::SameLine();
    }

    if (_showOriginalScreenshots)
    {
      ImGui::BeginGroup();
      ImGuiUtils::ImageFromFile(originalScreenshotFile);
      if (_showOriginalScreenshots && _showCurrentScreenshots)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.7f), "Original(js)");
      ImGui::EndGroup();
      ImGui::SameLine();
    }

    ImGui::BeginGroup();
    ImGui::Text("%s", sampleName.c_str());
    ImGui::TextWrapped("%s", sampleInfo.Brief.c_str());
    auto failure = _samplesIndex.doesSampleFail(sampleName);
    if (failure)
    {
      ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.3f, 1.f), "Failure: %s",
        SampleFailureReason_Str(failure.value()).c_str()
      );
    }
    guiOneSampleInfos(sampleName);
    ImGui::EndGroup();
    ImGui::SameLine();

  }

  bool doesSampleMatchQuery(const CategoryName & categoryName, const SampleName & sampleName)
  {
    std::vector<std::string> search_items = BABYLON::String::split(_query.query, ' ');

    bool doesMatch = true;

    {
      std::string all = BABYLON::String::toLowerCase(categoryName + " / " + sampleName);
      for (const auto &item : search_items)
        if (!BABYLON::String::contains(all, BABYLON::String::toLowerCase(item)))
          doesMatch = false;
    }

    if (!_query.includeDisabled)
      if (!_samplesIndex.isSampleEnabled(sampleName))
        doesMatch = false;

    if (_query.onlyFailures)
    {
      if (!_samplesIndex.doesSampleFail(sampleName))
        doesMatch = false;
    }
    else if (!_query.includeFailures)
      if (_samplesIndex.doesSampleFail(sampleName))
        doesMatch = false;

    return doesMatch;
  }

  void fillMatchingSamples()
  {
    _matchingSamples.clear();

    for (CategoryName category : _samplesIndex.getCategoryNames())
    {
      std::vector<SampleName> s;
      for (SampleName sample : _samplesIndex.getSampleNamesInCategory(category))
      {
        if (doesSampleMatchQuery(category, sample))
          s.push_back(sample);
        _matchingSamples[category] = s;
      }
    }
  }

  std::map<SampleName, SampleInfo> _samplesInfos;
  SamplesIndex _samplesIndex;
  std::map<CategoryName, std::vector<SampleName>> _matchingSamples;

  struct {
    std::string query = "";
    bool includeFailures = false;
    bool onlyFailures = false;
    bool includeDisabled = true;
  } _query;

  bool _showOriginalScreenshots = false;
  bool _showCurrentScreenshots = true;
};


SampleListPage::SampleListPage()
{
  pImpl = std::make_unique<SampleListPageImpl>();
}


SampleListPage::~SampleListPage() = default;

void SampleListPage::render()
{
  pImpl->OnNewRenderableScene = OnNewRenderableScene;
  pImpl->render();
}

} // end of namespace BABYLON
