// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "ColorScheme.h"
#include "DefaultSettings.h"
#include "../../types/inc/Utils.hpp"
#include "Utils.h"
#include "JsonUtils.h"

#include "ColorScheme.g.cpp"

using namespace ::Microsoft::Console;
using namespace TerminalApp;
using namespace winrt::TerminalApp::implementation;
using namespace winrt::Windows::UI;

static constexpr std::string_view NameKey{ "name" };
static constexpr std::string_view ForegroundKey{ "foreground" };
static constexpr std::string_view BackgroundKey{ "background" };
static constexpr std::string_view SelectionBackgroundKey{ "selectionBackground" };
static constexpr std::string_view CursorColorKey{ "cursorColor" };
static constexpr std::array<std::string_view, 16> TableColors = {
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "purple",
    "cyan",
    "white",
    "brightBlack",
    "brightRed",
    "brightGreen",
    "brightYellow",
    "brightBlue",
    "brightPurple",
    "brightCyan",
    "brightWhite"
};

ColorScheme::ColorScheme() :
    _schemeName{ L"" },
    _table{},
    _defaultForeground{ DEFAULT_FOREGROUND_WITH_ALPHA },
    _defaultBackground{ DEFAULT_BACKGROUND_WITH_ALPHA },
    _selectionBackground{ DEFAULT_FOREGROUND },
    _cursorColor{ DEFAULT_CURSOR_COLOR }
{
}

ColorScheme::ColorScheme(winrt::hstring name, Color defaultFg, Color defaultBg, Color cursorColor) :
    _schemeName{ name },
    _table{},
    _defaultForeground{ defaultFg },
    _defaultBackground{ defaultBg },
    _selectionBackground{ DEFAULT_FOREGROUND },
    _cursorColor{ cursorColor }
{
}

ColorScheme::~ColorScheme()
{
}

// Method Description:
// - Create a new instance of this class from a serialized JsonObject.
// Arguments:
// - json: an object which should be a serialization of a ColorScheme object.
// Return Value:
// - a new ColorScheme instance created from the values in `json`
winrt::com_ptr<ColorScheme> ColorScheme::FromJson(const Json::Value& json)
{
    auto result = winrt::make_self<ColorScheme>();
    result->LayerJson(json);
    return result;
}

// Method Description:
// - Returns true if we think the provided json object represents an instance of
//   the same object as this object. If true, we should layer that json object
//   on us, instead of creating a new object.
// Arguments:
// - json: The json object to query to see if it's the same
// Return Value:
// - true iff the json object has the same `name` as we do.
bool ColorScheme::ShouldBeLayered(const Json::Value& json) const
{
    std::wstring nameFromJson{};
    if (JsonUtils::GetValueForKey(json, NameKey, nameFromJson))
    {
        return nameFromJson == _schemeName;
    }
    return false;
}

// Method Description:
// - Layer values from the given json object on top of the existing properties
//   of this object. For any keys we're expecting to be able to parse in the
//   given object, we'll parse them and replace our settings with values from
//   the new json object. Properties that _aren't_ in the json object will _not_
//   be replaced.
// Arguments:
// - json: an object which should be a partial serialization of a ColorScheme object.
// Return Value:
// <none>
void ColorScheme::LayerJson(const Json::Value& json)
{
    JsonUtils::GetValueForKey(json, NameKey, _schemeName);
    JsonUtils::GetValueForKey(json, ForegroundKey, _defaultForeground);
    JsonUtils::GetValueForKey(json, BackgroundKey, _defaultBackground);
    JsonUtils::GetValueForKey(json, SelectionBackgroundKey, _selectionBackground);
    JsonUtils::GetValueForKey(json, CursorColorKey, _cursorColor);

    int i = 0;
    for (const auto& current : TableColors)
    {
        JsonUtils::GetValueForKey(json, current, _table.at(i));
        i++;
    }
}

winrt::hstring ColorScheme::Name() const noexcept
{
    return _schemeName;
}

winrt::com_array<Color> ColorScheme::Table() const noexcept
{
    winrt::com_array<Color> result{ COLOR_TABLE_SIZE };
    std::transform(_table.begin(), _table.end(), result.begin(), [](til::color c) -> Color { return c; });
    return result;
}

Color ColorScheme::Foreground() const noexcept
{
    return _defaultForeground;
}

Color ColorScheme::Background() const noexcept
{
    return _defaultBackground;
}

Color ColorScheme::SelectionBackground() const noexcept
{
    return _selectionBackground;
}

Color ColorScheme::CursorColor() const noexcept
{
    return _cursorColor;
}

// Method Description:
// - Parse the name from the JSON representation of a ColorScheme.
// Arguments:
// - json: an object which should be a serialization of a ColorScheme object.
// Return Value:
// - the name of the color scheme represented by `json` as a std::wstring optional
//   i.e. the value of the `name` property.
// - returns std::nullopt if `json` doesn't have the `name` property
std::optional<std::wstring> ColorScheme::GetNameFromJson(const Json::Value& json)
{
    return JsonUtils::GetValueForKey<std::optional<std::wstring>>(json, NameKey);
}
