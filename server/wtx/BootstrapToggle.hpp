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
  RH_ENUM_CLASS_REFLECTED(State, on, off, initial = off);
  RH_ENUM_CLASS_REFLECTED(Size, large, normal, small, mini, initial = normal);
  RH_ENUM_CLASS_REFLECTED(Enabled, enable, disable);

  BootstrapToggle(Wt::WContainerWidget* parent = nullptr,
                  State state = State::initial,
                  Size size = Size::initial)
    : Base(parent)
  {
    setState(state);

    std::stringstream javaScriptOptionsSS;
    // Not setting default options to allow html attributes override them.
    // see http://www.bootstraptoggle.com/ for details.
    if(size != Size::initial) {
      javaScriptOptionsSS << "{";
      javaScriptOptionsSS << Options::itemName(Options::size) << ":"
                          << "'" << size.itemName() << "'";
      javaScriptOptionsSS << "}";
    }

    m_javaScriptOptions = javaScriptOptionsSS.str();

    auto* app = Wt::WApplication::instance();
    app->useStyleSheet("wtx/css/bootstrap-toggle.css");
    app->require(Wt::WLink("wtx/js/bootstrap-toggle.js").url());
  }

  void setState(State state) {
    setChecked(state == State::on ? true : false);
  }

  State state() {
    return isChecked() ? State::on : State::off;
  }

  void setChecked(bool value) override {
    Base::setChecked(value);
    updateBootstrapToggleState();
  }

 protected:
  std::string renderAtBrowserJavaScriptStatement() override {
    return
      "(function() {\n"
      "  let $toggle = $('#" + id() + "');\n"
      "  $toggle.bootstrapToggle(" + m_javaScriptOptions + ");\n"
      "  let onchange = $toggle.attr('onchange');\n"
      "  $toggle.attr('onchange', null);\n"
      "  $toggle.bootstrapToggle('" +
      std::string(isChecked() ? State::itemName(State::on) :
                                State::itemName(State::off)) + "');\n"
      "  $toggle.attr('onchange', onchange);\n"
      "}())\n";
  }

  void propagateSetEnabled(bool enabled) override {
    Base::propagateSetEnabled(enabled);
    updateBootstrapToggleEnabled(enabled);
  }

  void updateBootstrapToggleState() {
    if(!renderedAtBrowser()) return;
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;"
      "  let $toggle = $('#" + id() + "');\n"
      "  let toggleDisabled = false;\n"
      "  if(!!$toggle.attr('disabled')) {\n"
      "    toggleDisabled = true;\n"
      "    $toggle.bootstrapToggle('enable');\n"
      "  }\n"
      "  let onchange = $toggle.attr('onchange');\n"
      "  $toggle.attr('onchange', null);\n"
      "  $toggle.bootstrapToggle('update');\n"
      "  $toggle.attr('onchange', onchange);\n"
      "  if(toggleDisabled) {\n"
      "    $toggle.bootstrapToggle('disable');\n"
      "  }\n"
      "}, 0);\n");
  }

  void updateBootstrapToggleEnabled(bool enabled) {
    if(!renderedAtBrowser()) return;
    Enabled enabledEnum = enabled ? Enabled::enable : Enabled::disable;
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;"
      "  $('#" + id() + "').bootstrapToggle('" +
      Enabled::itemName(enabledEnum) + "');\n"
      "}, 0);\n");
  };

 private:
  std::string m_javaScriptOptions;
};

} // namespace wtx

} // namespace rh

#endif // __BootstrapToggle_hpp__
