#include "Renderer.h"

INITIALIZE_EASYLOGGINGPP;

int main(int ac, char **av)
{
    //el::Configurations c;
    //c.setToDefault();
    //c.set(el::Level::Info, el::ConfigurationType::ToStandardOutput, "false");
    //el::Loggers::reconfigureLogger("default", c);
	START_EASYLOGGINGPP(ac, av);
	Dwarf::Renderer renderer;
}
