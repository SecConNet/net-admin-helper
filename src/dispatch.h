#pragma once

#define DISPATCH(FUNC, CMD) \
    if (!strcmp(CMD, #FUNC)) return FUNC(argc - 2, argv + 2)

