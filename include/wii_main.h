//---------------------------------------------------------------------------//
//                                                                           //
//  wii-emucommon:                                                           //
//  Wii emulator common code                                                 //
//                                                                           //
//  [github.com/raz0red/wiicolem]                                            //
//                                                                           //
//---------------------------------------------------------------------------//
//                                                                           //
//  Copyright (C) 2019 raz0red                                               //
//                                                                           //
//  This program is free software; you can redistribute it and/or            //
//  modify it under the terms of the GNU General Public License              //
//  as published by the Free Software Foundation; either version 2           //
//  of the License, or (at your option) any later version.                   //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA            //
//  02110-1301, USA.                                                         //
//---------------------------------------------------------------------------//

#ifndef WII_MAIN_H
#define WII_MAIN_H

#include <gccore.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <unistd.h>

#include "wii_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WII_WIDTH 640
#define WII_HEIGHT 480

struct treenode;
typedef struct treenode *TREENODEPTR;

/**
 * Simple hierarchical menu structure
 */
typedef struct treenode {
    char* name;
    int node_type;
    TREENODEPTR* children;
    u16 child_count;
    u16 max_children;
} TREENODE;

#define WII_MAX_PATH MAXPATHLEN + 1
#define WII_MENU_BUFF_SIZE 256

// vsync modes
#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1

// widescreen modes
#define WS_DISABLED 0
#define WS_ENABLED  1
#define WS_AUTO     2

/** The root of the menu */
extern TREENODE *wii_menu_root;
/** Whether the wiimote (for the menu) is horizontal or vertical */
extern BOOL wii_mote_menu_vertical;
/** Whether to return to the top menu after exiting a game */
extern BOOL wii_top_menu_exit;
/** The vsync mode */
extern int wii_vsync;
/** The stack containing the menus the user has navigated */
extern TREENODEPTR wii_menu_stack[64];
/** The head of the menu stack */
extern s8 wii_menu_stack_head;
/** Whether to quit the menu loop */
extern BOOL wii_menu_quit_loop;
/** Forces the menu to be redrawn */
extern BOOL wii_menu_force_redraw;
/** The last rom that was successly executed */
extern char *wii_last_rom;
/** Offset of the selection bar in the menu */
extern int wii_menu_sel_offset;
/** The menu selection color */
extern RGBA wii_menu_sel_color;
/** Whether the Wii is PAL or NTSC */
extern BOOL wii_is_pal;
/** Double strike mode */
extern BOOL wii_double_strike_mode;
/** Whether the WII is in widescreen mode */
extern BOOL is_widescreen;
/** Full widescreen */
extern int wii_full_widescreen;
/** USB keep alive */
extern BOOL wii_usb_keepalive;
/** Trap filter */
extern BOOL wii_trap_filter;
/** 16:9 correction */
extern BOOL wii_16_9_correction;

/** The display mode (from SDL) */
extern GXRModeObj *vmode;
/** Frame buffers (from SDL) */
extern unsigned int *xfb[2];

/**
 * Creates and returns a new menu tree node
 *
 * @param   type The type of the node
 * @param   name The name for the node
 * @return  The newly created node
 */
TREENODE* wii_create_tree_node(int type, const char* name);

/**
 * Attempts to find the tree node with the specified type
 *
 * @param   root Where to start the search
 * @param   type The type of the node
 * @return  The found tree node or NULL
 */
TREENODE* wii_find_tree_node(TREENODE* root, int type);

/**
 * Clears the children for the specified node
 *
 * @param   node The node to clear the children for
 */
void wii_menu_clear_children(TREENODE* node);

/**
 * Adds the specified child node to the specified parent
 *
 * @param   parent The parent
 * @param   child The child to add to the parent
 */
void wii_add_child(TREENODE* parent, TREENODE* childp);

/**
 * Pushes the specified menu onto the menu stack (occurs when the user navigates
 * to a sub-menu)
 *
 * @param   menu The menu to push on the stack
 */
void wii_menu_push(TREENODE* menu);

/**
 * Resets the menu indexes when an underlying menu in change (push/pop)
 */
