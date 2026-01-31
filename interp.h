#ifndef INTERP_H
#define INTERP_H

#include "parser.h"

#define INTERP_ERR_MAX 256

int interp_run(ASTProgram *prog, char *err_buf, int err_size);

#endif
