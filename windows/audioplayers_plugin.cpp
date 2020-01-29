// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "audioplayers_plugin.h"

#include <windows.h>
#include <memory>
#include <sstream>
#include <stdlib.h>
#include <MMsystem.h>
#include <mciapi.h>
#include <VersionHelpers.h>
#include <codecvt>
#include <string>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

namespace {

const flutter::EncodableValue &ValueOrNull(const flutter::EncodableMap &map, const char *key) {
  static flutter::EncodableValue null_value;
  auto it = map.find(flutter::EncodableValue(key));
  if (it == map.end()) {
    return null_value;
  }
  return it->second;
}

class AudioplayersPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar *registrar);

  // Creates a plugin that communicates on the given channel.
  AudioplayersPlugin(flutter::PluginRegistrar *registrar);

  virtual ~AudioplayersPlugin();

 private:
  // Called when a method is called on the plugin channel;
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  // The registrar for this plugin, for accessing the window.
  flutter::PluginRegistrar *registrar_;
};

// static
void AudioplayersPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrar *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "xyz.luan/audioplayers",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<AudioplayersPlugin>(registrar);

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

AudioplayersPlugin::AudioplayersPlugin(flutter::PluginRegistrar *registrar)
    : registrar_(registrar) {}

AudioplayersPlugin::~AudioplayersPlugin(){};

void AudioplayersPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("play") == 0) {
    //TCHAR path_buffer[MAX_PATH];
    ////UINT length = GetTempPath(MAX_PATH, path_buffer);
    //if (length == 0 || length > MAX_PATH) {
    //  result->Error("Unable to get temporary path");
    //////  return;
    //}
    auto args = method_call.arguments()->MapValue();
    auto input_filename = ValueOrNull(args, "url");
    if (input_filename.IsNull()) {
      result->Error("No file specified");
      return;
    }
    ////std::wstring_convert<std::codecvt_utf8<wchar_t>> wide_to_utf8;
    //std::string result_path = wide_to_utf8.to_bytes(path_buffer);
    //result_path += input_filename.StringValue();
    //path_buffer += "coin.wav";
    auto fileString = input_filename.StringValue();
    std::cout << fileString;
    std::wstring wstr(fileString.begin(), fileString.end());
    bool const ok = !!PlaySound(wstr.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    //mciSendString(TEXT("play coin.wav"), NULL, 0, NULL);
    if (ok) result->Success();
    else result->Error("Failed to play file " + input_filename.StringValue());
    return;
  }
}

}  // namespace

void AudioplayersPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  // The plugin registrar owns the plugin, registered callbacks, etc., so must
  // remain valid for the life of the application.
  static auto *plugin_registrar =
      new flutter::PluginRegistrar(registrar);

  AudioplayersPlugin::RegisterWithRegistrar(plugin_registrar);
}