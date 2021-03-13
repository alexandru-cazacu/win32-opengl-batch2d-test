/// Engine startup configuration read from .hyperrc
typedef struct {
    HyWindowStartMode startMode;
    char* user;
    
    struct theme {
        uint32_t background;
    } theme;
} HyConfig;

internal int configHandler(void* user, const char* section, const char* name, const char* value)
{
    HyConfig* config = (HyConfig*)user;
    
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    
    if (MATCH("window", "startMode")) {
        if (strcmp(value, "auto") == 0) {
            config->startMode = HyWindowStartMode_Auto;
        } else if (strcmp(value, "maximized") == 0) {
            config->startMode = HyWindowStartMode_Maximized;
        } else if (strcmp(value, "fullscreen") == 0) {
            config->startMode = HyWindowStartMode_Fullscreen;
        }
    } else if (MATCH("theme", "background")) {
        //config->theme.background = strdup(value);
        config->theme.background = 0xff00ffff;
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

internal HyConfig* hy_config_init()
{
    HyConfig* config = hy_malloc(sizeof(HyConfig));
    memset(config, 0, sizeof(HyConfig));
    
    config->startMode = HyWindowStartMode_Auto;
    
    HyFile* configFile = hy_read_file(".hypedrc");
    if (configFile) {
        ini_parse_string(configFile->data, configHandler, config);
        HY_INFO("Config loaded from '.hypedrc'");
        HY_INFO("  - startMode=%d", config->startMode);
        HY_INFO("  - user=%s", config->user);
        HY_INFO("  - theme");
        HY_INFO("    - bg=%x", config->theme.background);
    } else {
        HY_ERROR(".hypedrc not found");
    }
    
    hy_close_file(configFile);
    
    return config;
}

internal void hy_config_deinit(HyConfig* config)
{
    hy_free(config);
}