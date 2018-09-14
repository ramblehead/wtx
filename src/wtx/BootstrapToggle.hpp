// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_wtx_BootstrapToggle_hpp__
#define __rh_wtx_BootstrapToggle_hpp__

#include <string>

#include "reflection/RH_ENUM_SHELL.hpp"

#include "PostInitWidget.hpp"
#include "JavaScriptWidget.hpp"
#include "SimpleCheckBox.hpp"

namespace rh {

namespace wtx {

class PostInitSimpleCheckBox : public PostInitWidget<SimpleCheckBox> {
 private:
  using Base = PostInitWidget<SimpleCheckBox>;

 public:
  PostInitSimpleCheckBox(Wt::WContainerWidget *parent = nullptr)
      : Base({"0"}, parent)
  {}
};

class BootstrapToggle : public JavaScriptWidget<PostInitSimpleCheckBox> {
 private:
  using This = BootstrapToggle;
  using Base = JavaScriptWidget<PostInitSimpleCheckBox>;

 public:
  RH_ENUM_SHELL(
    Options, on, off, size, onstyle, offstyle, style, width, height);
  // RH_ENUM_SHELL(OnOffStyle, default, primary, success, info, warning, danger);
  RH_ENUM_SHELL(State, on, off);
  // Default size means let html/css to decide
  RH_ENUM_SHELL(Size, large, normal, small, mini, initial);
  RH_ENUM_SHELL(Enabled, enable, disable);

  BootstrapToggle(Size size = Size::initial,
                  Wt::WContainerWidget* parent = nullptr)
      : Base(parent)
  {
    Base::setChecked(false);
    m_pendingState = Base::isChecked() ? State::on : State::off;

    std::stringstream javaScriptOptionsSS;
    // Not setting default options to allow html attributes override them.
    // see http://www.bootstraptoggle.com/ for details.
    if(size != Size::initial) {
      javaScriptOptionsSS
        << "{"
        << Options::itemName(Options::size) << ":"
        << "'" << size.itemName() << "'"
        << "}";
    }

    m_javaScriptOptions = javaScriptOptionsSS.str();

    // changed().connect([this](Wt::NoClass) {
    //   m_pendingState = Base::isChecked() ? State::on : State::off;
    // });

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
    // m_stateSentToBrowser = m_pendingState;
    // Enabled enabledEnum = isEnabled() ? Enabled::enable : Enabled::disable;
    return "(function() {\n"
           "  let $toggle = $('#" + id() + "');\n"
           "  $toggle.bootstrapToggle(" + m_javaScriptOptions + ");\n"
           // "  let onchange = $toggle.attr('onchange');\n"
           // "  $toggle.attr('onchange', null);\n"
           // "  $toggle.next().css('transition', 'none');\n"
           // "  $toggle.bootstrapToggle('" + m_stateSentToBrowser.itemName() + "');\n"
           // // Trigger a reflow, flushing the CSS changes.
           // // see
           // // https://stackoverflow.com/questions/11131875/what-is-the-cleanest-way-to-disable-css-transition-effects-temporarily
           // "  $toggle.attr('onchange', onchange);\n"
           // "  $toggle.next()[0].offsetHeight;\n"
           // "  $toggle.next().css('transition', '');\n"
           // "  $toggle.bootstrapToggle('" + enabledEnum.itemName() + "');\n"
           "}())\n";
  }

  void afterRenderedAtBrowser() override {
    updateBootstrapToggleEnabled(isEnabled());
    Base::setChecked(m_pendingState == State::on ? true : false);
    updateBootstrapToggleState(m_pendingState, true);
  }

  void updateBootstrapToggleState(State state,
                                  bool disableTransition = false)
  {
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
      "  $toggle.bootstrapToggle('" + state.itemName() + "');\n"
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

  State m_pendingState;
  std::string m_javaScriptOptions;
};

} // namespace wtx

} // namespace rh

#endif // __rh_wtx_BootstrapToggle_hpp__
