// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_wtx_SimpleCheckBox_hpp__
#define __rh_wtx_SimpleCheckBox_hpp__

#include <Wt/WEnvironment>
#include <Wt/WAbstractToggleButton>
#include <Wt/DomElement.h>

namespace rh {

namespace wtx {

// SimpleCheckBox is modelled after Wt::WCheckBox
// Ideally, Wt::WFormWidget base class has to be used for
// SimpleCheckBox class instead of Wt::WAbstractToggleButton.
// However, instances of Wt::WAbstractToggleButton class have some special
// treatment in few places in Wt code, e.g.:
// WBootstrapTheme.C:git95b2ad9:374
// or
// WFormWidget.C:git95b2ad9:259
// Those treatments are also desirable for SimpleCheckBox (not adding
// "form-control" html class, for example). Otherwise, neither
// Wt::WAbstractToggleButton functions, nor its methods are used in
// SimpleCheckBox class.
class SimpleCheckBox : public Wt::WAbstractToggleButton {
 private:
  using Base = Wt::WAbstractToggleButton;

  // Disabling unused methods from Wt::WAbstractToggleButton
  void setText(const Wt::WString& text) =delete;
  const Wt::WString text() =delete;
  bool setTextFormat(Wt::TextFormat format) =delete;
  void setWordWrap(bool wordWrap) =delete;
  bool wordWrap() const =delete;
  // Ignore abstract Wt::WAbstractToggleButton::updateInput() method
  void updateInput(Wt::DomElement& input, bool all) final {}

 public:
  /*! \brief Creates a checkbox without label.
   */
  SimpleCheckBox(Wt::WContainerWidget *parent = 0)
    : Base(parent), state_(Wt::Unchecked)
  {
    setFormObject(true);
  }

  /*! \brief Destructor.
   */
  ~SimpleCheckBox() override {}

  /*! \brief Sets the toggle state.
   *
   * This method does not emit either the checked() or unChecked()
   * signals.
   *
   * \sa setChecked(), setUnChecked()
   */
  virtual void setChecked(bool value) {
    if(canOptimizeUpdates() && state_ == value) return;
    state_ = value ? Wt::Checked : Wt::Unchecked;
    flags_.set(BIT_STATE_CHANGED);
    repaint();
  }

  /*! \brief "Checks" the toggle.
   *
   * Does not emit the checked() signal.
   *
   * \sa setChecked(bool)
   */
  void setChecked() override {
    setChecked(true);
  }

  /*! \brief "Unchecks" the toggle.
   *
   * Does not emit the unChecked() signal.
   *
   * \sa setChecked(bool)
   */
  void setUnChecked() override {
    setChecked(false);
  }

  /*! \brief Returns the button state.
   *
   * \sa setChecked()
   */
  virtual bool isChecked() const {
    return state_ == Wt::Checked;
  }

  /*! \brief Returns the current value.
   *
   * Returns "yes" when checked and  "no" when unchecked.
   */
  WT_USTRING valueText() const override {
    if(state_ == Wt::Checked) return "yes";
    else return "yes";
  }

  /*! \brief Sets the current value.
   *
   * This interprets text values of "yes" or "no".
   */
  void setValueText(const WT_USTRING& text) override {
    if(text == "yes") setChecked(true);
    else if(text == "no") setChecked(false);
  }

  /*! \brief %Signal emitted when the button gets checked.
   *
   * This signal is emitted when the user checks the button.
   *
   * You can use the changed() signal to react to any change of the
   * button state.
   */
  Wt::EventSignal<>& checked() {
    return *voidEventSignal(CHECKED_SIGNAL, true);
  }

  /*! \brief %Signal emitted when the button gets un-checked.
   *
   * This signal is emitted when the user unchecks the button.
   *
   * You can use the changed() signal to react to any change of the
   * button state.
   */
  Wt::EventSignal<>& unChecked() {
    return *voidEventSignal(UNCHECKED_SIGNAL, true);
  }

  void refresh() override {
    WFormWidget::refresh();
  }

 protected:
  virtual void updateChecked(Wt::DomElement& element,
                             Wt::CheckState state)
  {
    element.setProperty(Wt::PropertyChecked,
                        state == Wt::Unchecked ? "false" : "true");
  }

  Wt::DomElementType domElementType() const override {
    return Wt::DomElement_INPUT;
  }

