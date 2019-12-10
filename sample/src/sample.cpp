#include <stdio.h>

#include "wii_main.h"

extern "C" {

/**
 * The different types of nodes in the menu
 */
enum NODETYPE { NODETYPE_ROOT = 0, NODETYPE_SPACER, NODETYPE_ROM };

/**
 * Initializes the application
 */
void wii_handle_init() {}

/**
 * Runs the application (main loop)
 */
void wii_handle_run() {
    // Show the menu
    wii_menu_show();
}

/**
 * Returns the base application directory
 *
 * @return  The base application directory
 */
const char* wii_get_app_base_dir() {
    return "sd:/apps/mysample/";
}

/***
 * Frees resources prior to the application exiting
 */
void wii_handle_free_resources() {}

//
// Menu-related methods
//

/**
 * Determines whether the node is currently visible
 *
 * @param   node The node
 * @return  Whether the node is visible
 */
BOOL wii_menu_handle_is_node_visible(TREENODE* node) {
    return TRUE;
}

/**
 * Updates the buffer with the header message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the header message for the current
 *          menu.
 */
void wii_menu_handle_get_header(TREENODE* menu, char* buffer) {
    snprintf(buffer, WII_MENU_BUFF_SIZE, "Some header...");
}

/**
 * The spacer node type
 *
 * @return  The spacer node type
 */
int wii_get_nodetype_spacer() {
    return NODETYPE_SPACER;
}

/**
 * The rom node type
 *
 * @return  The rom node type
 */
int wii_get_nodetype_rom() {
    return NODETYPE_ROM;
}

/**
 * Invoked after exiting the menu loop
 */
void wii_menu_handle_post_loop() {}

/**
 * Invoked when the home button is pressed when the menu is being displayed
 */
void wii_menu_handle_home_button() {}

/**
 * Invoked prior to entering the menu loop
 */
void wii_menu_handle_pre_loop() {}

/**
 * Provides an opportunity for the specified menu to do something during a
 * display cycle.
 *
 * @param   menu The menu
 */
void wii_menu_handle_update(TREENODE* menu) {}

/**
 * React to the "select" event for the specified node
 *
 * @param   node The node
 */
void wii_menu_handle_select_node(TREENODE* node) {}

/**
 * Updates the buffer with the name of the specified node
 *
 * @param   node The node
 * @param   name The name of the specified node
 * @param   value The value of the specified node
 */
void wii_menu_handle_get_node_name(TREENODE* node, char* buffer, char* value) {}

/**
 * Updates the buffer with the footer message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the footer message for the current
 *          menu.
 */
void wii_menu_handle_get_footer(TREENODE* menu, char* buffer) {}

/**
 * Determines whether the node is selectable
 *
 * @param   node The node
 * @return  Whether the node is selectable
 */
BOOL wii_menu_handle_is_node_selectable(TREENODE* node) {
    return FALSE;
}

}