// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __JavaScriptWidget_hpp__
#define __JavaScriptWidget_hpp__

#include <utility>
#include <string>

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WJavaScript>
#include <Wt/WLogger>

#include "reflection/refactorables.hpp"
#include "utils/RH_CODE_POINT.hpp"

namespace rh {

namespace wtx {

template<typename T>
class JavaScriptWidget : public T {
 private:
  using Base = T;
  using This = JavaScriptWidget;

  using SetWtRendered = Wt::JSignal<void>;
  using SetWtRenderedSPtr = std::shared_ptr<SetWtRendered>;
  using SetWtRenderedWPtr = std::weak_ptr<SetWtRendered>;

 public:
  template<typename... Ts>
  JavaScriptWidget(Ts&&... params)
    : Base(std::forward<Ts>(params)...),
      m_setWtRenderedSPtr(
        std::make_shared<SetWtRendered>(this, RH_N(m_setWtRenderedSPtr)))
  {}

  void render(Wt::WFlags<Wt::RenderFlag> flags) override {
    Base::render(flags);
    if(flags & Wt::RenderFull) {
      if(!m_setWtRenderedSPtr->isConnected()) {
        m_setWtRenderedSPtr->connect(this, &This::setWtRenderedHandler);
      }
      m_renderedAtBrowser = false;
      runRenderAtBrowser();
    }
  }

  void refresh() override {
    Base::refresh();
    m_renderedAtBrowser = false;
    runRenderAtBrowser();
  }

 protected:
  static constexpr int renderAtBrowserAttemptsMax = 300;
  static constexpr int renderAtBrowserAttemptIntervalMilliSecond = 300;

  // This widget ID. By default, it is this->Base::id(). This virtual member
  // function might be overridden in case of composite widgets, where it might
  // be useful to check one of its children for rendering, rather than self.
  virtual std::string renderedAtBrowserId() const {
    return Base::id();
  }

  virtual void afterRenderedAtBrowser() {}

  virtual std::string renderAtBrowserJavaScriptStatement() {
    return std::string();
  }

  bool renderedAtBrowser() const {
    return m_renderedAtBrowser;
  }

  std::string renderedAtBrowserJavaScriptPredicate() const {
    auto id = renderedAtBrowserId();
    auto rendered = RH_N(m_renderedAtBrowser);
    return
      "($('#" + id + "').prop('"+ rendered + "') !== undefined)";
  }

  virtual void renderAtBrowserFailed() {
    std::string errorMessage =
      RH_CODE_POINT + ": Browser render attempts " +
      "for id = '" + renderedAtBrowserId() + "' failed " +
      std::to_string(renderAtBrowserAttemptsMax) + " times.";
    Wt::WApplication::instance()->log("error") << errorMessage;
  }

 private:
  void runRenderAtBrowser() {
    if(m_renderedAtBrowser) {
      m_runRenderAtBrowserAttemptsCount = 0;
    }
    else {
      ++m_runRenderAtBrowserAttemptsCount;
      if(m_runRenderAtBrowserAttemptsCount <=
         renderAtBrowserAttemptsMax)
      {
        renderAtBrowser();
        SetWtRenderedWPtr setWtRefreshedWPtr = m_setWtRenderedSPtr;
        Wt::WServer::instance()->schedule(
          renderAtBrowserAttemptIntervalMilliSecond,
          Wt::WApplication::instance()->sessionId(),
          [this, setWtRefreshedWPtr]() {
            auto setWtRefreshedSPtr = setWtRefreshedWPtr.lock();
            if(setWtRefreshedSPtr == nullptr) return;
            runRenderAtBrowser();
          });
      }
      else renderAtBrowserFailed();
    }
  }

  void renderAtBrowser() {
    std::string id = renderedAtBrowserId();
    std::string rendered = RH_N(m_renderedAtBrowser);
    Base::doJavaScript(
      "setTimeout(function() {\n"
      "  if(document.getElementById('" + id + "')) {\n"
      "    if($('#" + id + "').prop('"+ rendered + "') === undefined) {\n"
      "      $('#" + id + "').prop('" + rendered + "', true);\n"
      "      (function() {"
      "         " + renderAtBrowserJavaScriptStatement() + ";\n"
      "      }());\n"
      "      " + m_setWtRenderedSPtr->createCall() + ";\n"
      "    }\n"
      "  }\n"
      "}, 0);\n");
  }

  SetWtRenderedSPtr m_setWtRenderedSPtr;
  void setWtRenderedHandler() {
    // Just in case, disabling stateless.
    Base::isNotStateless();
    m_renderedAtBrowser = true;
    afterRenderedAtBrowser();
  }

  bool m_renderedAtBrowser {false};
  int m_runRenderAtBrowserAttemptsCount {0};
};

} // namespace wtx

} // namespace rh

#endif // __JavaScriptWidget_hpp__
