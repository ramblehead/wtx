// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __BootstrapToggle_hpp__
#define __BootstrapToggle_hpp__

#include <string>
#include <regex>

#include "reflection/RH_ENUM_CLASS_REFLECTED.hpp"

#include "JavaScriptWidget.hpp"
#include "SimpleCheckbox.hpp"

namespace rh {

namespace wtx {

class BootstrapToggle : public JavaScriptWidget<SimpleCheckbox> {
 private:
  using This = BootstrapToggle;
  using Base = JavaScriptWidget<SimpleCheckbox>;

 public:
  RH_ENUM_CLASS_REFLECTED(
    Options, on, off, size, onstyle, offstyle, style, width, height);
  RH_ENUM_CLASS_REFLECTED(State, on, off);
  State stateDefault = State::off;
  RH_ENUM_CLASS_REFLECTED(Size, large, normal, small, mini);
  Size sizeDefault = Size::normal;
  RH_ENUM_CLASS_REFLECTED(Style, primary, success, info, warning, danger);
  Style styleDefault = Style::primary;
  RH_ENUM_CLASS_REFLECTED(Enabled, enable, disable);

  BootstrapToggle(Wt::WContainerWidget* parent = nullptr,
                  State state = State::off,
                  Size size = Size::normal,
                  Style style = Style::primary)
    : Base(parent), m_initialState(state)
  {
    std::stringstream javaScriptOptionsSS;
    // Not setting default options to allow html attributes override them.
    // see http://www.bootstraptoggle.com/ for details.
    if(style != styleDefault || size != sizeDefault) {
      javaScriptOptionsSS << "{";
      if(style != styleDefault) {
        javaScriptOptionsSS << Options::itemName(Options::style) << ":"
                            << "'" << Style::itemName(style) << "'";
      }
      if(size != sizeDefault) {
        if(style != styleDefault) javaScriptOptionsSS << ",";
        javaScriptOptionsSS << Options::itemName(Options::size) << ":"
                            << "'" << Size::itemName(size) << "'";
      }
      javaScriptOptionsSS << "}";
    }

    m_javaScriptOptions = javaScriptOptionsSS.str();

    auto* app = Wt::WApplication::instance();
    app->useStyleSheet("wtx/css/bootstrap-toggle.css");
    app->require(Wt::WLink("wtx/js/bootstrap-toggle.js").url());
  }

  void setChecked(bool value) override {
    Base::setChecked(value);
    updateBootstrapToggleState();
  }

 protected:
  std::string renderAtBrowserJavaScriptStatement() override {
    return
      "(function() {\n"
      "  let $t = $('#" + id() + "');\n"
      "  $t.bootstrapToggle(" + m_javaScriptOptions + ");\n"
      "  $t.bootstrapToggle('" + State::itemName(m_initialState) + "');\n"
      "}())\n";
  }

  void propagateSetEnabled(bool enabled) override {
    Base::propagateSetEnabled(enabled);
    updateBootstrapToggleEnabled(enabled);
  }

  void updateBootstrapToggleState() {
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;"
      "  var toggle = $('#" + id() + "');\n"
      "  var toggleDisabled = false;\n"
      "  if(!!toggle.attr('disabled')) {\n"
      "    toggleDisabled = true;\n"
      "    toggle.bootstrapToggle('enable');\n"
      "  }\n"
      "  toggle.bootstrapToggle('update');\n"
      "  if(toggleDisabled) {\n"
      "    toggle.bootstrapToggle('disable');\n"
      "  }\n"
      "}, 0);\n");
  }

  void updateBootstrapToggleEnabled(bool enabled) {
    Enabled enabledEnum = enabled ? Enabled::enable : Enabled::disable;
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;"
      "  $('#" + id() + "').bootstrapToggle(\n"
      "      '" + Enabled::itemName(enabledEnum) + "');\n"
      "}, 0);\n");
  };

 private:
  std::string m_javaScriptOptions;
  State m_initialState;
};

} // namespace wtx

} // namespace rh

#endif // __BootstrapToggle_hpp__
