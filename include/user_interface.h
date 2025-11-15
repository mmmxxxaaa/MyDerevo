#ifndef TREE_USER_INTERFACE_H_
#define TREE_USER_INTERFACE_H_

#include "tree_common.h"

typedef enum {
    CHOICE_FAILED = -1,
    CHOICE_LEAVE = 0,
    CHOICE_PLAY_AKINATOR,
    CHOICE_BASE_DUMP,
    CHOICE_HTM_DUMP,
    CHOICE_FIND_OBJ_DEFINITION,
    CHOICE_SAVE_TREE_TO_FILE,
    CHOICE_LOAD_TREE_FROM_FILE
} UserActionChoices;

const char* GetDataBaseFilename(int argc, const char** argv);
void        UserInterface(UserActionChoices* choice);
void        ChooseAction(Tree* tree, UserActionChoices choice, const char* filename);


#endif // TREE_USERT_INTERFACE_H_
