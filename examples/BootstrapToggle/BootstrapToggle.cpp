// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
// compile: g++ hello_wt.cpp -o hello_wt -Wall -Wextra -Werror -std=c++11 -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -lwthttp -lwt -lboost_signals
// run: ./BootstrapToggle --docroot ../../docroot --http-address 0.0.0.0 --http-port 8080
// browser: http://localhost:8080/
//      or: http://127.0.0.1:8080/

#include <Wt/WBootstrapTheme>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

#include <Wt/WPushButton>
#include <Wt/WCheckBox>

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

    auto *mainContainer = new Wt::WContainerWidget();

    m_bootstrapToggle = new wtx::BootstrapToggle();
    mainContainer->addWidget(m_bootstrapToggle);

    root()->addWidget(mainContainer);
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
// Local Variables:
// The following lompile command assumes that libwt and libwthttp
// "so" files can be located in "./"
// compile-command: "g++ -std=c++14 BootstrapToggle.cpp -o BootstrapToggle -I../../server -I/home/rh/s600/s600-host/build/root/include -L /home/rh/s600/s600-host/build/root/lib -g -ggdb -lwt -lwthttp -Wl,-rpath=."
// End:
