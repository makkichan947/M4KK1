#include "../include/editor.h"
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

// Plugin list
Plugin *plugins = NULL;

// Initialize plugin system
void pluginInit() {
    plugins = NULL;
}

// Load a plugin from path
int pluginLoad(const char *path) {
    // Check if plugin already loaded
    Plugin *existing = pluginGet(path);
    if (existing) {
        editorSetStatusMessage("Plugin %s already loaded", path);
        return 0;
    }

    // Create plugin structure
    Plugin *plugin = malloc(sizeof(Plugin));
    if (!plugin) {
        editorSetStatusMessage("Failed to allocate memory for plugin");
        return 0;
    }

    plugin->name = malloc(strlen(path) + 1);
    plugin->path = malloc(strlen(path) + 1);
    if (!plugin->name || !plugin->path) {
        free(plugin->name);
        free(plugin->path);
        free(plugin);
        editorSetStatusMessage("Failed to allocate memory for plugin paths");
        return 0;
    }

    strcpy(plugin->name, path);
    strcpy(plugin->path, path);
    plugin->handle = NULL;
    plugin->enabled = 1;
    plugin->next = plugins;
    plugins = plugin;

    editorSetStatusMessage("Plugin %s loaded", path);
    return 1;
}

// Unload a plugin by name
void pluginUnload(const char *name) {
    Plugin *prev = NULL;
    Plugin *curr = plugins;

    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            // Remove from list
            if (prev) {
                prev->next = curr->next;
            } else {
                plugins = curr->next;
            }

            // Close dynamic library if loaded
            if (curr->handle) {
                dlclose(curr->handle);
            }

            // Free memory
            free(curr->name);
            free(curr->path);
            free(curr);

            editorSetStatusMessage("Plugin %s unloaded", name);
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    editorSetStatusMessage("Plugin %s not found", name);
}

// Get plugin by name
Plugin *pluginGet(const char *name) {
    Plugin *plugin = plugins;
    while (plugin) {
        if (strcmp(plugin->name, name) == 0) {
            return plugin;
        }
        plugin = plugin->next;
    }
    return NULL;
}

// Call plugin hook
void pluginCallHook(PluginHookType hook, void *data) {
    Plugin *plugin = plugins;
    while (plugin) {
        if (plugin->enabled && plugin->handle) {
            // Try to call hook function
            char hook_name[64];
            switch (hook) {
                case HOOK_INIT:
                    strcpy(hook_name, "plugin_init");
                    break;
                case HOOK_KEYPRESS:
                    strcpy(hook_name, "plugin_keypress");
                    break;
                case HOOK_MODE_CHANGE:
                    strcpy(hook_name, "plugin_mode_change");
                    break;
                case HOOK_FILE_OPEN:
                    strcpy(hook_name, "plugin_file_open");
                    break;
                case HOOK_FILE_SAVE:
                    strcpy(hook_name, "plugin_file_save");
                    break;
                case HOOK_BUFFER_CHANGE:
                    strcpy(hook_name, "plugin_buffer_change");
                    break;
                case HOOK_QUIT:
                    strcpy(hook_name, "plugin_quit");
                    break;
                default:
                    continue;
            }

            PluginHook hook_func = (PluginHook)dlsym(plugin->handle, hook_name);
            if (hook_func) {
                hook_func(data);
            }
        }
        plugin = plugin->next;
    }
}

// List all loaded plugins
void pluginList() {
    Plugin *plugin = plugins;
    if (!plugin) {
        editorSetStatusMessage("No plugins loaded");
        return;
    }

    char message[256] = "Plugins: ";
    int len = strlen(message);

    while (plugin) {
        if (len + strlen(plugin->name) + 2 < sizeof(message)) {
            if (plugin != plugins) {
                strcat(message, ", ");
                len += 2;
            }
            strcat(message, plugin->name);
            len += strlen(plugin->name);
        }
        plugin = plugin->next;
    }

    editorSetStatusMessage("%s", message);
}

// Load plugins from directory
void pluginLoadFromDir(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        return; // Directory doesn't exist or can't be opened
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check for .so files (shared libraries)
        char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".so") == 0) {
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            // Try to load the plugin
            pluginLoad(full_path);
        }
    }

    closedir(dir);
}