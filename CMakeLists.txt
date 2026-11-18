cmake_minimum_required(VERSION 3.22)
project(METAPHYSICAL VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build for Apple Silicon" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(METAPHYSICAL
    COMPANY_NAME "ANY Studios"
    BUNDLE_ID "studio.any.metaphysical"
    PLUGIN_MANUFACTURER_CODE Anys
    PLUGIN_CODE Mphy
    FORMATS VST3
    PRODUCT_NAME "METAPHYSICAL"
    DESCRIPTION "Physical modelling instrument by ANY Studios"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(METAPHYSICAL PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PlateModel.cpp
)

target_compile_definitions(METAPHYSICAL PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(METAPHYSICAL PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)

add_executable(PlateModelTests Tests/PlateModelTests.cpp Source/PlateModel.cpp)
target_include_directories(PlateModelTests PRIVATE Source)
enable_testing()
add_test(NAME PlateModelTests COMMAND PlateModelTests)
