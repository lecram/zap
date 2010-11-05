/* Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
 *
 * This file is part of zap.
 *
 * zap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with zap.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Errors (header) */

typedef enum {
    ZE_OK,
    ZE_OUT_OF_MEMORY,
    ZE_INDEX_OUT_OF_RANGE,
    ZE_NAME_NOT_DEFINED,
    ZE_FUNCTION_NAME_NOT_DEFINED,
    ZE_ARITY_ERROR,
    ZE_BREAK_WITHOUT_LOOP,
    ZE_CONTINUE_WITHOUT_LOOP,
    ZE_UNKNOWN_TYPE_NUMBER,
    ZE_OPEN_FILE_ERROR
} ZError;

void raise(char *msg);
void raisecpl(char *msg, char *srcname, unsigned int linum);
void raiseOutOfMemory(const char *caller);
void raiseUnknownTypeNumber(const char *caller, unsigned char tnum);
void raiseIndexOutOfRange(const char *caller, int index, int length);
void raiseNameNotDefined(const char *name);
void raiseFunctionNameNotDefined(const char *fname);
void raiseArityError(unsigned int passed,
                     unsigned char expected,
                     const char *fname);
void raiseOpenFileError(const char *name);
