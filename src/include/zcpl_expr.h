/* Copyright 2010-2011 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/* Expression Compiler (header) */

int is_separator(char c);
void skip_space(char **str);
unsigned int write_uvlv(unsigned int n, signed char vlv[]);
unsigned int write_svlv(int n, signed char vlv[]);
unsigned int cpl_int(char **expr, char *bin);
char *escape_char(char e);
unsigned int cpl_asciibyte(char **expr, char *bin);
unsigned int cpl_bytearray(char **expr, char *bin);
unsigned int cpl_bignum(char **expr, char *bin);
unsigned int cpl_list(char **expr, char *bin);
unsigned int cpl_dict(char **expr, char *bin);
unsigned int cpl_func(char **expr, char *bin);
unsigned int cpl_expr(char **expr, char *bin);
