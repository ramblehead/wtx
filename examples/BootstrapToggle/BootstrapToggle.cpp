// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
// compile: g++ -std=c++14 BootstrapToggle.cpp -o BootstrapToggle -I../../server -I/home/rh/s600/s600-host/build/root/include -L /home/rh/s600/s600-host/build/root/lib -g -ggdb -lwt -lwthttp -Wl,-rpath=.

#include <Wt/WServer>

#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WContainerWidget>

#include "wtx/BootstrapToggle.hpp"

using namespace rh;

class BootstrapToggleExampleApplication : public Wt::WApplication {
 public:
  BootstrapToggleExampleApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
  {
    requireJQuery(Wt::WLink("wtx/js/jquery-3.1.0.min.js").url());

    auto* theme = new Wt::WBootstrapTheme();
    theme->setVersion(Wt::WBootstrapTheme::Version3);
    setTheme(theme);

    setTitle("Wt Bootstrap Toggle Example");

    auto* mainContainer = new Wt::WContainerWidget();

    // using State = wtx::BootstrapToggle::State;
    // m_bootstrapToggle = new wtx::BootstrapToggle(nullptr, State::on);
    m_bootstrapToggle = new wtx::BootstrapToggle();

    m_bootstrapToggle->checked().connect([this](...) {
      log("notice") << "*** bootstrapToggle checked ***";
    });

    m_bootstrapToggle->unChecked().connect([this](...) {
      log("notice") << "*** bootstrapToggle unchecked ***";
    });

    mainContainer->addWidget(m_bootstrapToggle);

    root()->addWidget(mainContainer);

    // m_bootstrapToggle->disable();
  }

 private:
  wtx::BootstrapToggle* m_bootstrapToggle;
};

Wt::WApplication *createApplication(const Wt::WEnvironment& env) {
  Wt::WApplication *app = new BootstrapToggleExampleApplication(env);
  return app;
}

int main(int argc, char **argv) {
  Wt::WString::setDefaultEncoding(Wt::CharEncoding::UTF8);
  return Wt::WRun(argc, argv, &createApplication);
}

// Emacs, here are file hints,
// The following lompile command assumes that libwt and libwthttp
// "so" files can be located in "./" (i.e. -rpath=.)
// Local Variables:
// compile-command: "g++ -std=c++14 -Wall -g -ggdb BootstrapToggle.cpp -o BootstrapToggle -I../../server -I../../../s600-host/build/root/include -L../../../s600-host/build/root/lib -lwt -lwthttp -Wl,-rpath=."
// End:
