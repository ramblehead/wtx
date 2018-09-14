// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_wtx_PostInitWidget_hpp__
#define __rh_wtx_PostInitWidget_hpp__

namespace rh {

namespace wtx {

template <typename T>
class PostInitWidget : public T
{
 private:
  using Base = T;

 public:
  using FormData = typename Base::FormData;

  template <typename... Ts>
  PostInitWidget(const Wt::Http::ParameterValues& initialFormDataValues,
                 Ts&&... params)
      : Base(std::forward<Ts>(params)...),
        m_initialFormDataValues(initialFormDataValues),
        m_ignoringAnyFormData(false),
        m_ignoringInitialFormData(true)
  {}

 protected:
  void setIgnoringAnyFormData(bool value = true) {
    m_ignoringAnyFormData = value;
  }

  void unsetIgnoringAnyFormData() {
    setIgnoringAnyFormData(false);
  }

  bool ignoringAnyFormData() {
    return m_ignoringAnyFormData;
  }

  void setIgnoringInitialFormData(bool value = true) {
    m_ignoringInitialFormData = value;
  }

  void unsetIgnoringInitialFormData() {
    setIgnoringInitialFormData(false);
  }

  bool ignoringInitialFormData() {
    return m_ignoringInitialFormData;
  }

  void setInitialFormDataValues(const Wt::Http::ParameterValues& value) {
    m_initialFormDataValues = value;
  }

  const Wt::Http::ParameterValues& initialFormDataValues() const {
    return m_initialFormDataValues;
  }

  void setFormData(const FormData& formData) override {
    if(m_ignoringAnyFormData) return;

    // Ignore empty forms
    if(formData.values.empty()) return;

    // Ignore initial value comming from browser's formData
    if(m_ignoringInitialFormData &&
       formData.values == m_initialFormDataValues)
    {
      return;
    }

    Base::setFormData(formData);
  }

 private:
  Wt::Http::ParameterValues m_initialFormDataValues;
  bool m_ignoringAnyFormData;
  bool m_ignoringInitialFormData;
};

} // namespace wtx

} // namespace rh

#endif // __rh_wtx_PostInitWidget_hpp__
