//
// Spreadtrum Wifi Tester
//
// for linux
//

class SoftapAPI {

public:
    SoftapAPI();
    int softapOpen( void );
    int softapClose( void );
    int softapSet(char ssid[], char psk[]);
    int softapSetMoreParam(int argc, char *argv[]);

private:
    static void * softapEventLoop( void *param );
};