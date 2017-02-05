#ifndef CONFIG_HPP
#define CONFIG_HPP

struct Config {
    int detector_id = 0;
    const char* rdv_ip = "127.0.0.1";
    unsigned short rdv_port = 1234;
};

#endif//CONFIG_HPP
