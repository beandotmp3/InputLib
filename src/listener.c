/*
 * listener.c - Input capturing and blocking
 *
 * Provides functions for capturing and manipulating inputs
 * 
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inputlib.h"

/* Define event poll queue length */
#define EVENT_QUEUE_CAPACITY 512

/* Define modifier bit shifts */
#define L_MOD_SHIFT (1 << 0)
#define L_MOD_CTRL (1 << 1)
#define L_MOD_ALT (1 << 2)
#define L_MOD_WIN (1 << 3)

/* 
 * vKey -  Structure mapping key names to virtual key codes
 */
typedef struct {
    const char* name;
    BYTE code; /* Windows vitrual key code */
} vKey;

/*
 * keymap - Maps human-readable key names to Windows virtual key codes
 *
 * This lookup table allows users to specify keys by name (e.g., "A", "ENTER")
 * instead of using numeric vitrual key codes directly. 
 */
static const vKey keymap[] = {
    /* Alphabetic keys A-Z (0x41-0x5A) */
    {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
    {"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
    {"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
    {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},

    /* Number keys 0-9 (0x30-0x39) and their shifted symbols */
    {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36},
    {"7", 0x37}, {"8", 0x38}, {"9", 0x39}, {"!", 0x31}, {"@", 0x32}, {"#", 0x33}, {"$", 0x34},
    {"%", 0x35}, {"^", 0x36}, {"&", 0x37}, {"*", 0x38}, {"(", 0x39}, {")", 0x30}, 

    /* Punctuation and symbol keys */
    {"`", 0xC0}, {"~", 0xC0}, {"-", 0xBD}, {"_", 0xBD}, {"=", 0xBB}, {"+", 0xBB}, {"[", 0xDB}, {"{", 0xDB}, {"]", 0xDD}, {"}", 0xDD}, {"\\", 0xDC},
    {"|", 0xDC}, {";", 0xBA}, {":", 0xBA}, {"'", 0xDE}, {"\"", 0xDE}, {",", 0xBC}, {"<", 0xBC}, {".", 0xBE}, {">", 0xBE}, {"/", 0xBF}, {"?", 0xBF},

    /* Special and navigation keys */
    {" ", 0x20}, {"SPACE", 0x20}, {"BACKSPACE", 0x08}, {"DELETE", 0x2E}, {"TAB", 0x09},
    {"ENTER", 0x0D}, {"ESCAPE", 0x1B}, {"HOME", 0x24}, {"END", 0x23}, {"INSERT", 0x2D},
    {"PAGEUP", 0x21}, {"PAGEDOWN", 0x22}, {"LEFT", 0x25}, {"UP", 0x26}, {"RIGHT", 0x27},
    {"DOWN", 0x28}, {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"ALT", 0x12}, {"WIN", 0x5B},

    /* Function keys F1-F12 (0x70-0x7B) */
    {"F1", 0x70}, {"F2", 0x71}, {"F3", 0x72}, {"F4", 0x73}, {"F5", 0x74}, {"F6", 0x75},
    {"F7", 0x76}, {"F8", 0x77}, {"F9", 0x78}, {"F10", 0x79}, {"F11", 0x7A}, {"F12", 0x7B}
};

/* Precalculate array length as a constant */
static const size_t keymap_count = sizeof(keymap) / sizeof(keymap[0]);


/*
 * find_vk - Look up virtual key code by key name
 * 
 * @key: Key name to search for (case-insensitive)
 * 
 * Searches the keymap for the matching key name and returns its virtual key code.
 * The search is case-insensitive ("a" and "A" both work).
 * 
 * Returns: Virtual key code (BYTE), or 0 if key not found
 */
static BYTE find_vk(const char* key) {
    if(!key) return 0; /* Null key name */

    /* Linear search through keymap for matching name */
    for(size_t i = 0; i < keymap_count; ++i) {
        if(_stricmp(keymap[i].name, key) == 0) return keymap[i].code;
    }

    /* Fallback if search is for single character in lowercase */
    if(strlen(key) == 1) {
        char s[2] = { (char)toupper((unsigned char)key[0]), 0 };
        for(size_t i = 0; i < keymap_count; ++i) {
            if(_stricmp(keymap[i].name, s) == 0) return keymap[i].code;
        }
    }
    return 0; /* Key not found */
}

typedef enum {
    GROUP_LETTERS = 0,
    GROUP_NUMBERS,
    GROUP_FUNCTION,
    GROUP_NAVIGATION,
    GROUP_MODIFIERS,
    GROUP_NUMPAD, /* reserved but not fully populated */
    GROUP_COUNT
} GroupId;

static const char* group_names[GROUP_COUNT] = {
    "LETTERS", "NUMBERS", "FUNCTION", "NAVIGATION", "MODIFIERS", "NUMPAD"
};

static int vk_in_group(BYTE vk, GroupId gid) {
    switch(gid) {
        case GROUP_LETTERS:
            return (vk >= 0x41 && vk <= 0x5A);
        case GROUP_NUMBERS:
            return (vk >= 0x30 && vk <= 0x39);
        case GROUP_FUNCTION:
            return (vk >= 0x70 && vk <= 0x7B);
        case GROUP_NAVIGATION:
            return (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN ||
                vk == VK_HOME || vk == VK_END || vk == VK_PRIOR || vk == VK_NEXT);
        case GROUP_MODIFIERS:
            return (vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU || vk == VK_LWIN || vk == VK_RWIN);
        case GROUP_NUMPAD:
            return (vk >= VK_NUMPAD0 && vk <= VK_DIVIDE); // approximate
        default:
            return 0;
    }
}

static int find_group_id(const char* name) {
    if(!name) return -1;

    for(int i = 0; i < GROUP_COUNT; ++i) {
        if(_stricmp(name, group_names[i]) == 0) return i;
    }
    return -1;
}

/*
 * Structure containing key information of a combo
 */
typedef struct ComboNode {
    BYTE* mods;                /* Array of modifier key codes */
    int mod_count;             /* Number of modifers in the array */
    BYTE key;                  /* Primary key code for combo */
    struct ComboNode* next;    /* Next combo in linked list */
} ComboNode;


static HHOOK g_hook = NULL;
static HANDLE g_thread = NULL;
static DWORD g_thread_id = 0;
static volatile int g_running = 0;
static unsigned long long g_start_time = 0;
static unsigned long long g_last_event_time = 0;
static unsigned long long g_key_down_time[256] = {0};

static CRITICAL_SECTION g_cs;

static void (*g_callback)(Event* ev) = NULL;
static int g_poll_mode = 0;
static Event g_event_queue[EVENT_QUEUE_CAPACITY];
static int g_q_head = 0;
static int g_q_tail = 0;
static int g_q_count = 0;

static unsigned char g_blocked_keys[256] = {0};
static unsigned char g_blocked_groups[GROUP_COUNT] = {0};
static ComboNode* g_combo_head = NULL;

static int g_block_all = 0;
static int g_block_sim = 0;
static int g_block_phys = 0;

static int g_ignore_injected_for_listener = 0;
static int g_mod_state = 0;
static HANDLE g_init_event = NULL;


/*
 * q_push - Push input event into polling queue 
 * 
 * @ev: Pointer to Event struct
 * 
 * Pushes the information from the specified Event struct
 * into the polling queue.
 */
static void q_push(const Event* ev) {
    if(g_q_count >= EVENT_QUEUE_CAPACITY) {
        g_q_head = (g_q_head + 1) % EVENT_QUEUE_CAPACITY;
        g_q_count--;
    }
    g_event_queue[g_q_tail] = *ev;
    g_q_tail = (g_q_tail + 1) % EVENT_QUEUE_CAPACITY;
    g_q_count++;
}

/*
 * q_pop - Pop oldest input event from polling queue
 * 
 * @out: Pointer to output Event struct
 * 
 * Pops the oldest event from the polling queue into a provided Event struct.
 * 
 * Returns: 1 on success, 0 if queue is empty
 */
static int q_pop(Event* out) {
    if(g_q_count == 0) return 0;
    *out = g_event_queue[g_q_head];
    g_q_head = (g_q_head + 1) % EVENT_QUEUE_CAPACITY;
    g_q_count--;
    return 1;
}

/*
 * q_clear - Clears all entries from polling queue
 * 
 * Clears all of the entries that are currently in the polling queue.
 */
static void q_clear(void) {
    g_q_head = g_q_tail = g_q_count = 0;
}


/*
 * combo_add - Add combo to list
 * 
 * @mods: Array of modifier key codes
 * @mod_count: Number of modifiers in the array
 * @key: Primary key code of the combo
 * 
 * Create and populate a new ComboNode, then add it to the linked list of combos. 
 * Caller must hold CS.
 * 
 * Returns: 1 on success, 0 if node is invalid
 */
static int combo_add(const BYTE* mods, int mod_count, BYTE key) { 
    ComboNode* node = (ComboNode*)malloc(sizeof(ComboNode));
    if(!node) return 0;
    node->mods = (BYTE*)malloc((size_t)mod_count);
    if(!node->mods) { free(node); return 0; }
    for(int i = 0; i < mod_count; ++i) node->mods[i] = mods[i];
    node->mod_count = mod_count;
    node->key = key;
    node->next = g_combo_head;
    g_combo_head = node;
    return 1;
}

/*
 * combo_remove - Remove combo from list
 * 
 * @mods:
 * @mod_count:
 * @key:
 * 
 * Remove combo entry from list if it matches the one provided exactly. 
 * Matches are unlinked from list and have thier memory freed. 
 * Caller must hold CS.
 * 
 * Returns: Number of combos removed
 */
static int combo_remove(const BYTE* mods, int mod_count, BYTE key) {
    ComboNode* prev = NULL;
    ComboNode* cur = g_combo_head;
    int removed = 0;
    while(cur) {
        int same = 0;
        if(cur->key == key && cur->mod_count == mod_count) {
            same = 1;
            for(int i = 0; i < mod_count && same; ++i)
                if(cur->mods[i] != mods[i]) same = 0;
        }
        if(same) {
            ComboNode* next = cur->next;
            if(prev) prev->next = next; else g_combo_head = next;
            free(cur->mods);
            free(cur);
            removed++;
            cur = next;
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
    return removed;
}

/*
 * combo_clear - Clear all combos from list
 * 
 * Frees every node's modifier array, followed by the node itself. After all 
 * nodes are freed, the list head is set to NULL. Caller must hold CS.
 * 
 * Returns: nothing
 */
static void combo_clear(void) {
    ComboNode* cur = g_combo_head;
    while(cur) {
        ComboNode* n = cur->next;
        free(cur->mods);
        free(cur);
        cur = n;
    }
    g_combo_head = NULL;
}

/*
 * mods_down_match - Check if specified modifiers are pressed
 * 
 * @mods: Array of modifier key codes to check
 * @mod_count: Number of modifiers in the array
 * 
 * Uses GetAsyncKeyState to test each modifer in the array. If any modifier is 
 * not currently down, returns 0.
 * 
 * Returns: 1 if all modifiers are pressed, 0 otherwise
 */
static int mods_down_match(const BYTE* mods, int mod_count) {
    for(int i = 0; i < mod_count; ++i) {
        SHORT s = GetAsyncKeyState((int)mods[i]);
        if((s & 0x8000) == 0) return 0;
    }
    return 1;
}

// check if any combo matches
/*
 * combo_matches_event - Check if a kehy event matches registered combo
 * 
 * @vk: Virtual key code of event to test
 * 
 * Searches combo linked list to check if any combo node matches the given key.
 * A Match occurs if: 
 * - The node's primary key equals vk, AND
 * - The node has no modifiers OR all required modifiers are currently pressed.
 * 
 * Returns: 1 if a match is found, 0 otherwise
 */
static int combo_matches_event(BYTE vk) {
    ComboNode* cur = g_combo_head;
    while(cur) {
        if(cur->key == vk) {
            if(cur->mod_count == 0) return 1;
            if(mods_down_match(cur->mods, cur->mod_count)) return 1;
        }
        cur = cur->next;
    }
    return 0;
}

/*
 * lowlevel_proc - Low level keyboard hook proc
 * 
 * @nCode: Hook code
 * @wParam: Keyboard event identifier
 * @lParam: Pointer to a KBDLLHOOKSTRUCT
 * 
 * Registered with SetWindowsHookEx(WH_KEYBOARD_LL), and is invoked by the 
 * system on every keyboard event (press/release). Populates an 
 * Event struct and dispatches it to either the callback or the internal queue.
 * 
 * Returns: 1 if event is blocked, passes input and calls CallNextHookEx otherwise
 */
static LRESULT CALLBACK lowlevel_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    if(nCode < 0) return CallNextHookEx(g_hook, nCode, wParam, lParam); /* Negative hook code */

    /* Retrieve KBDLLHOOKSTRUCT */
    KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
    if(!k) return CallNextHookEx(g_hook, nCode, wParam, lParam);

    int pressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) ? 1 : 0;
    BYTE vk = (BYTE)k->vkCode;
    int injected = ((k->flags & LLKHF_INJECTED) != 0) ? 1 : 0;
    unsigned long long now = GetTickCount64();

    /* Populate Event struct */
    Event ev;
    ev.vk = vk;
    ev.scan = (int)k->scanCode;
    ev.pressed = pressed;
    ev.injected = injected;
    ev.time = (unsigned long)(now - g_start_time);
    ev.delta = (unsigned long)(now - g_last_event_time);
    g_last_event_time = now;
    if(pressed) {
        g_key_down_time[vk] = now;
        ev.held = 0;
    } else {
        if(g_key_down_time[vk] != 0) {
            ev.held = (unsigned long)(now - g_key_down_time[vk]);
        } else {
            ev.held = 0;
        }
        g_key_down_time[vk] = 0;
    }

    /* Retrieve modifier bitmask */
    switch(vk) {
        case VK_LSHIFT: case VK_RSHIFT:
            if(pressed) g_mod_state |= L_MOD_SHIFT;
            else g_mod_state &= ~L_MOD_SHIFT;
            break;
        case VK_LCONTROL: case VK_RCONTROL:
            if(pressed) g_mod_state |= L_MOD_CTRL;
            else g_mod_state &= ~L_MOD_CTRL;
            break;
        case VK_LMENU: case VK_RMENU:
            if(pressed) g_mod_state |= L_MOD_ALT;
            else g_mod_state &= ~L_MOD_ALT;
            break;
        case VK_LWIN: case VK_RWIN:
            if(pressed) g_mod_state |= L_MOD_WIN;
            else g_mod_state &= ~L_MOD_WIN;
            break;
    }
    ev.modifiers = g_mod_state;

    int should_block = 0;

    EnterCriticalSection(&g_cs);

    /* Blocking logic */
    if(g_block_all) {
        should_block = 1;
    } else {
        if(g_block_sim && injected) should_block = 1;
        if(g_block_phys && !injected) should_block = 1;
        if(!should_block && g_blocked_keys[vk]) should_block = 1;
        if(!should_block) {
            for(int gi = 0; gi < GROUP_COUNT; ++gi) {
                if(g_blocked_groups[gi]) {
                    if(vk_in_group(vk, (GroupId)gi)) { should_block = 1; break; }
                }
            }
        }
        if(!should_block && combo_matches_event(vk)) should_block = 1;
    }

    /* Block input */
    if(should_block) {
        LeaveCriticalSection(&g_cs);
        return 1;
    }

    if(g_poll_mode) {
        q_push(&ev); /* Push event into queue */
        LeaveCriticalSection(&g_cs);
    } else {
        void (*cb)(Event*) = g_callback;
        LeaveCriticalSection(&g_cs);
        if(cb) {
            cb(&ev);
        } else {
            EnterCriticalSection(&g_cs);
            q_push(&ev);
            LeaveCriticalSection(&g_cs);
        }
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

/*
 * listener_thread_proc - Install and run keyboard hook 
 * 
 * \@param: Unused
 * 
 * Sets up low-level keyboard hook using SetWindowsHookExA with WH_KEYBOARD_LL, 
 * then enters a Windows message loop to keep hook alive.
 * 
 * Returns: 0 on normal termination, 1 if hook could not be installed
 */
static DWORD WINAPI listener_thread_proc(LPVOID param) {
    (void)param;
    HINSTANCE hinst = GetModuleHandle(NULL);
    g_hook = SetWindowsHookExA(WH_KEYBOARD_LL, lowlevel_proc, hinst, 0);
    if(!g_hook) {
        SetLastError(ERROR_INVALID_FUNCTION);
        if(g_init_event) SetEvent(g_init_event);
        return 1;
    }
    
    if(g_init_event) SetEvent(g_init_event);
    
    MSG msg;
    while(GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
    return 0;
}


/*
 * listener_start - Enable listener functions
 * 
 * Triggers the init event and begins the listener thread.
 * 
 * Returns: 0 if successful, 1 otherwise
 */
int INPUTLIB_CALL listener_start(void) {
    EnterCriticalSection(&g_cs);

    if(g_running) {
        LeaveCriticalSection(&g_cs);
        SetLastError(ERROR_ALREADY_EXISTS);
        return 1;
    }

    /* Prepare init event for sync */
    g_init_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    if(!g_init_event) {
        LeaveCriticalSection(&g_cs);
        SetLastError(ERROR_OUTOFMEMORY);
        return 1;
    }

    /* Create thread */
    g_thread = CreateThread(NULL, 0, listener_thread_proc, NULL, 0, &g_thread_id);
    if(!g_thread) {
        CloseHandle(g_init_event);
        g_init_event = NULL;
        LeaveCriticalSection(&g_cs);
        SetLastError(ERROR_OUTOFMEMORY);
        return 1;
    }
    LeaveCriticalSection(&g_cs);

    /* Wait up to 3 seconds for hook install */
    DWORD wait = WaitForSingleObject(g_init_event, 3000);
    CloseHandle(g_init_event);
    g_init_event = NULL;

    if(wait != WAIT_OBJECT_0) {
        /* Thread didn't initalize properly in time */
        PostThreadMessageA(g_thread_id, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = NULL;
        SetLastError(ERROR_TIMEOUT);
        return 1;
    }

    EnterCriticalSection(&g_cs);
    g_running = 1;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_stop - Disable listener functions
 * 
 * Kills listener thread.
 * 
 * Returns: 0 if successful, 1 if already stopped
 */
int INPUTLIB_CALL listener_stop(void) {
    EnterCriticalSection(&g_cs);

    if(!g_running) {
        LeaveCriticalSection(&g_cs);
        SetLastError(ERROR_INVALID_OPERATION);
        return 1;
    }
    g_running = 0;

    LeaveCriticalSection(&g_cs);

    if(g_thread) {
        PostThreadMessageA(g_thread_id, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = NULL;
        g_thread_id = 0;
    }
    return 0;
}

/*
 * listener_flush - Clears all toggles and blocks
 * 
 * Flushes everything, including the callback pointer, queue, blocked key lists, 
 * and global block toggles.
 */
int INPUTLIB_CALL listener_flush(void) {
    EnterCriticalSection(&g_cs);

    g_callback = NULL;
    q_clear();

    memset(g_blocked_keys, 0, sizeof(g_blocked_keys));
    memset(g_blocked_groups, 0, sizeof(g_blocked_groups));
    combo_clear();

    g_block_all = 0;
    g_block_sim = 0;
    g_block_phys = 0;

    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_cbsub - Subscribe to callbacks
 * 
 * @cb: Callback function to receive populated Event structs
 * 
 * Subscribes to keyboard event callbacks with a function.
 * 
 * Returns: 0 if successful, 1 if polling is enabled
 */
int INPUTLIB_CALL listener_cbsub(void (*cb)(Event* ev)) {
    if(!cb) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }

    EnterCriticalSection(&g_cs);

    if(g_poll_mode) {
        LeaveCriticalSection(&g_cs);
        SetLastError(ERROR_INVALID_OPERATION); /* Can't use callback while polling */
        return 1;
    }

    g_callback = cb;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_ucbsub - Unsubscribe from callbacks
 * 
 * Unsubscribes from callback events.
 * 
 * Returns: 0 (always succeeds)
 */
int INPUTLIB_CALL listener_ucbsub(void) {
    EnterCriticalSection(&g_cs);
    g_callback = NULL;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_cbpollmode - Enable event polling
 * 
 * Enables or disables polling mode in place of function callbacks. 
 * Mutually exclusive with normal callback mode.
 * 
 * Returns: 0 if successful, 1 if currently subscribed to callbacks
 */
int INPUTLIB_CALL listener_cbpollmode(int enabled) {
    EnterCriticalSection(&g_cs);
    if(enabled) {
        if(g_callback) {
            LeaveCriticalSection(&g_cs);
            SetLastError(ERROR_INVALID_OPERATION);
            return 1; /* Callback mode cannot be active at the same time as polling */
        }
    g_poll_mode = 1;
    } else {
        g_poll_mode = 0;
    }
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_cbpoll - Poll next event
 * 
 * @out: Pointer to Event struct to populate
 * 
 * Pops next queued keyboard input event into provided Event struct.
 * 
 * Returns: -1 if out is invalid, 1 otherwise
 */
int INPUTLIB_CALL listener_cbpoll(Event* out) {
    if(!out) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    EnterCriticalSection(&g_cs);
    int ok = q_pop(out);
    LeaveCriticalSection(&g_cs);
    return ok;
}

/*
 * listener_cbdumppoll - Dump polling queue to buffer
 * 
 * @buffer: Empty string to dump to
 * @len: Length of buffer
 * 
 * Dumps the entirety of the polling queue to the buffer.
 * 
 * Returns: 0 if successful, 1 otherwise
 */
int INPUTLIB_CALL listener_cbdumppoll(char* buffer, size_t len) {
    if(!buffer || len == 0) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    EnterCriticalSection(&g_cs);
    size_t pos = 0;
    for(int i = 0; i < g_q_count; ++i) {
        int idx = (g_q_head + i) % EVENT_QUEUE_CAPACITY;
        Event* ev = &g_event_queue[idx];
        char line[128];
        int n = _snprintf(line, sizeof(line), "VK=0x%02X %s MOD=0x%02X INJ=%d TIME=%lu\n",
         ev->vk, ev->pressed ? "DOWN" : "UP", ev->modifiers, ev->injected, ev->time);
        if(n <= 0) continue;
        if(pos + (size_t)n + 1 >= len) break;
        memcpy(buffer + pos, line, (size_t)n);
        pos += (size_t)n;
    }
    buffer[pos] = '\0';
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_cbflush - Clear callback-related things
 * 
 * Flushes (clears) polling queue and callback pointer without clearing 
 * blocklists or global block toggles.
 * 
 * Returns: 0 (always succeeds)
 */
int INPUTLIB_CALL listener_cbflush(void) {
    EnterCriticalSection(&g_cs);
    q_clear();
    g_callback = NULL;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_block - Block key by name
 * 
 * @key: Name of the key to block
 * 
 * Blocks all input from the given key.
 * 
 * Returns: 0 on success, 1 if key name not found
 */
int INPUTLIB_CALL listener_block(const char* key) {
    if(!key) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    BYTE vk = find_vk(key);
    if(!vk) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    EnterCriticalSection(&g_cs);
    g_blocked_keys[vk] = 1;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_ublock - Unblock key by name
 * 
 * @key: Name of the key to unblock
 * 
 * Unblocks input from the given key.
 * 
 * Returns: 0 on success, 1 if key name not found
 */
int INPUTLIB_CALL listener_ublock(const char* key) {
    if(!key) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    BYTE vk = find_vk(key);
    if(!vk) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    EnterCriticalSection(&g_cs);
    g_blocked_keys[vk] = 0;
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_blockc - Block combo with 1 modifier
 * 
 * @mod: Name of modifier
 * @key: Name of primary key
 * 
 * Blocks input from combo using combo_add.
 * 
 * Returns: 0 on success, 1 if key name not found
 */
int INPUTLIB_CALL listener_blockc(const char* mod, const char* key) {
    if(!mod || !key) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    BYTE vm = find_vk(mod);
    BYTE vk = find_vk(key);
    if(!vm || !vk) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    EnterCriticalSection(&g_cs);
    BYTE mods[1] = { vm };
    int ok = combo_add(mods, 1, vk);
    LeaveCriticalSection(&g_cs);
    if(!ok) { SetLastError(ERROR_OUTOFMEMORY); return 1; }
    return 0;
}

/*
 * listener_ublockc - Unblocks combo with 1 modifier
 * 
 * @mod: Name of modifier
 * @key: Name of primary key
 * 
 * Unblocks input from combo using combo_remove.
 * 
 * Returns: 0 on success, 1 if key name not found or removal failed
 */
int INPUTLIB_CALL listener_ublockc(const char* mod, const char* key) {
    if(!mod || !key) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    BYTE vm = find_vk(mod);
    BYTE vk = find_vk(key);
    if(!vm || !vk) { SetLastError(ERROR_INVALID_PARAMETER); return 1; }
    EnterCriticalSection(&g_cs);
    int removed = combo_remove((BYTE*)&vm, 1, vk);
    LeaveCriticalSection(&g_cs);
    return removed ? 0 : 1;
}


/*
 * listener_isblocked - Check if a key is currently blocked
 * 
 * @key: Name of the key to check
 * 
 * Checks the blocked keys array followed by the blocked groups array to 
 * determine whether a key is currently blocked.
 * 
 * Returns: 1 if key is blocked, 0 otherwise, and -1 if key name not found
 */
int INPUTLIB_CALL listener_isblocked(const char* key) {
    if(!key) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    BYTE vk = find_vk(key);
    if(!vk) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    EnterCriticalSection(&g_cs);
    if(g_blocked_keys[vk]) { LeaveCriticalSection(&g_cs); return 1; }
    for(int gi = 0; gi < GROUP_COUNT; ++gi) {
        if(g_blocked_groups[gi] && vk_in_group(vk, (GroupId)gi)) { LeaveCriticalSection(&g_cs); return 1; }
    }
    LeaveCriticalSection(&g_cs);
    return 0;
}

/*
 * listener_keystate - Check if a key is currently pressed
 * 
 * @key: Name of the key to check
 * 
 * Queries the current state of a key to determine if it is being held down.
 * Uses GetAsyncKeyState to check the high-order bit which indicates if the 
 * key is currently pressed. Functionally identical to key_isdown.
 * 
 * Returns: 1 if key is down, 0 if key is up, -1 on error or key not found
 */
int INPUTLIB_CALL listener_keystate(const char* key) {
    if(!key) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    BYTE vk = find_vk(key);
    if(!vk) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    SHORT s = GetAsyncKeyState((int)vk);
    return (s & 0x8000) ? 1 : 0;
}

/*
 * listener_modstate - Check the modifier bitmask
 * 
 * @out_mask: Pointer to recieve the bitmask
 * 
 * Returns: 0 on success, -1 on invalid parameter
 */
int INPUTLIB_CALL listener_modstate(int* out_mask) {
    if(!out_mask) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
    *out_mask = g_mod_state;
    return 0;
}

/*
 * listener_init - Initialize listener
 * 
 * Initalizes global state and critical section. Any subsequent calls after the 
 * inital call will do nothing.
 * 
 * Returns: nothing
 */
void listener_init(void) {
    static int inited = 0;
    if(inited) return;
    InitializeCriticalSection(&g_cs);
    g_start_time = GetTickCount64();
    g_last_event_time = g_start_time;
    inited = 1;
}