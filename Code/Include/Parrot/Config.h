#define MAX_SCREENS                   2
#define MAX_ROOM_BACKDROPS            2
#define MAX_ROOM_EXITS                10
#define MAX_ROOM_ENTITIES             20
#define MAX_ENTITY_NAME_LENGTH        29
#define MAX_VIEW_LAYOUTS              2
#define MAX_INPUT_EVENT_SIZE          32

/* 
  Maximum number of globals for scripting.
  Value must be power-of-two
*/
#define MAX_SCRIPT_GLOBALS            64


#define MAX_ROOM_SCRIPTS              8
#define MAX_ENTITY_SCRIPTS            4
#define MAX_ENTITY_IMAGES             4
#define MAX_VIRTUAL_MACHINES          8
#define MAX_VM_STACK_SIZE             16
#define MAX_VM_VARIABLES              16
#define MAX_VM_GLOBALS                64
#define MAX_OPEN_ARCHIVES             8
#define DEFAULT_ARCHIVE_PATH_FSTR     "PROGDIR:Tools/%lld.parrot"

/*
  Maximum number of constants per script.
  Value must be power-of-two
*/
#define MAX_CONSTANTS_PER_SCRIPT      8