void wii_menu_reset_indexes();

/**
 * Move the current selection in the menu by the specified step count
 *
 * @param   menu The current menu
 * @param   steps The number of steps to move the selection
 */
void wii_menu_move(TREENODE* menu, s16 steps);

/**
 * Pops the latest menu off of the menu stack (occurs when the user leaves the
 * current menu.)
 *
 * @return  The pop'd menu
 */
TREENODE* wii_menu_pop();

/**
 * Used for comparing menu names when sorting (qsort)
 *
 * @param   a The first tree node to compare
 * @param   b The second tree node to compare
 * @return  The result of the comparison
 */
int wii_menu_name_compare(const void* a, const void* b);

/**
 * Returns the standard list footer for snapshots, games, etc.
 *
 * @param   menu The current menu
 * @param   listname The name of the list (snapshot/game, etc.)
 * @param   listnameplural The name of the list in plural form (snapshots/games,
 *          etc.)
 * @param   buffer The buffer to write the footer to
 */
void wii_get_list_footer(TREENODE* menu,
                         const char* listname,
                         const char* listnameplural,
                         char* buffer);

/**
 * Sets whether to enable VSYNC or not
 *
 * @param   param sync Whether to enable VSYNC or not
 */
void wii_set_vsync(BOOL sync);

/**
 * Syncs the video
 */
void wii_sync_video();

/**
 * Returns whether VSYNC is enabled
 *
 * @return  Whether VSYNC is enabled
 */
int wii_vsync_enabled();

/**
 * Displays the menu
 */
void wii_menu_show();

/**
 * Returns the current menu index
 *
 * @return  The current menu index
 */
s16 wii_menu_get_current_index();

//
// Methods to be implemented by application
//

/**
 * Initializes the application
 */
void wii_handle_init();

/**
 * Frees resources prior to the application exiting
 */
void wii_handle_free_resources();

/**
 * Runs the application (main loop)
 */
void wii_handle_run();

/**
 * Returns the (current) max frame rate
 *
 * @return  The (current) max frame rate
 */
u8 wii_get_max_frames();

/**
 * Updates the buffer with the header message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the header message for the current
 *          menu.
 */
void wii_menu_handle_get_header(TREENODE* menu, char* buffer);

/**
 * Updates the buffer with the footer message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the footer message for the current
 *          menu.
 */
void wii_menu_handle_get_footer(TREENODE* menu, char* buffer);

/**
 * Updates the buffer with the name of the specified node
 *
 * @param   node The node
 * @param   name The name of the specified node
 * @param   value The value of the specified node
 */
void wii_menu_handle_get_node_name(TREENODE* node, char* name, char* value);

/**
 * React to the "select" event for the specified node
 *
 * @param   node The node
 */
void wii_menu_handle_select_node(TREENODE* node);

/**
 * Determines whether the node is currently visible
 *
 * @param   node The node
 * @return   Whether the node is visible
 */
BOOL wii_menu_handle_is_node_visible(TREENODE* node);

/**
 * Provides an opportunity for the specified menu to do something during a
 * display cycle.
 *
 * @param   menu The menu
 */
void wii_menu_handle_update(TREENODE* menu);

/**
 * Invoked prior to entering the menu loop
 */
void wii_menu_handle_pre_loop();

/**
 * Invoked after exiting the menu loop
 */
void wii_menu_handle_post_loop();

/**
 * Invoked when the home button is pressed when the  menu is being displayed
 */
void wii_menu_handle_home_button();

/**
 * Determines whether the node is selectable
 *
 * @param   node The node
 * @return  Whether the node is selectable
 */
BOOL wii_menu_handle_is_node_selectable(TREENODE* node);

/**
 * Returns the space node type
 *
 * @return  The spacer node type
 */
int wii_get_nodetype_spacer();

/**
 * Returns the rom node type
 *
 * @return  The rom node type
 */
int wii_get_nodetype_rom();

extern const u8 about_png_end[];
extern const u8 about_png[];
extern const u32 about_png_size;

#ifdef __cplusplus
}
#endif

#endif