  void updateDom(Wt::DomElement& element, bool all) override {
    if(all) element.setAttribute("type", "checkbox");

    Wt::EventSignal<> *check = voidEventSignal(CHECKED_SIGNAL, false);
    Wt::EventSignal<> *uncheck = voidEventSignal(UNCHECKED_SIGNAL, false);
    Wt::EventSignal<> *change = voidEventSignal(CHANGE_SIGNAL, false);
    Wt::EventSignal<Wt::WMouseEvent> *click =
      mouseEventSignal(M_CLICK_SIGNAL, false);

    // We piggy-back the checked and uncheck signals on the change signal.
    // If agent is IE, then we piggy-back the change on the clicked signal.
    Wt::WApplication *app = Wt::WApplication::instance();
    const Wt::WEnvironment& env = app->environment();
    bool piggyBackChangeOnClick = env.agentIsIE();

    bool needUpdateChangeSignal =
      (change && change->needsUpdate(all))
      || (check && check->needsUpdate(all))
      || (uncheck && uncheck->needsUpdate(all));

    bool needUpdateClickedSignal =
      (click && click->needsUpdate(all))
      || (piggyBackChangeOnClick && needUpdateChangeSignal);

    WFormWidget::updateDom(element, all);

    if(flags_.test(BIT_STATE_CHANGED) || all) {
      // element.setProperty(Wt::PropertyChecked,
      //                     state_ == Wt::Unchecked ? "false" : "true");
      updateChecked(element, state_);
      flags_.reset(BIT_STATE_CHANGED);
    }

    std::vector<Wt::DomElement::EventAction> changeActions;

    if(needUpdateChangeSignal ||
       (piggyBackChangeOnClick && needUpdateClickedSignal) ||
       all)
    {
      std::string dom = "o";

      if(check) {
        if(check->isConnected()) {
          changeActions.push_back(
            Wt::DomElement::EventAction(dom + ".checked",
                                        check->javaScript(),
                                        check->encodeCmd(),
                                        check->isExposedSignal()));
        }
        check->updateOk();
      }

      if(uncheck) {
        if(uncheck->isConnected()) {
          changeActions.push_back(
            Wt::DomElement::EventAction("!" + dom + ".checked",
                                        uncheck->javaScript(),
                                        uncheck->encodeCmd(),
                                        uncheck->isExposedSignal()));
        }
        uncheck->updateOk();
      }

      if(change) {
        if(change->isConnected()) {
          changeActions.push_back(
            Wt::DomElement::EventAction(std::string(),
                                        change->javaScript(),
                                        change->encodeCmd(),
                                        change->isExposedSignal()));
        }
        change->updateOk();
      }

      if(!piggyBackChangeOnClick) {
        if(!(all && changeActions.empty()))
          element.setEvent("change", changeActions);
      }
    }

    if(needUpdateClickedSignal || all) {
      if(piggyBackChangeOnClick) {
        if(click) {
          changeActions.push_back(
            Wt::DomElement::EventAction(std::string(),
                                        click->javaScript(),
                                        click->encodeCmd(),
                                        click->isExposedSignal()));
          click->updateOk();
        }

        if(!(all && changeActions.empty())) {
          element.setEvent(CLICK_SIGNAL, changeActions);
        }
      }
      else if(click) {
        updateSignalConnection(element, *click, CLICK_SIGNAL, all);
      }
    }
  }

  void getFormObjects(FormObjectsMap& formObjects) override {
    formObjects[formName()] = this;
  }

  std::string formName() const override {
    return WFormWidget::formName();
  }

  void setFormData(const FormData& formData) override {
    if(flags_.test(BIT_STATE_CHANGED) || isReadOnly()) return;
    if(!formData.values.empty()) {
      state_ = formData.values[0] != "0" ? Wt::Checked : Wt::Unchecked;
    }
    else if(isEnabled() && isVisible()) state_ = Wt::Unchecked;
  }

  void propagateRenderOk(bool deep) override {
    flags_.reset(BIT_STATE_CHANGED);
    Wt::EventSignal<> *check = voidEventSignal(CHECKED_SIGNAL, false);
    if(check) check->updateOk();
    Wt::EventSignal<> *uncheck = voidEventSignal(UNCHECKED_SIGNAL, false);
    if(uncheck) uncheck->updateOk();
    WInteractWidget::propagateRenderOk(deep);
  }

 private:
  Wt::CheckState state_ {Wt::Unchecked};

  static const int BIT_STATE_CHANGED = 0;
  // static const int BIT_TEXT_CHANGED = 1;

  std::bitset<1> flags_;

  static constexpr const char *CHECKED_SIGNAL = "M_checked";
  static constexpr const char *UNCHECKED_SIGNAL = "M_unchecked";
  static constexpr const char *CLICK_SIGNAL = "click";
};

} // namespace wtx

} // namespace rh

#endif // __rh_wtx_SimpleCheckBox_hpp__
