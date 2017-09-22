// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __BootstrapToggle_hpp__
#define __BootstrapToggle_hpp__

#include <string>

#include "reflection/RH_ENUM_SHELL.hpp"

#include "JavaScriptWidget.hpp"
#include "SimpleCheckbox.hpp"

namespace rh {

namespace wtx {

class BootstrapToggle : public JavaScriptWidget<SimpleCheckbox> {
 private:
  using This = BootstrapToggle;
  using Base = JavaScriptWidget<SimpleCheckbox>;

 public:
  // Initial values correspond to the initial values
  // used in JavaScript code of Bootstrap Toggle.
  RH_ENUM_RSHELL(Options, "onText = on, offText = off, sizeStyle = size,"
                 "onStyle = onstyle, offStyle = offstyle",
                onText, offText, sizeStyle, onStyle, offStyle,
                style, width, height);
  RH_ENUM_SHELL(State, on, off, initial = off);
  RH_ENUM_SHELL(SizeStyle, large, normal, small, mini, initial = normal);
  RH_ENUM_RSHELL(StateStyle, "default_ = default",
                 default_, primary , success, info, warning, danger,
                 onInitial = primary, offInitial = default_);
  RH_ENUM_SHELL(Enabled, enable, disable);

  BootstrapToggle(Wt::WContainerWidget* parent = nullptr,
                  State state = State::initial,
                  SizeStyle size = SizeStyle::initial)
    : Base(parent)
  {
    Base::setChecked(state == State::on ? true : false);
    m_pendingState = Base::isChecked() ? State::on : State::off;

    std::stringstream javaScriptOptionsSS;
    // Not setting default options to allow html attributes to override them.
    // see http://www.bootstraptoggle.com/ for details.
    if(size != SizeStyle::initial) {
      javaScriptOptionsSS << "{";
      javaScriptOptionsSS << Options::itemName(Options::sizeStyle) << ":"
                          << "'" << size.itemName() << "'";
      javaScriptOptionsSS << "}";
    }

    m_javaScriptOptions = javaScriptOptionsSS.str();

    changed().connect([this](Wt::NoClass) {
      m_pendingState = Base::isChecked() ? State::on : State::off;
    });

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

  bool isChecked() const override {
    if(renderedAtBrowser()) return Base::isChecked();
    else return m_pendingState == State::on ? true : false;
  }

  void setChecked(bool value) override {
    m_pendingState = value ? State::on : State::off;
    if(renderedAtBrowser()) {
      Base::setChecked(value);
      updateBootstrapToggleState(Base::isChecked() ? State::on : State::off);
    }
  }

 protected:
  void propagateSetEnabled(bool enabled) override {
    Base::propagateSetEnabled(enabled);
    if(renderedAtBrowser()) {
      updateBootstrapToggleEnabled(enabled);
    }
  }

 private:
  std::string renderAtBrowserJavaScriptStatement() override {
    m_stateSentToBrowser = m_pendingState;
    Enabled enabledEnum = isEnabled() ?
                          Enabled::enable : Enabled::disable;
    return
      "(function() {\n"
      "  let $toggle = $('#" + id() + "');\n"
      "  $toggle.bootstrapToggle(" + m_javaScriptOptions + ");\n"
      "  let onchange = $toggle.attr('onchange');\n"
      "  $toggle.attr('onchange', null);\n"
      "  $toggle.next().css('transition', 'none');\n"
      "  $toggle.bootstrapToggle('" + m_stateSentToBrowser.itemName() + "');\n"
      // Trigger a reflow, flushing the CSS changes.
      // see https://stackoverflow.com/questions/11131875/what-is-the-cleanest-way-to-disable-css-transition-effects-temporarily
      "  $toggle.next()[0].offsetHeight;\n"
      "  $toggle.next().css('transition', '');\n"
      "  $toggle.attr('onchange', onchange);\n"
      "  $toggle.bootstrapToggle('" + enabledEnum.itemName() + "');\n"
      "}())\n";
  }

  void afterRenderedAtBrowser() override {
    updateBootstrapToggleEnabled(isEnabled());
    if(m_stateSentToBrowser != m_pendingState) {
      Base::setChecked(m_pendingState == State::on ? true : false);
      updateBootstrapToggleState(m_pendingState, true);
    }
  }

  void updateBootstrapToggleState(State state,
                                  bool disableTransition = false)
  {
    m_stateSentToBrowser = state;
    using g = std::string;
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;\n"
      "  let $toggle = $('#" + id() + "');\n"
      "  let toggleDisabled = false;\n"
      "  if(!!$toggle.attr('disabled')) {\n"
      "    toggleDisabled = true;\n"
      "    $toggle.bootstrapToggle('enable');\n"
      "  }\n"
      "  let onchange = $toggle.attr('onchange');\n"
      "  $toggle.attr('onchange', null);\n"
      + g(disableTransition ?
          "  $toggle.next().css('transition', 'none');\n" :
          "") +
      "  $toggle.bootstrapToggle('" + m_stateSentToBrowser.itemName() + "');\n"
      + g(disableTransition ?
          // Trigger a reflow, flushing the CSS changes.
          // see https://stackoverflow.com/questions/11131875/what-is-the-cleanest-way-to-disable-css-transition-effects-temporarily
          "  $toggle.next()[0].offsetHeight;\n"
          "  $toggle.next().css('transition', '');\n" :
          "") +
      "  $toggle.attr('onchange', onchange);\n"
      "  if(toggleDisabled) {\n"
      "    $toggle.bootstrapToggle('disable');\n"
      "  }\n"
      "}, 0);\n");
  }

  void updateBootstrapToggleEnabled(bool enabled) {
    Enabled enabledEnum = enabled ? Enabled::enable : Enabled::disable;
    doJavaScript(
      "setTimeout(function() {\n"
      "  if(!" + renderedAtBrowserJavaScriptPredicate() + ") return;\n"
      "  let $toggle = $('#" + id() + "');\n"
      "  $toggle.bootstrapToggle('" + enabledEnum.itemName() + "');\n"
      "}, 0);\n");
  }

  State m_stateSentToBrowser;
  State m_pendingState;
  std::string m_javaScriptOptions;
};

} // namespace wtx

} // namespace rh

#endif // __BootstrapToggle_hpp__
